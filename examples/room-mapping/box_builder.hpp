#pragma once

#include "plane_estimation.hpp"
#include <movutl/core/rect.hpp>

class BoxConstructor {
private:
  Vec<bool> plane_used;

  std::vector<int> get_neighbor_planes(const size_t plane_idx) const {
    if(planes.size() <= plane_idx) return {};
    std::vector<int> neighbors;
    for(size_t i = 0; i < planes.size(); i++) {
      // 領域をちょっと拡大してマッチする？
      if(plane_idx == i) continue;
      if(rect_contains_with_scale(planes[i].r, planes[plane_idx].r)) neighbors.push_back(i);
    }
    return neighbors;
  }

  Rect3D get_root_rect(const Plane& p) const {
    Rect3D r = p.r;
    if(p.dir == NORMAL_X) r.x.min = r.x.max = p.r.x.center();
    if(p.dir == NORMAL_Y) r.y.min = r.y.max = p.r.x.center();
    if(p.dir == NORMAL_Z) r.z.min = r.z.max = p.r.x.center();
    return r;
  }

  bool valid_boundary(const Plane& p, const Rect3D& rect) {
    Range<float> r;
    float v;
    switch(p.dir) {
      case NORMAL_X:
        r = rect.x;
        v = p.r.x.center();
        break;
      case NORMAL_Y:
        r = rect.y;
        v = p.r.y.center();
        break;
      case NORMAL_Z:
        r = rect.z;
        v = p.r.z.center();
        break;
      case NORMAL_NOT_DEFINED: return false;
    }
    if(std::abs(v - r.min) < min_err_between_bondary) return false;
    if(std::abs(v - r.max) < min_err_between_bondary) return false;
    return true;
  }

  bool rect_contains_with_scale(const Rect3D& r1, const Rect3D r2) const {
    return r1.margin(margin).contains(r2.margin(margin));
  }

  void compute(const int plane_idx) {
    std::vector<Rect3D> boxes_tmp;
    boxes_tmp.push_back(get_root_rect(planes[plane_idx]));
    plane_used[plane_idx] = true;

    const auto nei = get_neighbor_planes(plane_idx);
    if(nei.size() == 0) {
      LOGE << "Neighbor size = 0!!";
      return;
    }

    for(const auto& n : nei) {
      size_t i = 0;
      while(i < boxes_tmp.size()) {
        do {
          if(!rect_contains_with_scale(planes[n].r, boxes_tmp[i])) break;
          if(!valid_boundary(planes[n], boxes_tmp[i])) break;

          plane_used[n] = true;
          /* if(i > 100) exit(1); */
          Rect3D r1 = boxes_tmp[i];
          Rect3D r2 = boxes_tmp[i];
          switch(planes[n].dir) {
            case NORMAL_X: {
              r1.x.min = planes[n].r.x.center();
              r2.x.max = planes[n].r.x.center();

              r1.y.merge(planes[n].r.y);
              r2.y.merge(planes[n].r.y);

              r1.z.merge(planes[n].r.z);
              r2.z.merge(planes[n].r.z);
              break;
            }
            case NORMAL_Y: {
              r1.y.min = planes[n].r.y.center();
              r2.y.max = planes[n].r.y.center();

              r1.x.merge(planes[n].r.x);
              r2.x.merge(planes[n].r.x);

              r1.z.merge(planes[n].r.z);
              r2.z.merge(planes[n].r.z);
              break;
            }
            case NORMAL_Z: {
              r1.z.min = planes[n].r.z.center();
              r2.z.max = planes[n].r.z.center();

              r1.x.merge(planes[n].r.x);
              r2.x.merge(planes[n].r.x);

              r1.y.merge(planes[n].r.y);
              r2.y.merge(planes[n].r.y);
              break;
            }
            default: LOGE << " NORMAL NOT DEFINED!!";
          }
          boxes_tmp[i] = r1;
          boxes_tmp.push_back(r2);
        } while(0);
        i++;
      }
    }
    if(boxes_tmp.size() > 1)
      for(const auto& b : boxes_tmp) boxes.push_back(b);
  }

public:
  Vec<mu::core::Rect3D> boxes;
  float min_err_between_bondary = 0.244;
  float margin = 0.0264;

  std::vector<Plane> planes;
  BoxConstructor()  = default;
  ~BoxConstructor() = default;

  void set_plane(const std::vector<Plane>& p) {
    planes = p;
    plane_used.resize(p.size());
    plane_used.fill(false);
  }

  void compute() {
    boxes.clear();
    /* const auto sorted = */
    // TODO: sort by plane area!!
    for(size_t i = 0; i < planes.size(); i++) {
      if(!plane_used[i]) compute(i);
    }
  }
};
