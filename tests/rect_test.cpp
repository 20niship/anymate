#include <movutl/core/rect.hpp>
#include <movutl/tester/tester.hpp>

int main(){
  mu::core::Rect3D r;
  DISP(r);
  r.expand(mu::core::Vec3{-1, -1, -1});
  DISP(r);


  float max = std::numeric_limits<float>::lowest();
  DISP(max);
  DISP(-1 > max);
  max = std::max<float>(-1, max);
  DISP(max);

  return 0;
}
