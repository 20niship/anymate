#include <istream>
#include <movutl/tools/magi/fitting.hpp>
#include <algorithm>
#include <iostream>
#include <limits>
#include <list>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include <random>

namespace mu::Magi {
using namespace mu::core;

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

void SphereFitting::setCloud(Vec3* _cloud, int* _indices, const size_t _n) {
  cloud_whole_size = _n;
  cloud            = _cloud;
  indices          = _indices;
  with_indices     = true;
}

void SphereFitting::setCloud(Vec3* _cloud, const size_t _n) {
  cloud_whole_size = _n;
  cloud            = _cloud;
  with_indices     = false;
}

void SphereFitting::CalcAll() {
  assert(cloud_whole_size > 0);
  if(with_indices) {
    for(size_t i = 0; i < cloud_whole_size; i++)
      (*this) << cloud[indices[i]];
  } else {
    for(size_t i = 0; i < cloud_whole_size; i++)
      (*this) << cloud[i];
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

} // namespace mu::Magi

