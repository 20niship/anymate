#pragma once
#include <movutl/core/vector.hpp>

namespace mu::core {
template <typename T> const T& min(const T& a) { return a; }

template <typename T, typename... Args> const T& min(const T& a, const T& b, const Args&... args) { return min(b < a ? b : a, args...); }

template <typename T> const T& max(const T& a) { return a; }

template <typename T, typename... Args> const T& max(const T& a, const T& b, const Args&... args) { return max(b > a ? b : a, args...); }

template<typename T>
inline auto rad_to_deg(const T x) { return x * 180.0 / 3.141592; }
template<typename T>
inline auto deg_to_rad(const T x) { return x / 180.0 * 3.141592; }

Mat3x3 rotation_mat(const Vec3& pry);

Mat4x4 rotation_mat_4x4(const Vec3& pry);
Mat4x4 scale_mat(const Vec3& scale);
Mat4x4 translation_mat(const Vec3& pos);
Mat4x4 transform_mat(const Vec3& pos, const Vec3 &pry, const Vec3 &scale);

inline void lookAt(float ex, float ey, float ez, float tx, float ty, float tz, float ux, float uy, float uz, float* matrix) {
  float l;

  /*
  ** 下の l は 0 になることがあるから,
  ** 必要ならエラー処理を追加して.
  */

  /* z 軸 = e - t */
  tx         = ex - tx;
  ty         = ey - ty;
  tz         = ez - tz;
  l          = sqrtf(tx * tx + ty * ty + tz * tz); /* この l と, */
  matrix[2]  = tx / l;
  matrix[6]  = ty / l;
  matrix[10] = tz / l;

  /* x 軸 = u x z 軸 */
  tx        = uy * matrix[10] - uz * matrix[6];
  ty        = uz * matrix[2] - ux * matrix[10];
  tz        = ux * matrix[6] - uy * matrix[2];
  l         = sqrtf(tx * tx + ty * ty + tz * tz); /* この l. */
  matrix[0] = tx / l;
  matrix[4] = ty / l;
  matrix[8] = tz / l;

  /* y 軸 = z 軸 x x 軸 */
  matrix[1] = matrix[6] * matrix[8] - matrix[10] * matrix[4];
  matrix[5] = matrix[10] * matrix[0] - matrix[2] * matrix[8];
  matrix[9] = matrix[2] * matrix[4] - matrix[6] * matrix[0];

  /* 平行移動 */
  matrix[12] = -(ex * matrix[0] + ey * matrix[4] + ez * matrix[8]);
  matrix[13] = -(ex * matrix[1] + ey * matrix[5] + ez * matrix[9]);
  matrix[14] = -(ex * matrix[2] + ey * matrix[6] + ez * matrix[10]);

  /* 残り */
  matrix[3] = matrix[7] = matrix[11] = 0.0f;
  matrix[15]                         = 1.0f;
}

inline Mat4x4 look_at(Vec3 pos, Vec3 to, Vec3 up) {
  Mat4x4 matrix;

  const auto tz = pos - to;
  const auto l  = tz.norm();
  MU_ASSERT(l > 0);
  matrix[2]  = tz[0] / l;
  matrix[6]  = tz[1] / l;
  matrix[10] = tz[2] / l;

  /* x 軸 = u x z 軸 */
  const Vec3 tx = {
    up[0] * matrix[10] - up[2] * matrix[6],
    up[1] * matrix[2] - up[0] * matrix[10],
    up[2] * matrix[6] - up[1] * matrix[2],
  };
  const auto lx = tx.norm();
  MU_ASSERT(lx > 0);
  matrix[0]     = tx[0] / lx;
  matrix[4]     = tx[1] / lx;
  matrix[8]     = tx[2] / lx;

  /* y 軸 = z 軸 x x 軸 */
  matrix[1] = matrix[6] * matrix[8] - matrix[10] * matrix[4];
  matrix[5] = matrix[10] * matrix[0] - matrix[2] * matrix[8];
  matrix[9] = matrix[2] * matrix[4] - matrix[6] * matrix[0];

  /* 平行移動 */
  matrix[12] = -(pos[0] * matrix[0] + pos[1] * matrix[4] + pos[2] * matrix[8]);
  matrix[13] = -(pos[0] * matrix[1] + pos[1] * matrix[5] + pos[2] * matrix[9]);
  matrix[14] = -(pos[1] * matrix[2] + pos[1] * matrix[6] + pos[2] * matrix[10]);

  /* 残り */
  matrix[3] = matrix[7] = matrix[11] = 0.0f;
  matrix[15]                         = 1.0f;
  return matrix;
}


template<typename T>
std::array<_Vec<T, 3>, 2> get_vert_vec(const _Vec<T, 3> x){
  _Vec<T, 3> e1, e2;
  if(x[0] >= x[1] && x[0] >= x[2]) { // X軸に近い直線
    e1 = {x[1], x[0], x[2]};
    e2 = {x[2], x[1], x[0]};
  } else if(x[1] >= x[0] && x[1] >= x[2]) {
    e1 = {x[1], x[0], x[2]};
    e2 = {x[0], x[2], x[1]};
  } else {
    e1 = {x[2], x[1], x[0]};
    e2 = {x[0], x[2], x[1]};
  }

  e1 = x.cross(e1);
  e1 = e1.normalize();

  e2 = x.cross(e1);
  e2 = e2.normalize();

  return {e1, e2};
}

} // namespace mu::core
