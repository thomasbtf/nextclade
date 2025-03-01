#include "parseMutation.h"

#include <fmt/format.h>
#include <nextclade/nextclade.h>
#include <nextclade/private/nextclade_private.h>
#include <utils/safe_cast.h>

#include <boost/algorithm/string.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <string>

namespace Nextclade {

  int parsePosition(const std::string& raw) {
    if (raw.empty()) {
      throw ErrorParseMutationInvalidPosition(raw);
    }

    try {
      const auto num = std::stoi(raw);
      return num - 1;// To 0-based indexing
    } catch (...) {
      throw ErrorParseMutationInvalidPosition(raw);
    }
  }

  Nucleotide parseNucleotide(const std::string& raw) {
    if (raw.size() != 1) {
      throw ErrorParseMutationInvalidNucleotide(raw);
    }

    try {
      const char c = safe_cast<char>(std::toupper(raw[0]));
      return toNucleotide(c);
    } catch (...) {
      throw ErrorParseMutationInvalidNucleotide(raw);
    }
  }

  Aminoacid parseAminoacid(const std::string& raw) {
    if (raw.size() != 1) {
      throw ErrorParseMutationInvalidAminoacid(raw);
    }

    try {
      const char c = safe_cast<char>(std::toupper(raw[0]));
      return charToAa(c);
    } catch (...) {
      throw ErrorParseMutationInvalidAminoacid(raw);
    }
  }


  NucleotideSubstitution parseMutation(const std::string& mut) {
    if (mut.size() < 3) {
      throw ErrorParseMutationInvalidFormat(mut);
    }

    using boost::xpressive::smatch;
    using boost::xpressive::sregex;

    // clang-format off
    const auto regex = sregex::compile(R"((?P<refNuc>[A-Z-])(?P<pos>\d{1,10})(?P<queryNuc>[A-Z-]))");
    // clang-format on

    const auto upper = boost::to_upper_copy(mut);

    smatch matches;
    if (!regex_match(upper, matches, regex)) {
      throw ErrorParseMutationInvalidFormat(mut);
    }

    const auto& refNucStr = std::string{matches["refNuc"]};
    const auto& posStr = std::string{matches["pos"]};
    const auto& queryNucStr = std::string{matches["queryNuc"]};

    const auto& refNuc = parseNucleotide(refNucStr);
    const auto& pos = parsePosition(posStr);
    const auto& queryNuc = parseNucleotide(queryNucStr);

    return NucleotideSubstitution{
      .ref = refNuc,
      .pos = pos,
      .qry = queryNuc,
      .pcrPrimersChanged = {},
      .aaSubstitutions = {},
      .aaDeletions = {},
    };
  }

  AminoacidSubstitutionWithoutGene parseAminoacidMutationWithoutGene(const std::string& mut) {
    if (mut.size() < 3) {
      throw ErrorParseAminoacidMutationInvalidFormat(mut);
    }

    using boost::xpressive::smatch;
    using boost::xpressive::sregex;

    const auto regex = sregex::compile(R"((?P<refAa>[A-Z-*])(?P<pos>\d{1,10})(?P<queryAa>[A-Z-*]))");

    const auto upper = boost::to_upper_copy(mut);

    smatch matches;
    if (!regex_match(upper, matches, regex)) {
      throw ErrorParseAminoacidMutationInvalidFormat(mut);
    }

    const auto& refAaStr = std::string{matches["refAa"]};
    const auto& posStr = std::string{matches["pos"]};
    const auto& queryAaStr = std::string{matches["queryAa"]};

    const auto& refAa = parseAminoacid(refAaStr);
    const auto& pos = parsePosition(posStr);
    const auto& queryAa = parseAminoacid(queryAaStr);

    return AminoacidSubstitutionWithoutGene{
      .ref = refAa,
      .pos = pos,
      .qry = queryAa,
    };
  }

  Nextclade::ErrorParseMutationInvalidNucleotide::ErrorParseMutationInvalidNucleotide(const std::string& mut)
      : ErrorNonFatal(fmt::format("When parsing mutation: Unable to parse nucleotide: \"{}\"", mut)) {}

  Nextclade::ErrorParseMutationInvalidPosition::ErrorParseMutationInvalidPosition(const std::string& posStr)
      : ErrorNonFatal(fmt::format("When parsing mutation: Unable to parse position: \"{:s}\"", posStr)) {}

  ErrorParseMutationInvalidFormat::ErrorParseMutationInvalidFormat(const std::string_view& mut)
      : ErrorNonFatal(
          fmt::format("When parsing mutation: Unable to parse mutation. The format is invalid: \"{:s}\"", mut)) {}

  ErrorParseMutationInvalidAminoacid::ErrorParseMutationInvalidAminoacid(const std::string& mut)
      : ErrorNonFatal(fmt::format("When parsing mutation: Unable to parse aminoacid: \"{}\"", mut)) {}

  ErrorParseAminoacidMutationInvalidFormat::ErrorParseAminoacidMutationInvalidFormat(const std::string_view& mut)
      : ErrorNonFatal(
          fmt::format("When parsing mutation: Unable to parse mutation. The format is invalid: \"{:s}\"", mut)) {}
}// namespace Nextclade
