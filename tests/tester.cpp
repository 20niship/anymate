#include <movutl/tester/tester.hpp>

int main() {
  TEST_EQ(1, 1);
  int x = 1;
  int y = 2;
  TEST_EQ(x, y);
  return 0;
}
