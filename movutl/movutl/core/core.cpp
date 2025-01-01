#include <cmath>
#include <movutl/core/core.hpp>
#include <opencv2/core/persistence.hpp>

namespace mu::core {

using namespace mu::core;

Mat4x4 rotation_mat_4x4(const Vec3& pry) {
  Mat4x4 p, r, y;
  // clang-format off
  p << 1,0,0,0,
    0,std::cos(pry[0]) , -std::sin(pry[0]),0, 
    0,std::sin(pry[0]) , std::cos(pry[0]),0, 
    0,0,0,1;
  p(0,0) = 1;

  r << std::cos(pry[1]), 0,-std::sin(pry[1]),0,
    0,1,0,0,
    std::sin(pry[1]),0,std::cos(pry[1]),0,
    0,0,0,1;
  r(0,0) = std::cos(pry[1]);

  y << std::cos(pry[2]) , -std::sin(pry[2]),0,0,
    std::sin(pry[2]), std::cos(pry[2]), 0,0,
    0,0,1,0,
    0,0,0,1;
  y(0,0) = std::cos(pry[2]);


  // clang-format on
  return p * r * y;
}

Mat4x4 scale_mat(const Vec3& scale) {
  Mat4x4 s;
  // clang-format off
  s << scale[0],0,0,0,
       0,scale[0],0,0,
       0,0,scale[0],0,
       0,0,0,1;
  // clang-format on
  return s;
}

Mat4x4 translation_mat(const Vec3& pos){
  Mat4x4 s;
  // clang-format off
  s << 1,0,0,pos[0],
       0,1,0,pos[1],
       0,0,1,pos[2],
       0,0,0,1;
  // clang-format on
  return s;
}

Mat4x4 transform_mat(const Vec3& pos, const Vec3& pry, const Vec3& scale){
  return rotation_mat_4x4(pry) * scale_mat(scale) * translation_mat(pos);
}

} // namespace mu::core
