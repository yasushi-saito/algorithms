
#include "base/chunked_queue.h"
#include "third_party/benchmark/benchmark.h"
#include "third_party/gtest/gtest.h"

namespace c3d {

TEST(ChunkedQueue, Basic) {
  ChunkedQueue<int> q;
  ASSERT_EQ(0, q.size());
  ASSERT_TRUE(q.empty());

  q.push_back(1);
  ASSERT_EQ(1, q.size());
  ASSERT_FALSE(q.empty());
  q.push_back(2);
  q.push_back(3);
  ASSERT_EQ(3, q.size());
  ASSERT_EQ(3, q.back());
  q.pop_back();
  ASSERT_EQ(2, q.back());
  q.pop_back();
  ASSERT_EQ(1, q.back());
  q.pop_back();
  ASSERT_TRUE(q.empty());
}

template<typename T>
void BM_Basic_String(benchmark::State& state) {
  const int n = state.range_x();
  const std::string data("foohah");
  while (state.KeepRunning()) {
    T container;
    for (int i = 0; i < n; ++i) {
      container.push_back(data);
    }
    for (int i = 0; i < n; ++i) {
      container.pop_back();
    }
  }
}

BENCHMARK_TEMPLATE(BM_Basic_String, std::vector<std::string>)
    ->Arg(1)
    ->Arg(16)
    ->Arg(1024);
BENCHMARK_TEMPLATE(BM_Basic_String, ChunkedQueue<std::string>)
    ->Arg(1)
    ->Arg(16)
    ->Arg(1024);

template<typename T>
void BM_Basic_Int(benchmark::State& state) {
  const int n = state.range_x();
  while (state.KeepRunning()) {
    T container;
    for (int i = 0; i < n; ++i) {
      container.push_back(i);
    }
    for (int i = 0; i < n; ++i) {
      container.pop_back();
    }
  }
}

BENCHMARK_TEMPLATE(BM_Basic_Int, std::vector<int>)->Arg(1)->Arg(16)->Arg(1024);
BENCHMARK_TEMPLATE(BM_Basic_Int, ChunkedQueue<int>)->Arg(1)->Arg(16)->Arg(1024);

}  // namespace c3d
