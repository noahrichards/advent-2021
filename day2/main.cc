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

// Simple position, for part1.
struct Position {
  int x;
  int z;

  Position operator*(int i) const { return Position{x * i, z * i}; }
  Position operator+(const Position& other) const {
    return Position{x + other.x, z + other.z};
  }
};

// Position with aim component.
struct SubPosition {
  int x;
  int z;
  int aim;

  SubPosition Down(int i) const { return SubPosition{x, z, aim + i}; }
  SubPosition Up(int i) const { return SubPosition{x, z, aim - i}; }
  SubPosition Forward(int i) const {
    return SubPosition{x + i, z + aim * i, aim};
  }
};

typedef std::tuple<std::string, int> Movement;

absl::flat_hash_map<std::string, Position> kDirections = {
    {"forward", {1, 0}}, {"down", {0, 1}}, {"up", {0, -1}}};

int main(int argc, char** argv) {
  google::InstallFailureSignalHandler();
  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = 1;

  std::ifstream file(argv[1]);
  CHECK(file);

  std::vector<Movement> movements;
  std::string line;
  while (std::getline(file, line)) {
    std::vector<std::string> parts = absl::StrSplit(line, " ");
    int magnitude;
    CHECK(absl::SimpleAtoi(parts[1], &magnitude));
    movements.push_back({absl::AsciiStrToLower(parts[0]), magnitude});
  }

  {
    Position pos = {0, 0};
    for (const auto& [direction, magnitude] : movements) {
      pos = pos + kDirections[direction] * magnitude;
    }
    LOG(INFO) << "PART 1: " << pos.x * pos.z;
  }

  {
    SubPosition pos = {0, 0, 0};
    for (const auto& [direction, magnitude] : movements) {
      if (direction == "up") {
        pos = pos.Up(magnitude);
      } else if (direction == "down") {
        pos = pos.Down(magnitude);
      } else if (direction == "forward") {
        pos = pos.Forward(magnitude);
      }
    }
    LOG(INFO) << "PART 2: " << pos.x * pos.z;
  }

  return 0;
}
