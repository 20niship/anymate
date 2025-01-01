#pragma once

#include <iostream>
#include <movutl/core/vector.hpp>
#include <pcl/octree/octree_search.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/search/kdtree.h>
#include <pcl/search/search.h>
#include <pcl/segmentation/region_growing.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/surface/concave_hull.h>
#include <vector>

namespace mu::Magi {
class DBSCAN {
private:
  pcl::octree::OctreePointCloudSearch<pcl::PointXYZ> octree;
  pcl::PointCloud<pcl::PointXYZ>::Ptr m_cloud;
  std::vector<int> search(const pcl::PointXYZ& point, double radius) const;

  void expand(size_t idx);
  float m_radius;
  size_t min_neighbors_th;

  int m_current_cluster_idx;
  int n_processed_point;
  std::vector<bool> m_pt_checked;
  int next_not_checked_idx()const;

public:
  DBSCAN();
  void setCloud(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud);
  void set_radius(float r) { m_radius = r; }
  void set_min_neighbors_threshould(size_t r) { min_neighbors_th = r; }
  void run();
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr getColoredOutput()const;
  std::vector<std::vector<size_t>> clusters;
};

} // namespace mu::Magi
