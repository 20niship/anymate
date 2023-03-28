#pragma once
#include <movutl/core/octree.hpp>
#include <movutl/db/mesh.hpp>
#include <movutl/db/shader.hpp>
#include <movutl/render/framebuffer.hpp>
#include <movutl/tools/mesh/segmentation/cluster.hpp>
#include <opencv2/opencv.hpp>

namespace mu::experimental {

class PointCloudPicker {
private:
  const int width  = 1080;
  const int height = 720;

  mu::render::Render r;
  mu::render::Framebuffer fb;
  size_t mesh_vertex_size;
  cv::Mat img;
  db::Shader* create_shader_picking_object();
  mu::core::Octree2<double, 10> octree;
  db::MeshCol* mesh;
  const Magi::ClusterManagerBase *m_clusters;

  bool m_select_in_bbox        = true;
  bool m_select_nearby         = true;
  bool m_use_clusters          = false;
  float m_select_nearby_radius = 0.03;

  void setup_renderer();
  void render_frame();

  core::Rect3D get_bbox_from_indices(const std::vector<int>) const;
  std::vector<int> get_inside_bbox(const core::Rect3D&) const;
  std::vector<size_t> get_nearby(const core::Vec3&) const;

public:
  PointCloudPicker() = default;
  ~PointCloudPicker();

  void setup();
  void set_mesh(db::MeshCol* m);
  void set_cluster_manager(const Magi::ClusterManagerBase *);
  void update(const db::CameraPosition& c);

  void select_nearby(bool k, float radius) {
    m_select_nearby        = k;
    m_select_nearby_radius = radius;
  }
  void select_in_bbox(bool k) { m_select_in_bbox = k; }
  void set_use_cluter(bool k){ m_use_clusters = k; }

  int get_point(const core::Vec2d& pos);
  std::vector<int> get_point_rect(const core::Vec2d& start, const core::Vec2d& end);
  std::vector<int> get_point_circle(const core::Vec2d& pos, const float radius);
};

} // namespace mu::experimental
