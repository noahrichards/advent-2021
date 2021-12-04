#include <execution>
#include <fstream>
#include <numeric>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_replace.h"
#include "absl/strings/str_split.h"
#include "absl/strings/strip.h"
#include "absl/strings/substitute.h"
#include "absl/types/optional.h"
#include "glog/logging.h"

int SumWindow(int* start) { return start[0] + start[1] + start[2]; }

int main(int argc, char** argv) {
  google::InstallFailureSignalHandler();
  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = 1;

  std::ifstream file(argv[1]);
  CHECK(file);

  std::vector<int> depths;
  std::string line;
  while (std::getline(file, line)) {
    int depth;
    CHECK(absl::SimpleAtoi(line, &depth));
    depths.push_back(depth);
  }

  int last = depths[0];
  int increases = 0;
  for (int i = 1; i < depths.size(); ++i) {
    if (depths[i] > last) {
      ++increases;
    }
    last = depths[i];
  }

  LOG(INFO) << "PART 1: " << increases;
  increases = 0;
  last = SumWindow(&depths[0]);
  for (int i = 1; i < depths.size() - 2; ++i) {
    int sum = SumWindow(&depths[i]);
    if (sum > last) {
      ++increases;
    }
    last = sum;
  }
  LOG(INFO) << "PART 2: " << increases;
  return 0;
}
