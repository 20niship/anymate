#if 0
#include <cwchar>
#include <istream>
#include <movutl/tools/magi/fitting.hpp>
#include <opencv2/highgui.hpp>

/* #include <compare> */
#include <algorithm>
#include <iostream>
#include <limits>
#include <list>
#include <opencv2/imgproc.hpp>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include <random>
// #include <functional>
// #include <magi/device/device.hpp>
#include <eigen3/Eigen/Eigenvalues>


namespace mu::Magi {

using namespace mu::core;

void FittingLinear::reset() {
  y_sum = x_sum = xx_sum = xy_sum = 0;
  n                               = 0;
  err                             = -1;
}

void FittingLinear::estimate() {
  assert(n > 1);
  const double delta = xx_sum * n - x_sum * x_sum;
  result             = {(xy_sum * n - x_sum * y_sum) / delta, (xx_sum * y_sum - x_sum * xy_sum) / delta};
}

void FittingLinear::setCloud(Vec2* p, const int _n) {
  reset();
  for(int i = 0; i < _n; i++) {
    add(p[i]);
  }
}


void FittingLinear::add(const Vec2 p) {
  // auto p = (*cloud_ptr)[i];
  // if( std::abs(p[0] * default_plane.x + p[1] * default_plane.y + default_plane.z - p[2]) > threshould)continue;
  x_sum += p[0];
  y_sum += p[1];
  xy_sum += p[0] * p[1];
  xx_sum += p[0] * p[0];
  n++;
}

void FittingLinear::operator<<(const Vec2 p) {
  // auto p = (*cloud_ptr)[i];
  // if( std::abs(p[0] * default_plane.x + p[1] * default_plane.y + default_plane.z - p[2]) > threshould)continue;
  x_sum += p[0];
  y_sum += p[1];
  xy_sum += p[0] * p[1];
  xx_sum += p[0] * p[0];
  n++;
  // minMax[0][0] = MAGI_NATH_MIN(minMax[0][0], p[0]);  minMax[0][1] = MAGI_NATH_MAX(minMax[0][1], p[0]);
  // minMax[1][0] = MAGI_NATH_MIN(minMax[1][0], p[1]);  minMax[1][1] = MAGI_NATH_MAX(minMax[1][1], p[1]);
  // minMax[2][0] = MAGI_NATH_MIN(minMax[2][0], p[2]);  minMax[2][1] = MAGI_NATH_MAX(minMax[2][1], p[2]);
}

void FittingParabola::reset() {
  y_sum = x_sum = xx_sum = xxx_sum = x4_sum = xxy_sum = xy_sum = 0;
  n                                                            = 0;
  err                                                          = -1;
}

void FittingParabola::estimate() {
  assert(n > 0);
  const Mat3x3 mat = {x4_sum, xxx_sum, xx_sum, xxx_sum, xx_sum, x_sum, xx_sum, x_sum, (double)n};
  const auto inv   = mat.inv();
  Vec3 tmp2        = {xxy_sum, xy_sum, y_sum};
  result           = inv * tmp2;
}

void FittingParabola::setCloud(Vec2* p, const int _n) {
  reset();
  for(int i = 0; i < _n; i++) {
    (*this) << (p[i]);
  }
}

void FittingParabola::operator<<(const Vec2 p) {
  // auto p = (*cloud_ptr)[i];
  // if( std::abs(p[0] * default_plane.x + p[1] * default_plane.y + default_plane.z - p[2]) > threshould)continue;
  x_sum += p[0];
  y_sum += p[1];
  xy_sum += p[0] * p[1];
  xxy_sum += p[0] * p[0] * p[1];
  xx_sum += p[0] * p[0];
  xxx_sum += p[0] * p[0] * p[0];
  x4_sum += p[0] * p[0] * p[0] * p[0];
  n++;
  // minMax[0][0] = MAGI_NATH_MIN(minMax[0][0], p[0]);  minMax[0][1] = MAGI_NATH_MAX(minMax[0][1], p[0]);
  // minMax[1][0] = MAGI_NATH_MIN(minMax[1][0], p[1]);  minMax[1][1] = MAGI_NATH_MAX(minMax[1][1], p[1]);
  // minMax[2][0] = MAGI_NATH_MIN(minMax[2][0], p[2]);  minMax[2][1] = MAGI_NATH_MAX(minMax[2][1], p[2]);
}

void PlaneFitting::reset() {
  A = B = C = D = E = F = G = H = 0;
  n                             = 0;
  result_.error                 = -1;
}

void PlaneFitting::estimate() {
  assert(n > 0);
  double temp    = A * (G * G - n * B) - 2 * C * F * G + B * F * F + n * C * C;
  const Vec3 abc = {// ax + by + z = c;
                    -(C * (G * H - n * E) + F * (E * G - B * H) + D * (n * B - G * G)) / temp, (A * (G * H - n * E) + F * (-C * H - D * G) + E * F * F + n * C * D) / temp, (A * (E * G - B * H) + C * C * H - C * D * G + (B * D - C * E) * F) / temp};
  result_.axis   = Vec3(abc[0], abc[1], -1).normalize();
  if(result_.axis[2] < 0) result_.axis = -result_.axis;
  result_.error = -1;
  result_.xyz   = {F / n, G / n, H / n};

  result_.size = {
    minMax[0][1] - minMax[0][0],
    minMax[1][1] - minMax[1][0],
    minMax[2][1] - minMax[2][0],
  };
}

void PlaneFitting::setCloud(Vec3* cloud, const int pc_size) {
  reset();
  for(int i = 0; i < pc_size; i++) {
    (*this) << cloud[i];
  }
}

void PlaneFitting::operator<<(const Vec3 p) {
  // auto p = (*cloud_ptr)[i];
  // if( std::abs(p[0] * default_plane.x + p[1] * default_plane.y + default_plane.z - p[2]) > threshould)continue;
  A += p[0] * p[0];
  B += p[1] * p[1];
  C += p[0] * p[1];
  D += p[0] * p[2];
  E += p[1] * p[2];
  F += p[0];
  G += p[1];
  H += p[2];
  n++;
  minMax[0][0] = MAGI_NATH_MIN(minMax[0][0], p[0]);
  minMax[0][1] = MAGI_NATH_MAX(minMax[0][1], p[0]);
  minMax[1][0] = MAGI_NATH_MIN(minMax[1][0], p[1]);
  minMax[1][1] = MAGI_NATH_MAX(minMax[1][1], p[1]);
  minMax[2][0] = MAGI_NATH_MIN(minMax[2][0], p[2]);
  minMax[2][1] = MAGI_NATH_MAX(minMax[2][1], p[2]);
}

void SphereFitting::reset() {
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      mat[i][j] = 0;
    }
    vec[i] = 0;
  }
  cloud_whole_size = 0;
  err              = -1;
  threshould       = std::numeric_limits<float>::max();
}

