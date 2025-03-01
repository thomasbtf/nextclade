#pragma once

#include <nextclade/nextclade.h>

#include <algorithm>
#include <optional>
#include <tuple>
#include <utility>

#include "contract.h"

/**
 * Checks is a number x belongs to a half-open range [from; to)
 */
inline constexpr bool inRange(int x, int from, int to) {
  return x >= from && x < to;
}

/**
 * Checks is a number x belongs to a half-open range [from; to)
 */
inline constexpr bool inRange(int x, const Range& range) {
  return x >= range.begin && x < range.end;
}

/**
 * Finds overlap of 2 half-open ranges [from; to)
 * (version with 4 integers)
 *
 * Returns a pair [begin, end) of the overlap boundaries.
 * If there's no overlap, returns std::nullopt.
 */
inline std::optional<std::pair<int, int>> intersection(int begin1, int end1, int begin2, int end2) {
  precondition_less(begin1, end1);
  precondition_less(begin2, end2);

  if (begin2 >= end1 || begin1 >= end2) {
    return std::optional<std::pair<int, int>>{};
  }

  return std::make_pair(std::max(begin1, begin2), std::min(end1, end2));
}

/**
 * Finds overlap of 2 half-open ranges [from; to)
 * (version with 2 `std::pair`s)
 *
 * Returns an std::optional of std::pair containing half-open range [begin, end) of the overlap.
 * If there's no overlap, returns std::nullopt.
 */
inline std::optional<std::pair<int, int>> intersection(std::pair<int, int> x, std::pair<int, int> y) {
  const auto& begin1 = x.first;
  const auto& end1 = x.second;
  const auto& begin2 = y.first;
  const auto& end2 = y.second;
  return intersection(begin1, end1, begin2, end2);
}

/**
 * Finds overlap of 2 half-open ranges [from; to)
 * (version with 2 `Range`s)
 *
 * Returns an std::optional of std::pair containing half-open range [begin, end) of the overlap.
 * If there's no overlap, returns std::nullopt.
 */
inline std::optional<std::pair<int, int>> intersection(const Range& x, const Range& y) {
  return intersection(x.begin, x.end, y.begin, y.end);
}

/**
 * Finds whether of 2 half-open ranges [from; to) intersect
 * (version with 4 integers)
 */
inline bool hasIntersection(int begin1, int end1, int begin2, int end2) {
  return static_cast<bool>(intersection(begin1, end1, begin2, end2));
}

/**
 * Finds whether of 2 half-open ranges [from; to) intersect
 * (version with 2 `std::pair`s)
 */
inline bool hasIntersection(std::pair<int, int> x, std::pair<int, int> y) {
  return static_cast<bool>(intersection(x, y));
}

/**
 * Finds whether of 2 half-open ranges [from; to) intersect
 * (version with 2 `Range`s)
 */
inline bool hasIntersection(const Range& x, const Range& y) {
  return static_cast<bool>(intersection(x, y));
}
