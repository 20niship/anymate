#include <cwchar>
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

namespace mu::Magi {
using namespace mu::core;

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

void CircleFitting::setCloud(Vec2* cloud, const size_t pc_size) {
  for(size_t i = 0; i < pc_size; i++)
    (*this) << cloud[i];
}


} // namespace mu::Magi

