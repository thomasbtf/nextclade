#include <fmt/format.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nextalign/nextalign.h>

#include <boost/algorithm/string/join.hpp>

#include "../src/io/gene.io.h"
#include "data/sampleGeneMap.h"

using ExpectedResults = std::vector<GeneMap::value_type>;


std::string makeGffLine(const Gene& gene) {
  const auto attribute = fmt::format(R"( gene_name "{:s}" )", gene.geneName);

  const std::vector<std::string> line = {
    /* seqname */ ".",
    /* source */ ".",
    /* feature */ "gene",
    /* start */ std::to_string(gene.start),
    /* end */ std::to_string(gene.end),
    /* score */ ".",
    /* strand */ "+",
    /* frame */ std::to_string(gene.frame),
    /* attribute */ attribute,
  };

  return boost::algorithm::join(line, "\t");
}

std::string makeGff(const std::vector<Gene>& genes) {
  std::vector<std::string> lines;
  std::transform(genes.begin(), genes.end(), std::back_inserter(lines), makeGffLine);
  return boost::algorithm::join(lines, "\n") + "\n";
}


TEST(parseGeneMapGff, ParsesGeneMapWithAttributesWithEquals) {
  std::stringstream input;
  input << R"(
.	.	gene	26245	26472	.	+	.	 gene_name=E
.	.	gene	26523	27191	.	+	.	 gene_name=M
.	.	gene	28274	29533	.	+	.	 gene_name=N
.	.	gene	29558	29674	.	+	.	 gene_name=ORF10
.	.	gene	28734	28955	.	+	.	 gene_name=ORF14
.	.	gene	266	13468	.	+	.	 gene_name=ORF1a
.	.	gene	13468	21555	.	+	.	 gene_name=ORF1b
.	.	gene	25393	26220	.	+	.	 gene_name=ORF3a
.	.	gene	27202	27387	.	+	.	 gene_name=ORF6
.	.	gene	27394	27759	.	+	.	 gene_name=ORF7a
.	.	gene	27756	27887	.	+	.	 gene_name=ORF7b
.	.	gene	27894	28259	.	+	.	 gene_name=ORF8
.	.	gene	28284	28577	.	+	.	 gene_name=ORF9b
.	.	gene	21563	25384	.	+	.	 gene_name=S
)";

  const auto results = parseGeneMapGff(input);

  EXPECT_EQ(results.size(), sampleGeneMap.size());
  EXPECT_THAT(results, testing::UnorderedElementsAreArray(sampleGeneMap));
}

TEST(parseGeneMapGff, ParsesGeneMapWithAttributesWithSpacesAndQuotes) {
  std::stringstream input;
  input << R"(
.	.	gene	26245	26472	.	+	.	 gene_name "E"
.	.	gene	26523	27191	.	+	.	 gene_name "M"
.	.	gene	28274	29533	.	+	.	 gene_name "N"
.	.	gene	29558	29674	.	+	.	 gene_name "ORF10"
.	.	gene	28734	28955	.	+	.	 gene_name "ORF14"
.	.	gene	266	13468	.	+	.	 gene_name "ORF1a"
.	.	gene	13468	21555	.	+	.	 gene_name "ORF1b"
.	.	gene	25393	26220	.	+	.	 gene_name "ORF3a"
.	.	gene	27202	27387	.	+	.	 gene_name "ORF6"
.	.	gene	27394	27759	.	+	.	 gene_name "ORF7a"
.	.	gene	27756	27887	.	+	.	 gene_name "ORF7b"
.	.	gene	27894	28259	.	+	.	 gene_name "ORF8"
.	.	gene	28284	28577	.	+	.	 gene_name "ORF9b"
.	.	gene	21563	25384	.	+	.	 gene_name "S"
)";

  const auto results = parseGeneMapGff(input);

  EXPECT_EQ(results.size(), sampleGeneMap.size());
  EXPECT_THAT(results, testing::UnorderedElementsAreArray(sampleGeneMap));
}

TEST(parseGeneMapGff, ParsesGeneMapWithMultipleAttributes) {
  std::stringstream input;
  input << R"(
.	.	gene	26245	26472	.	+	.	 gene_name=E;foo=42;bar=0
.	.	gene	26523	27191	.	+	.	 gene_name=M;foo=42;bar=1
.	.	gene	28274	29533	.	+	.	 gene_name=N;foo=42;bar=2
.	.	gene	29558	29674	.	+	.	 gene_name=ORF10;foo=42;bar=3
.	.	gene	28734	28955	.	+	.	 gene_name=ORF14;foo=42;bar=4
.	.	gene	266	13468	.	+	.	 gene_name=ORF1a;foo=42;bar=5
.	.	gene	13468	21555	.	+	.	 gene_name=ORF1b;foo=42;bar=6
.	.	gene	25393	26220	.	+	.	 gene_name=ORF3a;foo=42;bar=7
.	.	gene	27202	27387	.	+	.	 gene_name=ORF6;foo=42;bar=8
.	.	gene	27394	27759	.	+	.	 gene_name=ORF7a;foo=42;bar=9
.	.	gene	27756	27887	.	+	.	 gene_name=ORF7b;foo=42;bar=10
.	.	gene	27894	28259	.	+	.	 gene_name=ORF8;foo=42;bar=11
.	.	gene	28284	28577	.	+	.	 gene_name=ORF9b;foo=42;bar=12
.	.	gene	21563	25384	.	+	.	 gene_name=S;foo=42;bar=13
)";

  const auto results = parseGeneMapGff(input);

  EXPECT_EQ(results.size(), sampleGeneMap.size());
  EXPECT_THAT(results, testing::UnorderedElementsAreArray(sampleGeneMap));
}

TEST(parseGeneMapGff, IgnoresComments) {
  std::stringstream input;

  // clang-format off
  input << fmt::format(R"( #  This is a comment
# Another comment
  # Indented comment
.	.	gene	28274	29533	.	+	.	 gene_name=N
.	.	gene	29558	29674	.	+	.	 gene_name=ORF10
# In-body comment
.	.	gene	21563	25384	.	+	.	 gene_name=S
)");
  // clang-format on

  const auto results = parseGeneMapGff(input);

  const ExpectedResults expected = {//
    {"N",
      Gene{
        .geneName = "N",
        .start = 28273,
        .end = 29533,
        .strand = "+",
        .frame = 1,
        .length = 1260,
      }},
    {"ORF10",//
      Gene{
        .geneName = "ORF10",
        .start = 29557,
        .end = 29674,
        .strand = "+",
        .frame = 1,
        .length = 117,
      }},
    {"S",//
      Gene{
        .geneName = "S",
        .start = 21562,
        .end = 25384,
        .strand = "+",
        .frame = 1,
        .length = 3822,
      }}};

  EXPECT_EQ(results.size(), expected.size());
  EXPECT_THAT(results, testing::UnorderedElementsAreArray(expected));
}
