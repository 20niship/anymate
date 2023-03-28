#include <algorithm>
#include <iostream>
#include <math.h>
#include <movutl/core/vector.hpp>
#include <vector>

namespace mu::core {
class Spline {
private:
  std::vector<double> X, Y;
  std::vector<core::Vec2> result;
  unsigned int div;
  bool cyclic; // ループするか
  bool cubic;

public:
  Spline() {
    cyclic = false;
    cubic  = true;
    div    = 8;
  }

  void init(std::vector<double> x, std::vector<double> y, int _div = 8, bool _cubic = true, bool _cyclic = false) {
    X      = x;
    Y      = y;
    div    = _div;
    cubic  = _cubic;
    cyclic = _cyclic;
  }

  bool generate() {
    int beginT, endT, count;
    result.clear();
    if(X.size() != Y.size()) {
      std::cerr << "input X.size() != y.size()" << std::endl;
      return false;
    }

    if(cyclic) {
      beginT = 0;
      endT   = X.size();
    } else {
      beginT = -1;
      endT   = X.size();
    }
    count = (endT - beginT) * div;
    for(int i = 0; i <= count; i++) {
      double t         = (double)beginT + (double)(endT - beginT) * (double)i / (double)count;
      const auto point = evaluateSpline(t);
      result.push_back(point);
    }
    return true;
  }

  std::vector<double> getInutX() const { return X; }
  std::vector<double> getInutY() const { return Y; }
  std::vector<Vec2> getOutput() const { return result; }

  Vec2 getPosFromRange(double r) const {
    if(r > 1 || r < 0) {
      return Vec2(0, 0);
    }
    int index        = result.size() * r;
    double rr        = r * result.size() - index;
    int before_index = std::min<int>(index, result.size() - 1);
    int after_index  = std::min<int>(index + 1, result.size() - 1);
    auto before      = result[before_index];
    auto after       = result[after_index];
    return before * (1.0f - rr) + after * rr;
  }

private:
  double calcWeight_cubic(int basisT, double t) {
    double nt;
    if(t < (basisT - 1.5)) {
      return 0;
    } else if(t < (basisT - 0.5)) {
      nt = t - ((double)basisT - 1.5);
      return 0.5 * nt * nt;
    } else if(t < (basisT + 0.5)) {
      nt = t - (double)basisT;
      return 0.75 - (nt * nt);
    } else if(t < (basisT + 1.5)) {
      nt = t - ((double)basisT + 1.5);
      return 0.5 * nt * nt;
    } else {
      return 0;
    }
  }

  double calcWeight_linear(int basisT, double t) {
    if(t < (basisT - 1)) {
      return 0;
    } else if(t < basisT) {
      return t - (basisT - 1);
    } else if(t < (basisT + 1)) {
      return 1 - t + basisT;
    } else {
      return 0;
    }
  }


  Vec2 evaluateSpline(double t) {
    double x = 0;
    double y = 0;
    // tと次数でどの範囲の点を足すかを判断する。
    int degree     = cubic ? 2 : 1;
    int basisWidth = degree + 1;
    int pointBegin = ceil(t - ((double)basisWidth * 0.5));

    //int pointEnd = pointBegin + basisWidth;
    for(int i = 0; i < basisWidth; ++i) {
      int index  = pointBegin + i;
      int basisT = index;
      double basisWeight;
      if(cubic) {
        basisWeight = calcWeight_cubic(basisT, t);
      } else {
        basisWeight = calcWeight_linear(basisT, t);
      }
      if(index < 0) {
        if(cyclic) {
          index += X.size();
        } else {
          index = 0;
        }
      } else if((size_t)index >= X.size()) {
        if(cyclic) {
          index = index % X.size();
        } else {
          index = X.size() - 1;
        }
      }
      x += basisWeight * X[index];
      y += basisWeight * Y[index];
    }
    return Vec2(x, y);
  };
};
} // namespace mu::core

