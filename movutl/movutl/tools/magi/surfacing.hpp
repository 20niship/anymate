#include <cwchar>
#include <eigen3/Eigen/Eigenvalues>
#include <movutl/tools/magi/convex_hull.hpp>
#include <movutl/tools/magi/fitting.hpp>
#include <opencv2/core/persistence.hpp>
#include <tuple>

namespace mu::Magi{


class Surface {

public:
  enum class NAxis { AxisX = 1, AxisY, AxisZ };
  core::Vec3* cloud;
  size_t n;
  std::vector<core::Vec3> m_mesh;

  core::Vec3 m_normal, m_plane_v1, m_plane_v2, m_center;
  double plane_d;
  double m_plane_fitting_error;
  std::vector<size_t> m_convex_hull_idx;
  NAxis m_naxis;

  Surface(core::Vec3* _cloud, size_t n_) {
    cloud = _cloud;
    n     = n_;
  }

  void svd() {
    core::Vec3 center_pc{0, 0, 0};
    for(size_t i = 0; i < n; i++) {
      center_pc += cloud[i];
    }
    center_pc /= (double)n;
    m_center = center_pc;

#if 0
    Eigen::Matrix<double, 3, 3> eigenmat;
    for(size_t i = 0; i < n; i++) {
      for(size_t x = 0; x < 3; x++) {
        for(size_t y = 0; y < 3; y++) {
          eigenmat(x, y) += (cloud[i][x] - center_pc[x]) * (cloud[i][y] - center_pc[y]);
        }
      }
    }
    eigenmat /= (double)n;
    Eigen::EigenSolver<Eigen::Matrix<double, 3, 3> > s(eigenmat);
    const auto eigenvalues = s.eigenvalues();
    const auto eigenvecs   = s.eigenvectors();
    const int max_idx      = (eigenvalues(0, 0).real() > eigenvalues(1, 0).real() && eigenvalues(0, 0).real() > eigenvalues(2, 0).real()) ? 0 : ((eigenvalues(1, 0).real() > eigenvalues(0, 0).real() && eigenvalues(1, 0).real() > eigenvalues(2, 0).real()) ? 1 : 2);
    const auto norm        = eigenvecs.col((max_idx + 2) % 3);
    const auto v1_         = eigenvecs.col((max_idx) % 3);
    const auto v2_         = eigenvecs.col((max_idx + 1) % 3);

    m_normal   = Vec3(norm(0).real(), norm(1).real(), norm(2).real());
    m_plane_v1 = Vec3(v1_(0).real(), v1_(1).real(), v1_(2).real());
    m_plane_v2 = Vec3(v2_(0).real(), v2_(1).real(), v2_(2).real());
    plane_d    = -center_pc.dot(m_normal);
#endif

    mu::Magi::PlaneFitting fit;
    fit.setCloud(cloud, n);
    fit.estimate();
    const auto res = fit.result();
    m_normal   = res.axis;
    m_plane_v1 = core::Vec3(1, 0, 0).cross(res.axis);
    m_plane_v1 = m_plane_v1.normalize();
    m_plane_v2 = m_plane_v1.cross(res.axis);
    m_plane_v2 = m_plane_v2.normalize();
    plane_d    = -m_center.dot(m_normal);
  }

  auto _project(const core::Vec3& point) {
    core::Vec4 plane{m_normal[0], m_normal[1], m_normal[2], plane_d};
    const double l    = std::abs(point[0] * plane[0] + point[1] * plane[1] + point[2] + plane[2] + plane[3]);
    auto normal       = mu::core::Vec3(plane[0], plane[1], plane[2]);
    const double n    = normal.norm();
    const double dist = l / n;
    normal            = m_normal.normalize();
    MU_ASSERT("not implemented!!");
    return std::make_tuple<>(mu::core::Vec2(), dist);
  }

#if 0
  NAxis nearest_axis() {
    if(m_normal[2] > m_normal[0] && m_normal[2] > m_normal[1]) {
      return NAxis::AxisZ;
    } else if(m_normal[1] > m_normal[0] > m_normal[1] > m_normal[2]) {
      return NAxis::AxisY;
    } else {
      return NAxis::AxisX;
    }
  }
  Vec3 deprojection(const Vec2& p, double dist) {
    switch(m_naxis) {
      case NAxis::AxisX: return Vec3(dist, p[0], p[1]);
      case NAxis::AxisY: return Vec3(p[0], dist, p[1]);
      case NAxis::AxisZ: return Vec3(p[0], p[1], dist);
    }
  }
  std::tuple<Vec2, double> projection(const Vec3& p) {
    switch(m_naxis) {
      case NAxis::AxisX: return std::make_tuple(Vec2(p[1], p[2]), p[0]);
      case NAxis::AxisY: return std::make_tuple(Vec2(p[0], p[2]), p[1]);
      case NAxis::AxisZ: return std::make_tuple(Vec2(p[0], p[1]), p[2]);
    }
  }
#endif

  core::Vec3 deprojection(const core::Vec2& p, double dist) { return m_plane_v1 * p[0] + m_plane_v2 * p[1] + m_normal * dist; }

  std::tuple<core::Vec2, double> projection(const core::Vec3& p) {
    const double a1 = m_plane_v1.dot(p);
    const double a2 = m_plane_v2.dot(p);
    const double d  = m_normal.dot(p);
    return std::make_tuple(core::Vec2(a1, a2), d);
  }

  void calc_error() {
    double e = 0;
    for(size_t i = 0; i < n; i++) {
      double dist = m_normal.dot(cloud[i]) + plane_d;
      e += dist;
    }
    m_plane_fitting_error = e / n;
  }

  void boxing() {
    m_mesh.clear();
    std::vector<core::Vec2> projected;
    std::vector<double> depth;
    for(size_t i = 0; i < n; i++) {
      const auto [p, d] = projection(cloud[i]);
      projected.push_back(p);
      depth.push_back(d);
    }
    m_convex_hull_idx = mu::Magi::convex_hull(projected);
    for(size_t i = 0; i < m_convex_hull_idx.size(); i++) {
      const auto idx = m_convex_hull_idx[i];
      m_mesh.push_back(deprojection(projected[idx], depth[idx]));
    }
  }

  void run() {
    assert(n > 0);
    svd();
    calc_error();
    boxing();
  }
};
}
