#include <spdlog/spdlog.h>
#include <movutl/core/logger.hpp>
#include <movutl/tools/mesh/normal_estimation.hpp>

namespace mu::tools {

using namespace mu::core;
using namespace mu::db;

NormalEstimation::NormalEstimation() {}

void NormalEstimation::setRadius(float r) { radius = r; }
void NormalEstimation::setMesh(const Mesh* m_) {
  MU_ASSERT(m_);
  MU_ASSERT(m_->vertex.size() > 0);
  points.resize(m_->vertex.size());
  for(size_t i = 0; i < m_->vertex.size(); i++) {
    points[i][0] = m_->vertex[i].pos[0];
    points[i][1] = m_->vertex[i].pos[1];
    points[i][2] = m_->vertex[i].pos[2];
  }
}

void NormalEstimation::setMesh(const Mesh2D* m_) {
  MU_ASSERT(m_);
  MU_ASSERT(m_->vertex.size() > 0);
  LOGW << "Normal Estimation Warning : Mesh2D type was set. this would be ignore vertex z position!";
  points.resize(m_->vertex.size());
  for(size_t i = 0; i < m_->vertex.size(); i++) {
    points[i][0] = m_->vertex[i].pos[0];
    points[i][1] = m_->vertex[i].pos[1];
    points[i][2] = 0.0;
  }
}

void NormalEstimation::setMesh(const MeshCol* m_) {
  MU_ASSERT(m_);
  MU_ASSERT(m_->vertex.size() > 0);
  points.resize(m_->vertex.size());
  for(size_t i = 0; i < m_->vertex.size(); i++) {
    points[i][0] = m_->vertex[i].pos[0];
    points[i][1] = m_->vertex[i].pos[1];
    points[i][2] = m_->vertex[i].pos[2];
  }
}

void NormalEstimation::compute() {
  std::cout << "inserting......" << std::endl;
  octree.set_dataset(&points);
  octree.report();
  normal.resize(points.size());
  size_t i    = 0;
  float r_tmp = radius;

  while(i < points.size()) {
    const auto t   = points[i];
    const auto res = octree.findNearest(t, core::SearchMethod::Sphere, r_tmp);

    if(res.size() > 2) {
      const auto p1 = points[res[1]];
      const auto p2 = points[res[2]];
      const auto d1 = p1 - t;
      const auto d2 = p2 - t;
      normal[i]     = d1.cross(d2);
      i++;
      r_tmp = radius;
      DISP(res.size());
    } else {
      LOGW << "Normal Estimation Neighbor not found!";
      r_tmp *= 2;
      /* normal[i][0] = normal[i][1] = normal[i][2] = 0; */
      /* i++; */
    }
  }
}

void NormalEstimation::flip(const core::Vec3& center) {
  if(normal.size() == 0) {
    LOGW << "Normalestimation::flip Warning : Normal size = 0;";
    return;
  }

  for(size_t i = 0; i < normal.size(); i++)
    if(normal[i].dot(center) < 0) normal[i] = -normal[i];
}
} // namespace mu::tools
