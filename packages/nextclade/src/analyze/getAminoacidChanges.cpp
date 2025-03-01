#include "getAminoacidChanges.h"

#include <fmt/format.h>
#include <nextclade/nextclade.h>
#include <nextclade/private/nextclade_private.h>

#include <boost/algorithm/string/join.hpp>
#include <vector>

#include "../utils/contract.h"
#include "../utils/mapFind.h"
#include "../utils/safe_cast.h"

namespace {
  std::vector<std::string> surroundWithQuotes(const std::vector<std::string>& m) {
    std::vector<std::string> result;
    std::transform(m.cbegin(), m.cend(), std::back_inserter(result),
      [](const auto& x) { return fmt::format("\"{}\"", x); });
    return result;
  }
}// namespace

namespace Nextclade {
  /**
   * Finds aminoacid substitutions and deletions in query peptides relative to reference peptides, in one gene
   *
   * NOTE: Nucleotide sequences and peptides are required to be stripped from insertions
   *
   * Implementation details: We compare reference and query peptides (extracted by the preceding call to Nextalign),
   * one aminoacid at at time, and deduce changes. We then report the change and relevant nucleotide context surrounding
   * this change.
   * Previously we reported one-to-one mapping of aminoacid changes to corresponding nucleotide changes. However, it
   * was not always accurate, because if there are multiple nucleotide changes in a codon, the direct correspondence
   * might not always be established without knowing the order in which nucleotide changes have occurred. And in the
   * context of Nextclade we don't have this information.
   */
  void getAminoacidChangesForGene(                      //
    const NucleotideSequence& ref,                      //
    const NucleotideSequence& query,                    //
    const AminoacidSequence& refPeptide,                //
    const AminoacidSequence& queryPeptide,              //
    const Gene& gene,                                   //
    const Range& alignmentRange,                        //
    std::vector<AminoacidSubstitution>& aaSubstitutions,//
    std::vector<AminoacidDeletion>& aaDeletions         //
  ) {
    precondition_equal(queryPeptide.size(), refPeptide.size());
    precondition_equal(query.size(), ref.size());

    const auto numNucs = safe_cast<int>(query.size());
    const auto numCodons = safe_cast<int>(queryPeptide.size());
    for (int codon = 0; codon < numCodons; ++codon) {
      invariant_greater_equal(codon, 0);
      invariant_less(codon, refPeptide.size());
      invariant_less(codon, queryPeptide.size());

      const auto& refAa = refPeptide[codon];
      const auto& queryAa = queryPeptide[codon];

      // Find where the codon is in nucleotide sequences
      const auto codonBegin = gene.start + codon * 3;
      const auto codonEnd = codonBegin + 3;

      if (!alignmentRange.contains(codonBegin) || !alignmentRange.contains(codonEnd)) {
        continue;
      }

      invariant_greater_equal(codonBegin, 0);
      invariant_greater_equal(codonBegin, gene.start);
      invariant_less(codonBegin, gene.end);
      invariant_greater_equal(codonEnd, codonBegin);

      // Provide surrounding context in nucleotide sequences: 1 codon to the left and 1 codon to the right
      const auto contextBegin = std::clamp(codonBegin - 3, 0, numNucs);
      const auto contextEnd = std::clamp(codonEnd + 3, 0, numNucs);
      const auto contextLength = contextEnd - contextBegin;

      invariant_greater_equal(contextBegin, 0);
      invariant_less(contextEnd, ref.size());
      invariant_less(contextEnd, query.size());
      invariant_less(contextEnd, numNucs);
      invariant_greater_equal(contextEnd, contextBegin);
      invariant_greater_equal(contextLength, 0);
      invariant_less_equal(contextLength, 9);

      NucleotideSequence refContext = ref.substr(contextBegin, contextLength);
      NucleotideSequence queryContext = query.substr(contextBegin, contextLength);

      if (isGap(queryAa)) {
        // Gap in the ref sequence means that this is a deletion in the query sequence
        aaDeletions.emplace_back(AminoacidDeletion{
          .gene = gene.geneName,
          .ref = refAa,
          .pos = codon,
          .codonNucRange = Range{.begin = codonBegin, .end = codonEnd},
          .refContext = std::move(refContext),
          .queryContext = std::move(queryContext),
          .contextNucRange = Range{.begin = contextBegin, .end = contextEnd},
          .nucSubstitutions = {},
          .nucDeletions = {},
        });
      } else {
        // TODO: we might account for ambiguous aminoacids in this condition
        if (queryAa != refAa && queryAa != Aminoacid::X) {
          // If not a gap and the state has changed, than it's a substitution
          aaSubstitutions.emplace_back(AminoacidSubstitution{
            .gene = gene.geneName,
            .ref = refAa,
            .pos = codon,
            .qry = queryAa,
            .codonNucRange = Range{.begin = codonBegin, .end = codonEnd},
            .refContext = std::move(refContext),
            .queryContext = std::move(queryContext),
            .contextNucRange = Range{.begin = contextBegin, .end = contextEnd},
            .nucSubstitutions = {},
            .nucDeletions = {},
          });
        }
      }
    }
  }

  /**
   * Finds aminoacid substitutions and deletions in query peptides relative to reference peptides, in all genes
   *
   * NOTE: Nucleotide sequences and peptides are required to be stripped from insertions
   */
  AminoacidChangesReport getAminoacidChanges(                    //
    const NucleotideSequence& ref,                               //
    const NucleotideSequence& query,                             //
    const std::map<std::string, RefPeptideInternal>& refPeptides,//
    const std::vector<PeptideInternal>& queryPeptides,           //
    const Range& alignmentRange,                                 //
    const GeneMap& geneMap                                       //
  ) {
    std::vector<AminoacidSubstitution> aaSubstitutions;
    std::vector<AminoacidDeletion> aaDeletions;

    for (const auto& queryPeptide : queryPeptides) {
      const auto& geneName = queryPeptide.name;
      auto found = mapFind(refPeptides, geneName);
      if (!found) {
        throw ErrorRefPeptideNotFound(geneName);
      }
      const auto& refPeptide = found->peptide;
      invariant_equal(refPeptide.size(), queryPeptide.seq.size());

      const auto gene = mapFind(geneMap, geneName);
      if (!gene) {
        throw ErrorGeneNotFound(geneName, geneMap);
      }

      getAminoacidChangesForGene(//
        ref,                     //
        query,                   //
        refPeptide,              //
        queryPeptide.seq,        //
        *gene,                   //
        alignmentRange,          //
        aaSubstitutions,         //
        aaDeletions              //
      );
    }

    return {
      .aaSubstitutions = aaSubstitutions,
      .aaDeletions = aaDeletions,
    };
  }

  ErrorRefPeptideNotFound::ErrorRefPeptideNotFound(const std::string& name)
      : ErrorNonFatal(fmt::format(//
          "When searching for aminoacid mutations: peptide \"{:s}\" was requested, but was not found among reference "
          "peptides. This is an internal issue. Please report this to developers, providing data and parameters you "
          "used, in order to replicate the error.",
          name)) {}

  ErrorGeneNotFound::ErrorGeneNotFound(const std::string& geneName, const GeneMap& geneMap)
      : ErrorNonFatal(fmt::format(//
          "When searching for aminoacid mutations: gene \"{:s}\" was requested, but was not found in the gene map. The "
          "genes present in the gene map were: {}. This is an internal issue. Please report this to developers, "
          "providing data and parameters you used, in order to replicate the error.",
          geneName, boost::join(surroundWithQuotes(keys(geneMap)), ", "))) {}
}// namespace Nextclade
