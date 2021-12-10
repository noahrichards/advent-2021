#include <execution>
#include <fstream>
#include <numeric>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/debugging/failure_signal_handler.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_replace.h"
#include "absl/strings/str_split.h"
#include "absl/strings/strip.h"
#include "absl/strings/substitute.h"
#include "absl/types/optional.h"
#include "glog/logging.h"

// Inverts the least-significant |digit_count| bits of val.
uint32_t Invert(uint32_t val, int digit_count) {
  return ~val & ~(0xffffffff << digit_count);
}

// Returns a value of |digit_count| size where each bit is the most-commonly-set
// bit in vals. Ties are counted as 1s.
uint32_t CalculateMcs(std::vector<uint32_t> vals, int digit_count) {
  std::vector<int> bit_sums(digit_count, 0);
  for (uint32_t val : vals) {
    for (int i = 0; i < digit_count; ++i) {
      uint32_t digit = val & 1;
      val = (val >> 1);
      bit_sums[i] += digit ? 1 : -1;
    }
  }
  uint32_t mcs = 0;
  for (int i = 0; i < digit_count; ++i) {
    mcs |= (bit_sums[i] >= 0 ? 1 : 0) << i;
  }
  return mcs;
}

// Returns values that are equal to |test| under the given mask.
std::vector<uint32_t> FilterBy(std::vector<uint32_t> vals, uint32_t test,
                               uint32_t mask) {
  test &= mask;
  std::vector<uint32_t> equal;
  for (uint32_t val : vals) {
    if ((val & mask) == test) {
      equal.push_back(val);
    }
  }
  return equal;
}

uint32_t FindMcsMatch(std::vector<uint32_t> vals, int digit_count,
                      bool inverted) {
  // The mask and most-commonly-set bits. Each time we calculate only |digit|'s
  // mcs value from the remaining set of vals.
  uint32_t mask = 0;
  uint32_t mcs = 0;
  for (int digit = digit_count - 1; vals.size() != 1 && digit >= 0; --digit) {
    // The MCS of *this* set, which may produce different values for earlier
    // MCS calculations, so we grab just the bit we want below.
    uint32_t current_mcs = CalculateMcs(vals, digit_count);
    if (inverted) current_mcs = Invert(current_mcs, digit_count);
    mask |= 1 << digit;
    mcs |= (1 << digit) & current_mcs;
    vals = FilterBy(vals, mcs, mask);
  }
  CHECK(vals.size() == 1);
  return vals[0];
}

int main(int argc, char** argv) {
  absl::InstallFailureSignalHandler({});
  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = 1;

  std::ifstream file(argv[1]);
  CHECK(file);

  int digit_count = 0;
  std::vector<uint32_t> vals;
  std::string line;
  while (std::getline(file, line)) {
    uint32_t val;
    CHECK(absl::numbers_internal::safe_strtoi_base(line, &val, 2));
    vals.push_back(val);
    digit_count = line.size();
  }

  {
    // For part 1, look at all bit stats.
    uint32_t mcs = CalculateMcs(vals, digit_count);
    uint32_t lcs = Invert(mcs, digit_count);
    LOG(INFO) << "PART 1: " << mcs * lcs;
  }
  {
    uint32_t mcs = FindMcsMatch(vals, digit_count, /*inverted=*/false);
    uint32_t lcs = FindMcsMatch(vals, digit_count, /*inverted=*/true);

    // For part two, from highest-order-bit to lowest order, we generate a
    // mask and only take elements that match the mask.
    LOG(INFO) << "PART 2: " << mcs * lcs;
  }
  return 0;
}
