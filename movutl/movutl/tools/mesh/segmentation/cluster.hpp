#pragma once
#include <movutl/core/vector.hpp>
#include <movutl/db/camera.hpp>
#include <movutl/db/image.hpp>
#include <movutl/db/mesh.hpp>
#include <movutl/tools/magi/fitting.hpp>

namespace mu::Magi {

struct Cluster {
public:
  std::vector<db::IndicesT> indices;
  ClusterType type = ClusterType::NotDefined;
  Magi::FittingResult fit_result;

  // int n;
  core::Vec3 pos_sum, rgb_sum, hsv_sum;                            // 点群の重心
  core::Vec3 min_pos, max_pos, min_rgb, max_rgb, min_hsv, max_hsv; // 以下、Lagori認識用

  core::Vec3 estimated_pos, estimated_size;
  Cluster() = default;
};

class ClusterManagerBase {
protected:
  db::MeshCol* m_mesh;

public:
  std::vector<Cluster> m_clusters;
  ClusterManagerBase()                      = default;
  virtual void set_cloud(db::MeshCol* mesh) = 0;
  virtual void run()                        = 0;
  void get_colored_output(db::MeshCol* m)const;
  int get_cluster_index(const db::IndicesT idx)const;
  std::vector<db::IndicesT> get_indices_of_cluster_idx(const int cluster_idx)const;
};

class ClusterManagerNormal : public ClusterManagerBase {
private:
  core::Vec<core::Vec3> m_normal;

public:
  ClusterManagerNormal() = default;
  void set_cloud(db::MeshCol*) override;
  void run() override;
};

class ClusterManagerRGB : public ClusterManagerBase {
private:
  core::Vec<core::Vec3> m_normal;

public:
  ClusterManagerRGB() = default;
  void set_cloud(db::MeshCol*) override;
  void run() override;
};

} // namespace mu::Magi
