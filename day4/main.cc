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

typedef std::vector<int> RowT;

struct Board {
  explicit Board(std::vector<RowT> rows)
      : rows_(rows), row_counts_(5, 0), col_counts_(5, 0) {}
  void Pick(int number) {
    for (int row = 0; row < 5; ++row) {
      for (int col = 0; col < 5; ++col) {
        if (number == rows_[row][col]) {
          rows_[row][col] = -1;
          if (++row_counts_[row] == 5) {
            won_ = true;
          }
          if (++col_counts_[col] == 5) {
            won_ = true;
          }
          return;
        }
      }
    }
  }

  int Score() const {
    int sum = 0;
    for (const RowT& row : rows_) {
      for (int val : row) {
        if (val > 0) {
          sum += val;
        }
      }
    }
    return sum;
  }
  bool is_won() const { return won_; }

 private:
  std::vector<RowT> rows_;
  std::vector<int> row_counts_;
  std::vector<int> col_counts_;
  bool won_ = false;
};

namespace {

RowT ParseRow(std::string line, std::string separator) {
  RowT row;
  int as_integer;
  for (absl::string_view val :
       absl::StrSplit(line, separator, absl::SkipEmpty())) {
    CHECK(absl::SimpleAtoi(val, &as_integer)) << "Invalid: " << val;
    row.push_back(as_integer);
  }
  return row;
}

};  // namespace

int main(int argc, char** argv) {
  google::InstallFailureSignalHandler();
  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = 1;

  std::ifstream file(argv[1]);
  CHECK(file);

  std::string line;
  CHECK(std::getline(file, line));
  std::vector<int> moves = ParseRow(line, ",");

  std::vector<Board> boards;
  std::vector<RowT> current_rows;
  while (std::getline(file, line)) {
    if (line.empty()) {
      if (!current_rows.empty()) {
        boards.emplace_back(current_rows);
      }
      current_rows.clear();
    } else {
      current_rows.push_back(ParseRow(line, " "));
    }
  }
  if (!current_rows.empty()) {
    boards.emplace_back(current_rows);
  }

  {
    int first_win = 0;
    int last_win = 0;
    std::vector<Board> current_boards = boards;
    int total_wins = 0;
    for (int move : moves) {
      for (Board& board : current_boards) {
        if (!board.is_won()) {
          board.Pick(move);
          if (board.is_won()) {
            if (first_win == 0) {
              first_win = board.Score() * move;
            }
            if (++total_wins == boards.size()) {
              // Last winner!
              last_win = board.Score() * move;
            }
          }
        }
      }
      if (last_win > 0) {
        break;
      }
    }
    LOG(INFO) << "PART 1: " << first_win;
    LOG(INFO) << "PART 2: " << last_win;
  }
  return 0;
}