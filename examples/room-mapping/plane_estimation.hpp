#pragma once

#include <filesystem>
#include <stack>
#include <vector>

#include <movutl/core/logger.hpp>
#include <movutl/core/octree.hpp>
#include <movutl/core/rect.hpp>
#include <movutl/core/vector.hpp>
#include <movutl/db/mesh.hpp>
#include <movutl/ui/colors.hpp>

using namespace mu::core;
using namespace mu::db;

enum PlaneNormalDir {
  NORMAL_NOT_DEFINED = 0,
  NORMAL_X,
  NORMAL_Y,
  NORMAL_Z,
};

template <typename T> inline float cos_similarity(const _Vec<T, 3>& a, const _Vec<T, 3>& b) { return std::abs(a.dot(b) / (a.norm() * b.norm())); }

template <typename T> inline PlaneNormalDir get_normal_dir(const _Vec<T, 3>& n, const double min_error = 0.15) {
  const double cos_sim_min = 1.0 - min_error;
  const _Vec<T, 3> cx(1, 0, 0);
  const _Vec<T, 3> cy(0, 1, 0);
  const _Vec<T, 3> cz(0, 0, 1);
  if(cos_similarity(cx, n) > cos_sim_min) return NORMAL_X;
  if(cos_similarity(cy, n) > cos_sim_min) return NORMAL_Y;
  if(cos_similarity(cz, n) > cos_sim_min) return NORMAL_Z;
  return NORMAL_NOT_DEFINED;
}

struct Plane {
  size_t nPoints;
  mu::core::Rect3D r;
  PlaneNormalDir dir;
  mu::core::Vec3 p;
};

struct PlaneEstimationParams {
  float min_rotation_error    = 0.12;
  float min_plane_point_error = 0.1; //! 現在の平面と点の最小距離[meter]
  float min_pos_error_search  = 0.4;
  float min_pos_error         = 0.4;

  size_t min_plane_points = 30; //! minimum point size for plane
  int min_plane_area   = 0;  //! minimum plane area
};


#define PLANE_NOT_DEFINED -1
#define NOT_PLANE -2

