#include <istream>
#include <movutl/tools/magi/fitting.hpp>
#include <opencv2/highgui.hpp>

#include <algorithm>
#include <iostream>
#include <limits>
#include <list>
#include <opencv2/imgproc.hpp>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include <random>
#include <eigen3/Eigen/Eigenvalues>


namespace mu::Magi {
using namespace mu::core;

void CylinderFitting::reset() {
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      mat[i][j] = 0;
    }
    vec[i] = 0;
  }
  n   = 0;
  err = -1;
}

void CylinderFitting::setCloud(Vec3* points, size_t cloud_size) {
  cloud = points;
  n     = cloud_size;
}

void CylinderFitting::estimate() {
  assert(cloud != nullptr);
  assert(n > 0);

  // 中心座標を計算
  Vec3 center_pc{0, 0, 0};
  for(size_t i = 0; i < n; i++) {
    center_pc += cloud[i];
  }
  center_pc /= (double)n;

  Eigen::Matrix<double, 3, 3> eigenmat;
  for(size_t i = 0; i < n; i++) {
    for(int x = 0; x < 3; x++) {
      for(int y = 0; y < 3; y++) {
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

    std::cout << "固有値 " << std::endl << s.eigenvalues() << std::endl;
    std::cout << "固有ベクトル" << std::endl << s.eigenvectors() << std::endl;
 

  const Vec3 norm_myvec{norm(0).real(), norm(1).real(), norm(2).real()};
  const Vec3 v1{v1_(0).real(), v1_(1).real(), v1_(2).real()};
  const Vec3 v2{v2_(0).real(), v2_(1).real(), v2_(2).real()};

  CircleFitting ft;
  ft.reset();
  double lmin = std::numeric_limits<double>::max();
  double lmax = std::numeric_limits<double>::lowest();
  for(size_t i = 0; i < n; i++) {
    const double L       = norm_myvec.dot(cloud[i] - center_pc);
    lmin                 = std::min<double>(L, lmin);
    lmax                 = std::min<double>(L, lmax);
    const Vec2 projected = {
      v1.dot(cloud[i] - center_pc),
      v2.dot(cloud[i] - center_pc),
    };
    ft << projected;
  }
  ft.estimate();
  const auto circle_res = ft.result();
  result_.xyz           = v1 * circle_res.xyz[0] + v2 * circle_res.xyz[1] + norm_myvec * (lmin + lmax) / 2;
  result_.axis          = norm_myvec;
  result_.size          = {circle_res.size[0], lmax - lmin, 0}; // radius, length, 0;
}


void CylinderFittingRANSAC::reset() {
  cloud = nullptr;
  n     = 0;
  err   = -1;
}
void CylinderFittingRANSAC::setCloud(Vec3* points, size_t cloud_size) {
  cloud = points;
  n     = cloud_size;
}
void CylinderFittingRANSAC::estimate() {
  assert(cloud != nullptr);
  assert(n > 0);

  // 中心座標を計算\ff
  Vec3 center_pc{0, 0, 0};
  for(size_t i = 0; i < n; i++)
    center_pc += cloud[i];
  center_pc /= (double)n;

  const int maxIteration = 100;
  for(int i = 0; i < maxIteration; i++) {
    const Vec3 pts[] = {cloud[rand() % n], cloud[rand() % n], cloud[rand() % n]};

    const auto vecA      = pts[1] - pts[0];
    const auto vecA_norm = vecA.normalize();
    const auto vecB      = pts[2] - pts[0];
    const auto vecB_norm = vecB.normalize();

    const auto vecC      = vecA_norm.cross(vecB_norm);
    //const auto vecC_norm = vecC.normalize();

    const auto rodrigues_rot = [](const Vec3& p, const Vec3& from, const Vec3& dest) {
      const auto n0 = from.normalize();
      const auto n1 = dest.normalize();
      const auto k  = n0.cross(n1);
      Mat3x3 p_rot;
      p_rot.zeros();
      if(k.norm() == 0) return p;
      //const auto k2      = k.normalize();
      const double theta = std::acos(n0.dot(n1));
      const Vec3 ret     = p * std::cos(theta) + k.cross(p) * p * std::sin(theta) + k * k.dot(p) * (1 - std::cos(theta));
      return ret;
    };
    Vec3 P_rot[3] = {
      rodrigues_rot(pts[0], vecC, {0, 0, 1}),
      rodrigues_rot(pts[1], vecC, {0, 0, 1}),
      rodrigues_rot(pts[2], vecC, {0, 0, 1}),
    };
    double ma = 0, mb = 0;
    while(ma == 0) {
      ma = (P_rot[1][1] - P_rot[1][1]) / (P_rot[1][0] - P_rot[0][0]);
      mb = (P_rot[2][1] - P_rot[1][1]) / (P_rot[2][0] - P_rot[0][0]);
      if(ma == 0) {
        const auto tmp = P_rot[0];
        P_rot[0]       = P_rot[1];
        P_rot[1]       = P_rot[2];
        P_rot[2]       = tmp;
      } else {
        break;
      }
    }
    const auto center_x   = (ma * mb * (P_rot[0][1] - P_rot[2][1]) + mb * (P_rot[0][0] + P_rot[1][0]) - ma * (P_rot[1][0] + P_rot[2][0])) / (2 * (mb - ma));
    const double center_y = -1 / ma * (center_x - (P_rot[0][0] + P_rot[1][0]) / 2) + (P_rot[0][1] + P_rot[1][1]) / 2;
    const Vec3 center_rot{center_x, center_y, 0};
    const double r    = (center_rot - P_rot[0]).norm();
    const auto center = rodrigues_rot(center_rot, {0, 0, 1}, vecC);

    result_.xyz  = center;
    result_.size = {r, 0, 0};
    result_.axis = vecC;
  }
}

void CylinderFittingNormal::operator<<(const Vec3 p) { std::cout << "not implemented" << p << std::endl; }
void CylinderFittingNormal::reset() {
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      mat[i][j] = 0;
    }
    vec[i] = 0;
  }
  n   = 0;
  err = -1;
}

void CylinderFittingNormal::setCloud(Vec3* points, size_t cloud_size) {
  assert("Please use NORMAL!");
  cloud = points;
  n     = cloud_size;
}

void CylinderFittingNormal::setCloud(Vec3* points, Vec3* norm, const size_t cloud_size) {
  cloud  = points;
  normal = norm;
  n      = cloud_size;
}

void CylinderFittingNormal::estimate() {
  assert(cloud != nullptr);
  assert(normal != nullptr);
  assert(n > 0);

  // 中心座標を計算
  PlaneFitting ft_p;
  ft_p.reset();
  ft_p.setCloud(normal, n);
  ft_p.estimate();
  const auto res_ft_p = ft_p.result();
  const auto norm     = res_ft_p.axis;
  Vec3 e1, e2;
  if(norm[0] >= norm[1] && norm[0] >= norm[2]) { // X軸に近い直線
    e1 = {norm[1], norm[0], norm[2]};
    e2 = {norm[2], norm[1], norm[0]};
  } else if(norm[1] >= norm[0] && norm[1] >= norm[2]) {
    e1 = {norm[1], norm[0], norm[2]};
    e2 = {norm[0], norm[2], norm[1]};
  } else {
    e1 = {norm[2], norm[1], norm[0]};
    e2 = {norm[0], norm[2], norm[1]};
  }
  e1 = e1.normalize();
  e2 = e2.normalize();

  /* const Vec3 v1{ v1_(0).real(), v1_(1).real(), v1_(2).real() }; */
  /* const Vec3 v2{ v2_(0).real(), v2_(1).real(), v2_(2).real() }; */
  CircleFitting ft_c;
  ft_c.reset();
  double lmin = std::numeric_limits<double>::max();
  double lmax = std::numeric_limits<double>::lowest();

  Vec3 sum_xyz{0, 0, 0};
  for(size_t i = 0; i < n; i++) {
    const double L       = norm.dot(cloud[i]);
    lmin                 = std::min<double>(L, lmin);
    lmax                 = std::min<double>(L, lmax);
    const Vec2 projected = {
      e1.dot(cloud[i]),
      e2.dot(cloud[i]),
    };
    ft_c << projected;
    sum_xyz += cloud[i];
  }
  ft_c.estimate();
  const auto circle_res = ft_c.result();
  const double r        = std::min<double>(circle_res.size[0], 250);
  const double l        = lmax - lmin;

  /* result_.xyz = v1 * circle_res.xyz[0] + v2*circle_res.xyz[1] + norm_myvec*(lmin + lmax)/2; */
  result_.xyz  = sum_xyz / n;
  result_.axis = norm;
  result_.size = {r, l, 0}; // radius, length, 0;
}

void CylinderFitting2::operator<<(const Vec3 p) { std::cout << "not implemented" << p << std::endl; }
void CylinderFitting2::reset() { n = 0; }

void CylinderFitting2::setCloud(Vec3* points, size_t cloud_size) {
  assert("Please use NORMAL!");
  cloud = points;
  n     = cloud_size;
}

void CylinderFitting2::estimate() {
  assert(cloud != nullptr);
  assert(n > 0);
  // 中心座標を計算
  PlaneFitting ft_p;
  ft_p.reset();
  ft_p.estimate();
  const auto res_ft_p = ft_p.result();
  const auto norm     = res_ft_p.axis;
  Vec3 e1, e2;
  if(norm[0] >= norm[1] && norm[0] >= norm[2]) { // X軸に近い直線
    e1 = {norm[1], norm[0], norm[2]};
    e2 = {norm[2], norm[1], norm[0]};
  } else if(norm[1] >= norm[0] && norm[1] >= norm[2]) {
    e1 = {norm[1], norm[0], norm[2]};
    e2 = {norm[0], norm[2], norm[1]};
  } else {
    e1 = {norm[2], norm[1], norm[0]};
    e2 = {norm[0], norm[2], norm[1]};
  }
  e1 = e1.normalize();
  e2 = e2.normalize();

  /* const Vec3 v1{ v1_(0).real(), v1_(1).real(), v1_(2).real() }; */
  /* const Vec3 v2{ v2_(0).real(), v2_(1).real(), v2_(2).real() }; */
  CircleFitting ft_c;
  ft_c.reset();
  double lmin = std::numeric_limits<double>::max();
  double lmax = std::numeric_limits<double>::lowest();

  Vec3 sum_xyz{0, 0, 0};
  for(size_t i = 0; i < n; i++) {
    const double L       = norm.dot(cloud[i]);
    lmin                 = std::min<double>(L, lmin);
    lmax                 = std::min<double>(L, lmax);
    const Vec2 projected = {
      e1.dot(cloud[i]),
      e2.dot(cloud[i]),
    };
    ft_c << projected;
    sum_xyz += cloud[i];
  }
  ft_c.estimate();
  const auto circle_res = ft_c.result();
  const double r        = std::min<double>(circle_res.size[0], 250);
  const double l        = lmax - lmin;

  /* result_.xyz = v1 * circle_res.xyz[0] + v2*circle_res.xyz[1] + norm_myvec*(lmin + lmax)/2; */
  result_.xyz  = sum_xyz / n;
  result_.axis = norm;
  result_.size = {r, l, 0}; // radius, length, 0;
}

} // namespace mu::Magi

