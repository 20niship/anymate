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

void FittingLinear::setCloud(Vec2* p, const size_t _n) {
  reset();
  for(size_t i = 0; i < _n; i++)
    add(p[i]);
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

void FittingParabola::setCloud(Vec2* p, const size_t _n) {
  reset();
  for(size_t i = 0; i < _n; i++)
    (*this) << (p[i]);
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

} // namespace mu::Magi