template <typename T = float> class PlaneDetection {
private:
  mu::db::MeshCol* m;
  mu::core::Vec<mu::core::_Vec<T, 3>> normals; //! normals for each pointcloud points
  mu::core::Vec<int16_t> plane_ids;            //! plane id for each pointcloud points
  std::stack<size_t> idx_stack;                //! points to process

  mu::core::Vec<mu::core::Vec3> dataset;
  mu::core::Octree2<double, 10> octree;


  void setup_octree() {
    const auto nData = m->vertex.size();
    dataset.resize(nData);
    for(size_t i = 0; i < nData; i++) {
      dataset[i][0] = m->vertex[i].pos[0];
      dataset[i][1] = m->vertex[i].pos[1];
      dataset[i][2] = m->vertex[i].pos[2];
    }
    std::cout << "start octree" << std::endl;
    octree.set_dataset(&dataset);
    octree.report();
    std::cout << "inserting......" << std::endl;
  }

#if 1
  std::vector<size_t> find_near_points(const size_t index) {
    if(dataset.size() <= index) {
      LOGE << "dataset size <= index!";
      exit(1);
      return {};
    }
    const auto query = dataset[index];
    const auto res   = octree.findNearest(query, mu::core::SearchMethod::Sphere, params.min_pos_error_search);
    std::cout << "plane estimation Neighbor = " << res.size() << std::endl;
    return res;
  }
#else
  std::vector<size_t> find_near_points(const size_t index) {
    //! bluteforce
    if(dataset.size() <= index) {
      LOGE << "dataset size <= index!";
      exit(1);
      return {};
    }
    std::vector<size_t> res;
    for(int i = 0; i < dataset.size(); i++)
      if((dataset[i] - dataset[index]).norm() < params.min_pos_error_search) res.push_back(i);
    return res;
  }
#endif

  int get_next_not_computed_index() {
    for(auto i = 0; i < plane_ids.size(); i++) {
      if(plane_ids[i] < 0) return i;
    }
    return -1;
  }

  bool check_angle(const size_t idx1, const size_t idx2) {
    const auto n1 = normals[idx1];
    const auto n2 = normals[idx2];

    const auto cos_similarity = n1.dot(n2) / (n1.norm() * n2.norm());
    /* DISP(n1); */
    /* DISP(n2); */
    /* DISP(cos_similarity); */
    return std::abs(cos_similarity) > params.min_rotation_error;
  }

  Vec3 get_normal_from(const PlaneNormalDir dir) {
    if(dir == NORMAL_X) return Vec3(1, 0, 0);
    if(dir == NORMAL_Y) return Vec3(0, 1, 0);
    if(dir == NORMAL_Z) return Vec3(0, 0, 1);
    return Vec3(0, 0, 0);
  }

  bool check_distance_plane(const size_t idx1, const size_t idx2, const PlaneNormalDir dir) {
    if(dir == NORMAL_NOT_DEFINED) return false;
    const auto p1 = dataset[idx1] - dataset[idx2];
    const auto dn = get_normal_from(dir);

    return dn.dot(p1) < params.min_plane_point_error;

    const auto error = std::abs(dn.dot(p1) / p1.norm());
    return error < params.min_plane_point_error;
  }

  bool check_distance(const size_t idx1, const size_t idx2) {
    const auto p1 = dataset[idx1];
    const auto p2 = dataset[idx2];
    return (p1 - p2).norm_sq() < params.min_pos_error * params.min_pos_error;
  }

  void add_plane(size_t point_idx, size_t plane_idx, const PlaneNormalDir dir) {
    if(planes.size() <= plane_idx) planes.resize(plane_idx + 1);
    planes[plane_idx].nPoints++;
    planes[plane_idx].dir = dir;
    planes[plane_idx].r.expand(dataset[point_idx]);
    plane_ids[point_idx] = plane_idx;
  }

  void grow(int seed) {
    const auto seed_normal_dir = get_normal_dir(normals[seed], params.min_rotation_error);
    if(seed_normal_dir == NORMAL_NOT_DEFINED) return;

    std::stack<size_t> point_stack;
    point_stack.push(seed);

    const auto next_plane_idx = planes.size();
    {
      const auto cur_idx          = point_stack.top();
      const auto neighbor_indices = find_near_points(cur_idx);
      /* std::cout << seed << " / " << normals.size() << " n = " << point_stack.size() << " - neighor = " << neighbor_indices.size() << std::endl; */
    }

    while(!point_stack.empty()) {
      const auto cur_idx          = point_stack.top();
      const auto neighbor_indices = find_near_points(cur_idx);
      point_stack.pop();
      if(neighbor_indices.size() == 0) return;

      for(size_t idx : neighbor_indices) {
        const auto dir = get_normal_dir(normals[idx], params.min_rotation_error);
        if(idx == cur_idx) continue;
        if(dir == NORMAL_NOT_DEFINED) continue;
        /* if(!check_angle(cur_idx, idx)) continue; */
        if(seed_normal_dir != dir) continue;
        /* if(!check_distance_plane(seed, idx, dir)) continue; */
        if(!check_distance(cur_idx, idx)) continue;
        if(plane_ids[idx] != PLANE_NOT_DEFINED) continue;

        add_plane(idx, next_plane_idx, dir);
        point_stack.push(idx);
      }
    }
  }

  void merge_planes() {
    // merge planes and delete small pla
  }

  void delete_small_planes() {
    if(planes.size() == 0) return;
    size_t i = 0;
    while(i < planes.size()) {
      if(planes[i].nPoints > params.min_plane_points && planes[i].r.area() > params.min_plane_area) {
        i++;
        continue;
      }
      planes.erase(planes.begin() + i);
    }
  }

public:
  std::vector<Plane> planes;
  PlaneEstimationParams params;
  void set(mu::db::MeshCol* m_, const mu::core::Vec<_Vec<T, 3>>& n) {
    m       = m_;
    normals = n;
    plane_ids.resize(m->get_vertices_size());
    plane_ids.fill(PLANE_NOT_DEFINED);
    idx_stack = std::stack<size_t>();
    setup_octree();
  }

  void compute() {
    DISP(plane_ids.size());
    // compute all planes
    for(size_t i = 0; i < plane_ids.size(); i++) {
      if(plane_ids[i] != PLANE_NOT_DEFINED) continue;
      grow(i);
    }
    merge_planes();
    delete_small_planes();
  }

  void apply_colormap() {
    const auto colors = mu::colors::colormap;
    const auto nc     = colors.size();

    for(int i = 0; i < plane_ids.size(); i++) {
      const auto pi = plane_ids[i];
      if(pi < 0) {
        m->vertex[i].col[0] = 0;
        m->vertex[i].col[1] = 0;
        m->vertex[i].col[2] = 0;
      } else {
        m->vertex[i].col[0] = colors[(pi % nc) - 1][0];
        m->vertex[i].col[1] = colors[(pi % nc) - 1][1];
        m->vertex[i].col[2] = colors[(pi % nc) - 1][2];
      }
    }
  }

  void colormap_by_normal() {
    const auto colors = mu::colors::colormap;
    const auto nc     = colors.size();
    for(size_t i = 0; i < normals.size(); i++) {
      const auto k        = get_normal_dir(normals[i], params.min_rotation_error);
      m->vertex[i].col[0] = colors[k % nc][0];
      m->vertex[i].col[1] = colors[k % nc][1];
      m->vertex[i].col[2] = colors[k % nc][2];
    }
  }

  void report() const {
    octree.report();
    DISP(planes.size());
    DISP(normals.size());
  }
};
