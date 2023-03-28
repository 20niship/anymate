#include <movutl/mu.hpp>
#include <vector>

using namespace mu;
using namespace mu::core;
using namespace mu::db;


void test1() {
  mu::core::Vec<int> vec;
  assert(vec.size() == 0);

  vec.push_back(20);
  vec.push_back(10);
  assert(vec[0] == 20);
  assert(vec[1] == 10);
}


class A {
public:
  int x;
  A() {
    std::cout << "constructor called!" << std::endl;
    x = 10;
  }
  ~A() { std::cout << "destructor called!" << std::endl; }
};

void test2() {
  /* mu::core::Vec<A> as; */
  std::vector<A> as;
  {
    std::cout << "pushback" << std::endl;
    as.resize(1);
    as[0] = std::move(A());
    std::cout << "pushback" << std::endl;
    std::cout << as[0].x << std::endl << std::endl;
  }
  std::cout << "pushback done" << std::endl;
  std::cout << as[0].x << std::endl << std::endl;
}

void test3() {
  instance::init();
  ui::create_window("a", 10, 10);
  Vec<_MeshBase*> m;
  m.push_back(new Mesh());
  m.push_back(new MeshCol());
  m.push_back(new Mesh2D());
  m.clear();
}

int test4() {
  Vec<int> x = {0, 1, 2, 3, 4, 5};
  Vec<int> y = {6, 7, 8, 9, 10};
  Vec<int> z = {11, 12, 13};
  x.insert(x.end(), y.begin(), y.end());
  x.insert(x.end(), z.begin(), z.end());
  return 0;
}

int test5() {
  core::Vec<int> x{1, 2, 3, 4, 5, 6};
  DISP(x.size() == 6);
  core::Vec<int> y;
  y = {0, 1, 2, 3, 4, 5, 6};
  DISP(x.size() == 7);
  return 0;
}

int main() {
  test1();
  test2();
  test3();
  test4();
  test5();
  return 0;
}
