#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

#include <movutl/core/vector.hpp>
#include <movutl/instance/instance.hpp>
#include <movutl/io/object_loader.hpp>

namespace mu::io {
db::Body* impl_load_pcd(const char* fname) {
  DISP(fname);
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr colored_cloud;

  auto raw_cloud = static_cast<pcl::PointCloud<pcl::PointXYZRGB>::Ptr>(new pcl::PointCloud<pcl::PointXYZRGB>());
  if(pcl::io::loadPCDFile<pcl::PointXYZRGB>(fname, *raw_cloud) == -1) {
    LOGE << "Cloud reading failed.";
    return nullptr;
  }
  std::cout << raw_cloud->size() << " pc found1 " << std::endl;

  std::ifstream fin(fname, std::ios::in | std::ios::binary);
  if(!fin) {
    LOGE << "file is not found." << fname;
    return nullptr;
  }

  db::Body* b = instance::create_body();
  auto m      = instance::create_mesh_col();
  MU_ASSERT(m);
  const auto pc_size = raw_cloud->points.size();

  // 各ポリゴンの法線と座標の読み込み
  m->vertex.resize(pc_size);

  for(size_t i = 0; i < pc_size; i++) {
    const auto pt       = raw_cloud->points[i];
    m->vertex[i].pos[0] = pt.x;
    m->vertex[i].pos[1] = pt.y;
    m->vertex[i].pos[2] = pt.z;
#if 0
    std::uint32_t rgb = *reinterpret_cast<int*>((void *)&pt.rgb);
    std::uint8_t r = (rgb >> 16) & 0x0000ff;
    std::uint8_t g = (rgb >> 8)  & 0x0000ff;
    std::uint8_t b = (rgb)       & 0x0000ff;
    m->vertex[i].col[0] = r;
    m->vertex[i].col[1] = g;
    m->vertex[i].col[2] = b;
#else
    m->vertex[i].col[0] = pt.r;
    m->vertex[i].col[1] = pt.g;
    m->vertex[i].col[2] = pt.b;
#endif
  }
  m->primitive_type(db::_MeshBase::PrimitiveType::POINTS);
  b->meshs.push_back(m);
  return b;
}

} // namespace mu::io
