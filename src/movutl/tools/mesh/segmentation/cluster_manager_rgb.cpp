#include <movutl/tools/colormap.hpp>
#include <movutl/tools/mesh/normal_estimation.hpp>
#include <movutl/tools/mesh/segmentation/cluster.hpp>
#include <movutl/ui/colors.hpp>

#include <pcl/features/normal_3d.h>
#include <pcl/segmentation/region_growing.h>
#include <pcl/segmentation/region_growing_rgb.h>

namespace mu::Magi {

using namespace mu::core;
using namespace mu::db;

void ClusterManagerRGB::set_cloud(MeshCol* m) { m_mesh = m; }
void ClusterManagerRGB::run() {
  /* tools::normal_estimate(m_mesh, m_normal); */

  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_ptr(new pcl::PointCloud<pcl::PointXYZRGB>());
  pcl::PointCloud<pcl::Normal>::Ptr cloud_normals(new pcl::PointCloud<pcl::Normal>());
  /* pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_segmented(new pcl::PointCloud<pcl::PointXYZRGB>()); */

  cloud_ptr->width  = m_mesh->get_vertices_size();
  cloud_ptr->height = 1;
  cloud_ptr->points.resize(m_mesh->get_vertices_size());
  for(size_t i = 0; i < m_mesh->get_vertices_size(); i++) {
    cloud_ptr->points[i].x = m_mesh->vertex[i].pos[0];
    cloud_ptr->points[i].y = m_mesh->vertex[i].pos[1];
    cloud_ptr->points[i].z = m_mesh->vertex[i].pos[2];
    cloud_ptr->points[i].r = m_mesh->vertex[i].col[0];
    cloud_ptr->points[i].g = m_mesh->vertex[i].col[1];
    cloud_ptr->points[i].b = m_mesh->vertex[i].col[2];
  }

  pcl::NormalEstimation<pcl::PointXYZRGB, pcl::Normal> ne;
  ne.setInputCloud(cloud_ptr);
  pcl::search::KdTree<pcl::PointXYZRGB>::Ptr tree_n(new pcl::search::KdTree<pcl::PointXYZRGB>());
  ne.setSearchMethod(tree_n);
  ne.setRadiusSearch(0.3);
  ne.compute(*cloud_normals);

  // Region growing
  pcl::RegionGrowingRGB<pcl::PointXYZRGB, pcl::Normal> rg;
  rg.setSmoothModeFlag(false);
  rg.setRegionColorThreshold(4);
  rg.setPointColorThreshold(5);
  rg.setMinClusterSize(10);
  rg.setInputCloud(cloud_ptr);

  std::vector<pcl::PointIndices> clusters;
  rg.extract(clusters);
  m_clusters.resize(clusters.size());
  for(size_t i = 0; i < clusters.size(); i++) {
    const auto indices = clusters[i].indices;
    if(indices.size() < 2) continue;
    m_clusters[i].indices.reserve(indices.size());
    for(size_t k = 0; k < indices.size(); k++) m_clusters[i].indices.push_back(indices[k]);
  }

  /* { */
  /*   Vec<Vec3> normals; */
  /*   normals.resize(cloud_normals->points.size()); */
  /*   for(size_t i = 0; i < normals.size(); i++) { */
  /*     normals[i][0] = cloud_normals->points[i].normal_x; */
  /*     normals[i][1] = cloud_normals->points[i].normal_y; */
  /*     normals[i][2] = cloud_normals->points[i].normal_z; */
  /*   } */
  /*   tools::colormap_by_normal(m_mesh, m_mesh, normals); */
  /* } */
}

} // namespace mu::Magi