void SphereFitting::reset_result() {
  estimated = false;
  err       = -1;
}

void SphereFitting::set_threshould(float th) { threshould = th; }
float SphereFitting::get_threshould() { return threshould; }

void SphereFitting::operator<<(const Vec3 p) {
  // if(err < 0 && (p - pos).norm_sq() - r*r  > threshould * threshould ) return;
  mat[0][0] += 1; // まあループの外で pointListNum を入れれば良いんだけど...
  mat[0][1] += p[0];
  mat[0][2] += p[1];
  mat[0][3] += p[2];
  mat[1][1] += (p[0] * p[0]);
  mat[1][2] += (p[0] * p[1]);
  mat[1][3] += (p[0] * p[2]);
  mat[2][2] += (p[1] * p[1]);
  mat[2][3] += (p[1] * p[2]);
  mat[3][3] += (p[2] * p[2]);
  double rho = (p[0] * p[0]) + (p[1] * p[1]) + (p[2] * p[2]);
  vec[0] += rho;
  vec[1] += (rho * p[0]);
  vec[2] += (rho * p[1]);
  vec[3] += (rho * p[2]);
  n++;
  minMax[0][0] = MAGI_NATH_MIN(minMax[0][0], p[0]);
  minMax[0][1] = MAGI_NATH_MAX(minMax[0][1], p[0]);
  minMax[1][0] = MAGI_NATH_MIN(minMax[1][0], p[1]);
  minMax[1][1] = MAGI_NATH_MAX(minMax[1][1], p[1]);
  minMax[2][0] = MAGI_NATH_MIN(minMax[2][0], p[2]);
  minMax[2][1] = MAGI_NATH_MAX(minMax[2][1], p[2]);
}

