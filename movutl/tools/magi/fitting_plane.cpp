#include <istream>
#include <movutl/tools/magi/fitting.hpp>
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

void PlaneFitting::setCloud(Vec3* cloud, const size_t pc_size) {
  reset();
  for(size_t i= 0; i < pc_size; i++)
    (*this) << cloud[i];
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

} // namespace mu::Magi

