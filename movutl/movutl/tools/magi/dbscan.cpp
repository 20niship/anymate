#include "dbscan.hpp"
#include <external/pbar.hpp>
#include <movutl/core/logger.hpp>
#include <movutl/ui/colors.hpp>

namespace mu::Magi {

float resolution = 128.0f;
DBSCAN::DBSCAN() : octree(pcl::octree::OctreePointCloudSearch<pcl::PointXYZ>(resolution)) {}

void DBSCAN::setCloud(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud) {
  m_cloud = cloud;
  octree.setInputCloud(m_cloud);
  octree.addPointsFromInputCloud();

  m_pt_checked.resize(m_cloud->size());
  for(size_t i = 0; i < m_cloud->size(); i++) m_pt_checked[i] = false;
}

std::vector<int> DBSCAN::search(const pcl::PointXYZ& , double ) const {
  std::vector<int> indices;
  std::vector<float> dist;
  /* std::cout << "search at (" << searchPoint.x << " " << searchPoint.y << " " << searchPoint.z << ") with radius=" << radius << std::endl; */
  //const bool nfound = octree.radiusSearch(searchPoint, radius, indices, dist);
  /* if(nfound > 0) { */
  /*   for(std::size_t i = 0; i < indices.size(); ++i) std::cout << "    " << (*m_cloud)[indices[i]].x << " " << (*m_cloud)[indices[i]].y << " " << (*m_cloud)[indices[i]].z << " (squared distance: " << dist[i] << ")" << std::endl; */
  /* } */
  return indices;
}

void DBSCAN::expand(size_t idx) {
  m_pt_checked[idx] = true;
  n_processed_point++;
  const auto p         = m_cloud->points[idx];
  const auto neighbors = search(p, m_radius);
  std::cout << (float)n_processed_point * 100.0f / m_cloud->size() << ", " << neighbors.size() << ", " << m_current_cluster_idx << std::endl;
  if(neighbors.size() < min_neighbors_th) return;
  MU_ASSERT(m_pt_checked.size() > idx);

  if((size_t)m_current_cluster_idx + 1 > clusters.size()) clusters.resize(m_current_cluster_idx + 1);
  clusters[m_current_cluster_idx].push_back(idx);
  for(const auto& pt : neighbors)
    if(!m_pt_checked[pt]) expand(pt);
}

int DBSCAN::next_not_checked_idx() const {
  for(size_t i = 0; i < m_pt_checked.size(); i++)
    if(!m_pt_checked[i]) return i;
  return -1;
}

void DBSCAN::run() {
  MU_ASSERT(m_radius > 0);
  MU_ASSERT(min_neighbors_th > 0);
  m_current_cluster_idx = 0;
  clusters.clear();
  if(m_cloud->size() == 0) {
    LOGE << "input cloud size is 0";
    return;
  }
  while(true) {
    const auto next = next_not_checked_idx();
    if(next < 0) return;
    expand(next);
    m_current_cluster_idx++;
  }
}

pcl::PointCloud<pcl::PointXYZRGB>::Ptr DBSCAN::getColoredOutput() const {
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr pc(new pcl::PointCloud<pcl::PointXYZRGB>);
  pc->points.resize(m_cloud->size());

  for(size_t i = 0; i < m_cloud->size(); ++i) {
    pc->points[i].x = m_cloud->points[i].x;
    pc->points[i].y = m_cloud->points[i].y;
    pc->points[i].z = m_cloud->points[i].z;
    pc->points[i].r = 60;
    pc->points[i].g = 0;
    pc->points[i].b = 0;
  }

  int i = 0;
  using mu::colors::colormap;
  for(const auto& c : clusters) {
    const auto col = colormap[i % colormap.size()];
    i++;
    for(const auto& idx : c) {
      pc->points[idx].r = col[0];
      pc->points[idx].g = col[1];
      pc->points[idx].b = col[2];
    }
  }

  return pc;
}

} // namespace mu::Magi