void SphereFitting::setCloud(Vec3* _cloud, int* _indices, const int _n) {
  cloud_whole_size = _n;
  cloud            = _cloud;
  indices          = _indices;
  with_indices     = true;
}

void SphereFitting::setCloud(Vec3* _cloud, const int _n) {
  cloud_whole_size = _n;
  cloud            = _cloud;
  with_indices     = false;
}

void SphereFitting::CalcAll() {
  assert(cloud_whole_size > 0);
  if(with_indices) {
    for(int i = 0; i < cloud_whole_size; i++) {
      (*this) << cloud[indices[i]];
    }
  } else {
    for(int i = 0; i < cloud_whole_size; i++) {
      (*this) << cloud[i];
    }
  }
}

void SphereFitting::estimate() {
  assert(cloud_whole_size > 0);
  if(mat[0][1] == 0 && mat[0][2] == 0 && mat[1][2] == 0) CalcAll();
  // 対称行列なので半分はここで入れる
  mat[1][0] = mat[0][1];
  mat[2][0] = mat[0][2];
  mat[2][1] = mat[1][2];
  mat[3][0] = mat[0][3];
  mat[3][1] = mat[1][3];
  mat[3][2] = mat[2][3];

  // 掃き出し法で mat の逆行列 matinv を算出
  double matinv[4][4];
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      matinv[i][j] = 0;
      if(i == j) matinv[i][j] = 1;
    }
  }

  double tmp;
  for(int i = 0; i < 4; i++) {
    tmp = 1 / mat[i][i];
    for(int j = 0; j < 4; j++) {
      mat[i][j] *= tmp;
      matinv[i][j] *= tmp;
    }
    for(int j = 0; j < 4; j++) {
      if(i == j) continue;
      tmp = mat[j][i];
      for(int k = 0; k < 4; k++) {
        mat[j][k] -= (mat[i][k] * tmp);
        matinv[j][k] -= (matinv[i][k] * tmp);
      }
    }
  }

  // 最終結果計算
  double R  = 0;
  double X0 = 0;
  double Y0 = 0;
  double Z0 = 0;
  for(int j = 0; j < 4; j++) {
    R += (matinv[0][j] * vec[j]);
    X0 += (matinv[1][j] * vec[j]);
    Y0 += (matinv[2][j] * vec[j]);
    Z0 += (matinv[3][j] * vec[j]);
  }
  r           = sqrtf(R + (X0 * X0 + Y0 * Y0 + Z0 * Z0) / 4.0);
  pos[0]      = X0 / 2.0;
  pos[1]      = Y0 / 2.0;
  pos[2]      = Z0 / 2.0;
  result_.xyz = pos;
  for(int i = 0; i < 3; i++) result_.size[i] = r;
}

void CircleFitting::reset() {
  xx_sum = xy_sum = x_sum = y_sum = yy_sum = xxx_sum = yyy_sum = xxy_sum = xyy_sum = 0;
  n                                                                                = 0;
  err                                                                              = -1;
}

void CircleFitting::estimate() {
  assert(n > 0);
  Mat3x3 H{xx_sum, xy_sum, x_sum, xy_sum, yy_sum, y_sum, x_sum, y_sum, (double)n};
  Vec3 I{-(xxx_sum + xyy_sum), -(xxy_sum + yyy_sum), -(xx_sum + yy_sum)};
  auto output    = H.inv() * I;
  const double a = -output[0] / 2;
  const double b = -output[1] / 2;
  const double c = std::sqrt(a * a + b * b - output[2]);
  result_.xyz    = {a, b, 0};
  result_.size   = {c, c, c};
  // pos = (x, y, 0)
  // size = {radius, radius, radius}
}

