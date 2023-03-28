#pragma once
#include <movutl/core/vector.hpp>
namespace mu::core {

struct Quat {
  float x, y, z, w;
  Quat() {x = y = z = w = 0;}
  Quat(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
  Quat conjugate(const Quat& q) const { return Quat(-q.x, -q.y, -q.z, q.w); }
  Quat operator*(const Quat& q) const { return Quat(w * q.x - z * q.y + y * q.z + x * q.w, z * q.x + w * q.y - x * q.z + y * q.w, -y * q.x + x * q.y + w * q.z + z * q.w, -x * q.x - y * q.y - z * q.z + w * q.w); }
  Vec3 rotate(const Vec3 v) const {
    auto vq = Quat(v[0], v[1], v[2], 0);
    auto cq = conjugate(*this);
    auto mq = *this * vq * cq;
    return Vec3(mq.x, mq.y, mq.z);
  }
};

} // namespace mu::core
