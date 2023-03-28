/// DEMO .pcd -> .ply file


#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>

#define DISP(A) std::cout << #A << " = " << (A) << std::endl

using namespace pcl;
using namespace pcl::io;
using namespace pcl::console;

bool loadCloud(const std::string& filename, pcl::PCLPointCloud2& cloud) {
  DISP(filename);
  if(loadPCDFile(filename, cloud) < 0) return (false);
  DISP(cloud.width);
  DISP(cloud.height);
  DISP(pcl::getFieldsList(cloud).c_str());
  return (true);
}

void saveCloud(const std::string& filename, const pcl::PCLPointCloud2& cloud, bool binary, bool use_camera) {
  print_highlight("Saving ");
  print_value("%s ", filename.c_str());

  pcl::PLYWriter writer;
  writer.write(filename, cloud, Eigen::Vector4f::Zero(), Eigen::Quaternionf::Identity(), binary, use_camera);

  print_value("%d", cloud.width * cloud.height);
  print_info(" points]\n");
}

/* ---[ */
int main(int argc, char** argv) {
  std::cout << "FFFFFFF == " << argv[0] << std::endl;

  bool format     = true; //True = binary, false=ascii
  bool use_camera = true; //True = using camera, false=no camera

  pcl::PCLPointCloud2 cloud;
  if(!loadCloud(argv[0], cloud)) return (-1);

  // Convert to PLY and save
  saveCloud("hoge.ply", cloud, format, use_camera);

  return (0);
}