void CircleFitting::operator<<(const Vec2 p) {
  const double x = p[0];
  const double y = p[1];
  xx_sum += x * x;
  xy_sum += x * y;
  x_sum += x;
  y_sum += y;
  yy_sum += y * y;
  xxx_sum += x * x * x;
  yyy_sum += y * y * y;
  xxy_sum += x * x * y;
  xyy_sum += x * y * y;
  n++;
}

void CircleFitting::setCloud(Vec2* cloud, const int pc_size) {
  for(int i = 0; i < pc_size; i++) {
    (*this) << cloud[i];
  }
}

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

void CylinderFitting::setCloud(Vec3* points, int cloud_size) {
  cloud = points;
  n     = cloud_size;
}

void CylinderFitting::estimate() {
  assert(cloud != nullptr);
  assert(n > 0);

  // 中心座標を計算
  Vec3 center_pc{0, 0, 0};
  for(int i = 0; i < n; i++) {
    center_pc += cloud[i];
  }
  center_pc /= (double)n;

  Eigen::Matrix<double, 3, 3> eigenmat;
  for(int i = 0; i < n; i++) {
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

  const Vec3 norm_myvec{norm(0).real(), norm(1).real(), norm(2).real()};
  const Vec3 v1{v1_(0).real(), v1_(1).real(), v1_(2).real()};
  const Vec3 v2{v2_(0).real(), v2_(1).real(), v2_(2).real()};

  CircleFitting ft;
  ft.reset();
  double lmin = std::numeric_limits<double>::max();
  double lmax = std::numeric_limits<double>::min();
  for(int i = 0; i < n; i++) {
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
void CylinderFittingRANSAC::setCloud(Vec3* points, int cloud_size) {
  cloud = points;
  n     = cloud_size;
}
void CylinderFittingRANSAC::estimate() {
  assert(cloud != nullptr);
  assert(n > 0);

  // 中心座標を計算\ff
  Vec3 center_pc{0, 0, 0};
  for(int i = 0; i < n; i++) {
    center_pc += cloud[i];
  }
  center_pc /= (double)n;

  const int maxIteration = 100;
  for(int i = 0; i < maxIteration; i++) {
    const Vec3 pts[] = {cloud[rand() % n], cloud[rand() % n], cloud[rand() % n]};

    const auto vecA      = pts[1] - pts[0];
    const auto vecA_norm = vecA.normalize();
    const auto vecB      = pts[2] - pts[0];
    const auto vecB_norm = vecB.normalize();

    const auto vecC      = vecA_norm.cross(vecB_norm);
    const auto vecC_norm = vecC.normalize();

    const auto rodrigues_rot = [](const Vec3& p, const Vec3& from, const Vec3& dest) {
      const auto n0 = from.normalize();
      const auto n1 = dest.normalize();
      const auto k  = n0.cross(n1);
      Mat3x3 p_rot;
      p_rot.zeros();
      if(k.norm() == 0) return p;
      const auto k2      = k.normalize();
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

void CylinderFittingNormal::setCloud(Vec3* points, int cloud_size) {
  assert("Please use NORMAL!");
  cloud = points;
  n     = cloud_size;
}

void CylinderFittingNormal::setCloud(Vec3* points, Vec3* norm, const int cloud_size) {
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
  double lmax = std::numeric_limits<double>::min();

  Vec3 sum_xyz{0, 0, 0};
  for(int i = 0; i < n; i++) {
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

void CylinderFitting2::setCloud(Vec3* points, int cloud_size) {
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
  double lmax = std::numeric_limits<double>::min();

  Vec3 sum_xyz{0, 0, 0};
  for(int i = 0; i < n; i++) {
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


#endif
