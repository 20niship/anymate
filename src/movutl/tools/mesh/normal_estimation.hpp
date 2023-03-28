#pragma once

#include <movutl/core/octree.hpp>
#include <movutl/db/mesh.hpp>

namespace mu::tools {

class NormalEstimation {
private:
  float radius;
  mu::core::Octree2<float, 10> octree;
  core::Vec<core::Vec3f> points;
public:
  core::Vec<core::Vec3f> normal;
  NormalEstimation();
  void setMesh(const db::Mesh* m_);
  void setMesh(const db::Mesh2D* m_);
  void setMesh(const db::MeshCol* m_);
  void setRadius(float);
  void compute();
  void flip(const core::Vec3 &center={0, 0, 0});
};

template<typename T>
void normal_estimate(db::MeshCol* m, core::Vec<core::_Vec<T, 3>> &normals, const float R = 0.1);

} // namespace mu::tools
