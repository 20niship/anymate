#include <array>
#include <movutl/core/vector.hpp>
#include <vector>

namespace mu::core {
//! @brief データの平均を求める
//! @param [in] vec データの配列
//! @return 各要素の平均
inline Vec3 Covariance_Ave(const std::vector<Vec3>& vec) {
  // 初期化
  Vec3 ave;
  ave[0] = 0;
  ave[1] = 0;
  ave[2] = 0;

  // 各要素平均
  for(size_t i = 0; i < vec.size(); i++) {
    ave[0] += vec[i][0];
    ave[1] += vec[i][1];
    ave[2] += vec[i][2];
  }
  ave[0] /= vec.size();
  ave[1] /= vec.size();
  ave[2] /= vec.size();
  return ave;
}

//! @brief 共分散を求める
//! @param [in] average 各要素の平均
//! @param [in] vec データ
//! @param [in] param どの要素に対して求めるか。例えばxyzの時、x,yに対する共分散なら{0,1}を与える。
//! @return 偏差の積の和の要素数分の一
inline double Covariance(const Vec3& average, const std::vector<Vec3>& vec, const std::array<int, 2>& param) {
  double sum = 0.0;
  for(size_t i = 0; i < vec.size(); i++) {
    // 指定したパラメータの偏差を求める
    Vec3 deviation;
    for(size_t j = 0; j < param.size(); j++) {
      int target          = param[j];
      deviation[target] = (vec[i][target] - average[target]);
    }

    // 偏差の積
    double product = 1.0;
    for(size_t j = 0; j < param.size(); j++) {
      int target = param[j];
      product *= deviation[target];
    }
    // 偏差の積の和を更新
    sum += product;
  }

  // 偏差の積の和のN分の一
  return 1.0 / vec.size() * sum;
}

inline mu::core::Mat3x3 covariance(const mu::core::Vec3 points[], size_t n) {
  std::vector<Vec3> pt;
  for(size_t i = 0; i < n; i++) pt.push_back(points[i]);
  auto average = Covariance_Ave(pt);

  // 共分散を求める
  // 第三引数の{0,0}はxxを表す。xyなら{0,1}。これはデータがxyzの順に並んでいる事が前提。
  double Sxx = Covariance(average, pt, {0, 0});
  double Sxy = Covariance(average, pt, {0, 1});
  double Sxz = Covariance(average, pt, {0, 2});
  double Syy = Covariance(average, pt, {1, 1});
  double Syz = Covariance(average, pt, {1, 2});
  double Szz = Covariance(average, pt, {2, 2});

  // 分散共分散行列を表示
  printf("分散共分散行列\n");
  printf("%.5lf %.5lf %.5lf\n", Sxx, Sxy, Sxz);
  printf("%.5lf %.5lf %.5lf\n", Sxy, Syy, Syz);
  printf("%.5lf %.5lf %.5lf\n", Sxz, Syz, Szz);

  Mat3x3 res;
  res(0, 1) = res(1, 0) = Sxy;
  res(0, 2) = res(2, 0) = Sxz;
  res(1, 2) = res(2, 1) = Syz;
  res(0, 0)             = Sxx;
  res(1, 1)             = Syy;
  res(2, 2)             = Syy;

  return res;
}

} // namespace mu::core
