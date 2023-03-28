#include <movutl/core/vector.hpp>
#include <movutl/db/mesh.hpp>
#include <movutl/tools/mesh/normal_estimation.hpp>

#include <pcl/features/normal_3d.h>
#include <pcl/point_types.h>

using namespace mu;
using namespace mu::core;
using namespace mu::db;

using mu::core::Vec;
using mu::db::MeshCol;

namespace mu::tools {

template <typename T> void normal_estimate(MeshCol* m, Vec<_Vec<T, 3>>& normals, const float R) {
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
  pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> ne;

  std::cout << "importing to pcl::PointCloud......." << std::endl;
  {
    using pcl::PointXYZ;
    for(size_t i = 0; i < m->vertex.size(); i++) {
      const auto p = m->vertex[i].pos;
      cloud->push_back(pcl::PointXYZ(p[0], p[1], p[2]));
    }
  }

  std::cout << "start NormalEstimation......." << std::endl;
  ne.setInputCloud(cloud);
  std::cout << "set input cloud......." << std::endl;
  pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>());
  ne.setSearchMethod(tree);
  std::cout << "set tree......." << std::endl;
  pcl::PointCloud<pcl::Normal>::Ptr cloud_normals(new pcl::PointCloud<pcl::Normal>);
  std::cout << "create normal!......." << std::endl;

  // Use all neighbors in a sphere of radius 3cm
  ne.setRadiusSearch(R);
  std::cout << "computing normals......." << std::endl;
  ne.compute(*cloud_normals);

  const size_t s = cloud_normals->size();
  DISP(s);
  normals.resize(s);
  for(size_t i = 0; i < s; i++) {
    const auto n = cloud_normals->points[i];
    normals[i]   = {n.normal_x, n.normal_y, n.normal_z};
  }
}


template void normal_estimate(MeshCol*, Vec<_Vec<double, 3>>&, const float);
template void normal_estimate(MeshCol*, Vec<_Vec<float, 3>>&, const float);

} // namespace mu::tools
