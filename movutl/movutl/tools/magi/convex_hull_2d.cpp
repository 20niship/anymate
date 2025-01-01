#include <movutl/core/assert.hpp>
#include <movutl/core/vector.hpp>
#include <movutl/tools/magi/convex_hull.hpp>
#include <opencv2/core/persistence.hpp>
#include <vector>

using namespace mu::core;

namespace mu::Magi {

#define rep(x, y) for(size_t x = 0; x < y; x++)
using namespace std;
// x座標、y座標、偏角メンバを持つ構造体
struct Point {
  double x, y, arg;
  int nouse = 0;
  int idx;

  Point(){
    x = 0;
    y = 0;
    calc_arg();
  }

  Point(double x_, double y_, size_t _idx) {
    x = x_;
    y = y_;
    idx = _idx;
    calc_arg();
  }
  // x, yの値をもとに偏角を計算する
  void calc_arg() {
    if(x == 0) {
      // 0除算を防ぐため、x=0の時argを10に設定する。
      arg = 10;
      // 原点の場合11に設定する
      if(y == 0) arg++;
      return;
    }
    // 通常argは-π/2からπ/2までの範囲
    arg = atan(y / x);
  }
  // 原点からの距離の二乗を求める関数
  double dist() { return x * x + y * y; }
  static bool comp_x(Point& a, Point& b) {
    // x座標が等しい場合、y座標を基準にする
    if(a.x == b.x) return a.y < b.y;
    return a.x < b.x;
  }
  static bool comp_arg(Point& a, Point& b) { return a.arg < b.arg; }
  static bool comp_nouse(Point& a, Point& b) { return a.nouse < b.nouse; }
};
// 符号付き面積を求める関数
double area2(const Point a, const Point b, const Point c) { return a.x * b.y + a.y * c.x + b.x * c.y - b.y * c.x - c.y * a.x - a.y * b.x; }
// 一直線上に点が並んでしまうような集合の場合を考慮して、原点から最も遠いものを残して削除する関数
void delete_samearg(vector<Point>& p, size_t n) {
  int recent = 0;
  int nouse  = 0;
  rep(i, n) {
    if(i == 0) continue;
    if(p[recent].arg == p[i].arg) {
      nouse++;
      if(p[recent].dist() < p[i].dist()) {
        p[recent].nouse = 1;
        recent          = i;
      } else {
        p[i].nouse = 1;
      }
    } else
      recent = i;
  }
  n -= nouse;
  sort(p.begin(), p.end(), Point::comp_nouse);
  p.resize(n);
  sort(p.begin(), p.end(), Point::comp_arg);
}

template <typename T> std::vector<Point> cvt(const std::vector<_Vec<T, 2>>& points) {
  std::vector<Point> p;
  int i=0;
  for(const auto& pt : points) {
    p.push_back(Point(pt[0], pt[1], i));
    i++;
  }
  return p;
}

template <typename T> auto cvt(const std::vector<Point>& points) {
  std::vector<size_t> p;
  for(const auto& pt : points) {
    p.push_back(pt.idx);
  }
  return p;
}

using namespace mu::core;
template <typename T> std::vector<size_t> convex_hull(const std::vector<_Vec<T, 2>>& points) {
  auto p = cvt(points);
  // xとyが一番小さい点をp[0]にする。
  iter_swap(min_element(p.begin(), p.end(), Point::comp_x), p.begin());
  Point p0 = p[0];
  // p[0]基準に原点を移動する
  rep(i, p.size()) {
    p[i].x -= p0.x;
    p[i].y -= p0.y;
    p[i].calc_arg();
  }
  // 偏角順にソート
  sort(p.begin(), p.end(), Point::comp_arg);
  delete_samearg(p, p.size());
  std::vector<Point> sta;
  sta.push_back(p[p.size() - 1]);
  sta.push_back(p[0]);

  // グラハムのアルゴリズム開始
  rep(i, p.size()) {
    if(i == 0) continue;
    while(1) {
      Point t = sta.back();
      sta.pop_back();
      Point s = sta.back();
      if(area2(s, t, p[i]) > 0) {
        sta.push_back(t);
        sta.push_back(p[i]);
        break;
      }
    }
  }
  sta.pop_back();
  return cvt<T>(sta);
}

template std::vector<size_t> convex_hull(const std::vector<Vec2>&);
template std::vector<size_t> convex_hull(const std::vector<Vec2d>&);
template std::vector<size_t> convex_hull(const std::vector<Vec2f>&);

} // namespace mu::Magi
