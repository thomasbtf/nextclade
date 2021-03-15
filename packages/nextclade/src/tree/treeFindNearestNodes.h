#pragma once

#include <nextclade/nextclade.h>
#include <nextclade/private/nextclade_private.h>

#include "TreeNode.h"

namespace Nextclade {
  struct TreeFindNearestNodesResult {
    TreeNode nearestNode;
    std::vector<NucleotideSubstitution> privateMutations;
  };

  TreeFindNearestNodesResult treeFindNearestNode(
    const NextcladeResultIntermediate& analysisResult, const NucleotideSequence& rootSeq, const Tree& tree);
}// namespace Nextclade