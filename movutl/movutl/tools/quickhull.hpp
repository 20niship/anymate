#pragma once
#include <movutl/core/core.hpp>
#include <vector>

namespace mu::Magi {

class QuickHull {
private:
  core::Vec<core::Vec2> hull;
  core::Vec2* input;
  int n;
  int findSide(core::Vec2 p1, core::Vec2 p2, core::Vec2 p) {
    int val = (p[1] - p1[1]) * (p2[0] - p1[0]) - (p2[1] - p1[1]) * (p[0] - p1[0]);
    if(val > 0) return 1;
    if(val < 0) return -1;
    return 0;
  }

  // returns input value proportional to the distance
  // between the point p and the line joining the
  // points p1 and p2
  inline int lineDist(core::Vec2 p1, core::Vec2 p2, core::Vec2 p) { return std::abs((p[1] - p1[1]) * (p2[0] - p1[0]) - (p2[1] - p1[1]) * (p[0] - p1[0])); }

  // End points of line L are p1 and p2.  side can have value
  // 1 or -1 specifying each of the parts made by the line L
  void quickHull(core::Vec2 input[], int n, core::Vec2 p1, core::Vec2 p2, int side) {
    int ind      = -1;
    int max_dist = 0;

    // finding the point with maximum distance
    // from L and also on the specified side of L.
    for(int i = 0; i < n; i++) {
      int temp = lineDist(p1, p2, input[i]);
      if(findSide(p1, p2, input[i]) == side && temp > max_dist) {
        ind      = i;
        max_dist = temp;
      }
    }

    // If no point is found, add the end points
    // of L to the convex hull.
    if(ind == -1) {
      hull.push_back(p1);
      hull.push_back(p2);
      return;
    }
    // Recur for the two parts divided by input[ind]
    quickHull(input, n, input[ind], p1, -findSide(input[ind], p1, p2));
    quickHull(input, n, input[ind], p2, -findSide(input[ind], p2, p1));
  }

public:
  QuickHull() = delete;
  QuickHull(core::Vec2* input_, const int n_) { set(input_, n_); }
  void set(core::Vec2* input_, const int n_) {
    input = input_;
    n     = n_;
  }
  QuickHull& run() {
    int min_x = 0, max_x = 0;
    for(int i = 1; i < n; i++) {
      if(input[i][0] < input[min_x][0]) min_x = i;
      if(input[i][0] > input[max_x][0]) max_x = i;
    }
    // Recursively find convex hull points on
    // one side of line joining input[min_x] and
    // input[max_x]
    quickHull(input, n, input[min_x], input[max_x], 1);
    // Recursively find convex hull points on
    // other side of line joining input[min_x] and
    // input[max_x]
    quickHull(input, n, input[min_x], input[max_x], -1);
    return (*this);
  }

  const core::Vec<core::Vec2>& get() const { return hull; }
  const core::Vec2* get_ptr() const { return hull.Data; }
  int size() const { return hull.size(); }
};

} // namespace mu::Magi
