#include <algorithm>
#include <iostream>
#include <istream>
#include <limits>
#include <list>
#include <movutl/core/assert.hpp>
#include <movutl/core/logger.hpp>
#include <movutl/tools/magi/fitting.hpp>
#include <opencv2/imgproc.hpp>
#include <random>
#include <vector>

namespace mu::Magi {

using namespace mu::core;

void TerrainFitting::reset() {
  n             = 0;
  result_.error = -1;
  bbox.clear();
}

auto ra(const Range<float> base, int n, int idx) {
  Range<float> res;
  const auto sp = (base.length() / n);
  res.min       = base.min + sp * idx;
  res.max       = base.min + sp * (idx + 1);
  return res;
}

int TerrainFitting::get_idx(const Vec3& p) const {
  MU_ASSERT(sp_x > 0);
  MU_ASSERT(sp_y > 0);
  for(int y = 0; y < sp_y; y++) {
    for(int x = 0; x < sp_x; x++) {
      Rect bbox_tmp;
      bbox_tmp.x = ra(bbox.x, sp_x, x);
      bbox_tmp.y = ra(bbox.y, sp_y, y);
      if(bbox_tmp.contains(p[0], p[1])) return y * sp_x + x;
    }
  }
  LOGW << p << "\n" << bbox << "out of range";
  return -1;
}

struct TerrainData {
  std::vector<Vec3> cloud;
  PlaneFitting fit;
};

auto center(const std::vector<Vec3>& cl) {
  Vec3 p = Vec3(0, 0, 0);
  for(const auto& c : cl) p += c;
  return p / cl.size();
}

void TerrainFitting::estimate() {
  MU_ASSERT(sp_x > 0);
  MU_ASSERT(sp_y > 0);
  MU_ASSERT(n > 0);
  std::vector<TerrainData> mat;
  mat.resize(sp_x * sp_y);
  for(size_t i = 0; i < mat.size(); i++) mat[i].cloud.reserve(n / sp_x / sp_y / 10);
  for(size_t i = 0; i < n; i++) {
    const auto p   = cloud[i];
    const auto idx = get_idx(p);
    if(idx < 0) continue;
    mat[idx].cloud.push_back(p);
  }

  terrain.resize(sp_x);
  for(size_t i = 0; i < terrain.size(); i++) terrain[i].resize(sp_y);

  for(int x = 0; x < sp_x; x++) {
    for(int y = 0; y < sp_y; y++) {
      const auto idx = sp_x * y + x;
      terrain[x][y]  = center(mat[idx].cloud);
    }
  }

  return;

  for(size_t i = 0; i < mat.size(); i++) {
    if(mat[i].cloud.size() < 10) continue;
    mat[i].fit.reset();
    mat[i].fit.setCloud(mat[i].cloud.data(), mat[i].cloud.size());
    mat[i].fit.estimate();
  }
}

void TerrainFitting::setCloud(Vec3* cloud_, const size_t pc_size) {
  reset();
  cloud = cloud_;
  n     = pc_size;
  for(size_t i = 0; i < pc_size; i++) bbox.expand(cloud[i]);
}

void TerrainFitting::operator<<(const Vec3) {
  LOGE << "NOT IMPLREMENTED";
  exit(1);
}

void TerrainFitting::nSplit(int x, int y) {
  sp_x = x;
  sp_y = y;
}

} // namespace mu::Magi

