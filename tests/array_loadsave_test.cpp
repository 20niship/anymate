#include <cwchar>
#include <movutl/experimental/pickle.hpp>
#include <movutl/tester/tester.hpp>
#include <spdlog/spdlog.h>

using namespace mu::core;
using namespace mu::experimental;

template<typename T, int N=20>
void float_test() {
  const std::string fname = "tmp.bin";
  Vec<T> v;
  for(int i = 0; i < N; ++i) v.push_back(i);
  save_array(v, fname);
  Vec<T> l = load_array<T>(fname);

  TEST_EQ(l.size(), v.size());
  for(int i = 0; i < N; ++i)
    TEST_EQ(l[i], v[i]);
}

template<int N=20>
void vec3_test() {
  const std::string fname = "tmp.bin";
  Vec<Vec3> v;
  for(int i = 0; i < N; ++i) v.push_back(Vec3().all(i));
  save_array(v, fname);
  auto l = load_array<Vec3>(fname);

  TEST_EQ(l.size(), v.size());
  for(int i = 0; i < N; ++i)
    TEST_EQ(l[i], v[i]);
}

int main() {
  float_test<char, 20>();
  float_test<float, 30>();
  vec3_test<14>();
  return 0;
}
