#pragma once
#include <cwchar>
#include <movutl/core/vector.hpp>
#include <tuple>

namespace mu::Magi {

// @params: plane: 平面の方程式$ax + by+cz + d = 0$　のabcd
inline std::tuple<mu::core::Vec2, double> project(const mu::core::Vec3& point, const mu::core::Vec4& plane) {
  const double l    = std::abs(point[0] * plane[0] + point[1] * plane[1] + point[2] + plane[2] + plane[3]);
  auto normal       = mu::core::Vec3(plane[0], plane[1], plane[2]);
  const double n    = normal.norm();
  const double dist = l / n;
  normal            = normal.normalize();
  const auto di     = point - normal * dist;
  MU_ASSERT("not implemented!!");
  return std::make_tuple<>(mu::core::Vec2(), dist);
}

} // namespace mu::Magi
