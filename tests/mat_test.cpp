#include <movutl/core/vector.hpp>

using namespace mu::core;


void test1() {
  Mat4x4 m;
  m << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16;
  MU_ASSERT(m(0,0) == 1);
  DISP(m);
  DISP(m(0,0));
  Vec4 x(10, 20, 30, 40);
  const auto res = m * x;
  DISP(res == Vec4(300, 700, 1100, 1500));
}

int main() {
  test1();
  return 0;
}
