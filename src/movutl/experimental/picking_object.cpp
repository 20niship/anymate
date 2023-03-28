#include <cwchar>
#include <movutl/experimental/picking_object.hpp>
#include <movutl/mu.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace mu;
using namespace mu::db;

namespace mu::experimental {

void PointCloudPicker::setup_renderer() {
  fb.set_size(width, height);
  fb.create();
} // namespace mu::experimental

void PointCloudPicker::render_frame() {
  r.push();
  r.sort();
  r.draw();
  r.clear_drawlist();
  if(r.mesh_col != nullptr) r.mesh_col->clear();
  if(r.mesh_ui != nullptr) r.mesh_ui->clear();
  if(r.mesh_3d != nullptr) r.mesh_3d->clear();
}

Shader* PointCloudPicker::create_shader_picking_object() {
  // return instance::get_ui_shader();
  const char* vs_src = R"(
    #version 450 
    uniform mat4 proj;
    uniform mat4 model;
    uniform mat4 view;
    uniform vec2 uvsize;
    in vec3 position;
    in vec3 color;
    in vec2 vuv;
    out vec4 outColor;
    
    vec3 unpackColor(int f){
        vec3 color;
        color.r = floor(f / 65536);
        color.g = floor((f - color.r * 65536) / 256.0);
        color.b = floor(f - color.r * 65536 - color.g * 256.0);
        return color / 255.0;
    }

    void main() {
      gl_Position = proj * model * vec4(position, 1.0);
      outColor = vec4(unpackColor(gl_VertexID).rgb, 1.0);
    }
  )";

  const char* fs_src = R"(
    #version 450 
    in vec4 outColor;
    out vec4 col;
    void main(void) {
      col= outColor;
    };
  )";

  auto s = new Shader(vs_src, fs_src);
  return s;
}
void PointCloudPicker::setup() {
  setup_renderer();
  r.init();
  r.m_framebuffer_id = fb.get_fbo_id();
  auto s             = create_shader_picking_object();
  MU_ASSERT(s->enabled());
  r.set_shader(s);
  r.point_size(3);
}

void PointCloudPicker::set_mesh(MeshCol* m) {
  r.add_mesh(m);
  mesh_vertex_size = m->get_vertices_size();
  octree.set_dataset(m);
  octree.report();
  mesh = m;
}

void PointCloudPicker::update(const CameraPosition& c) {
  r.camera = c;
  render_frame();
  const int ch    = 3;
  GLubyte* pixels = new GLubyte[width * height * ch];
  glBindTexture(GL_TEXTURE_2D, fb.get_texture_id());
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  img = cv::Mat(height, width, CV_8UC3, pixels);
  cv::flip(img, img, 0);
  // delete[] pixels;
}

int PointCloudPicker::get_point(const Vec2d& pos) {
  if(pos[0] < 0 || pos[1] < 0 || pos[0] > width || pos[1] > height) return -1;
#if 0
    unsigned char data[3];
    fb.bind();
    glFlush();
    glFinish();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(pos[0], height - pos[1], 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
    const size_t index = (size_t)data[2] + ((size_t)data[1] << 8) + ((size_t)data[0] << 16);
    fb.unbind();
#else
  const auto x       = pos[0];
  const auto y       = pos[1];
  const size_t r     = img.data[(y * width + x) * 3];
  const size_t g     = img.data[(y * width + x) * 3 + 1];
  const size_t b     = img.data[(y * width + x) * 3 + 2];
  const size_t index = b + (g << 8) + (r << 16);

  // const auto data    = img.at<cv::Vec3b>(y, x);
  // DISP((int)data[0]);
  // DISP((int)data[1]);
  // DISP((int)data[2]);
  // const size_t index = (size_t)data[2] + ((size_t)data[1] << 8) + ((size_t)data[0] << 16);
#endif
  return (index < mesh_vertex_size) ? index : -1;
}

Rect3D PointCloudPicker::get_bbox_from_indices(const std::vector<int> indices) const {
  Rect3D bbox;
  bbox.clear();
  for(const auto& i : indices) {
    const auto p = mesh->vertex[i].pos;
    bbox.expand(p[0], p[1], p[2]);
  }
  return bbox;
}

std::vector<int> PointCloudPicker::get_inside_bbox(const core::Rect3D& bbox) const {
  std::vector<int> res;
  for(size_t i = 0; i < mesh->get_vertices_size(); i++) {
    const auto p = mesh->vertex[i].pos;
    if(bbox.contains(p[0], p[1], p[2])) res.push_back(i);
  }
  return res;
}

std::vector<size_t> PointCloudPicker::get_nearby(const core::Vec3& pos) const {
  const auto res = octree.findNearest(pos, SearchMethod::Sphere, m_select_nearby_radius);
  return res;
}

std::vector<int> PointCloudPicker::get_point_rect(const Vec2d& start, const Vec2d& end) {
  std::vector<int> res;
  for(int y = start[1]; y < end[1]; y++) {
    for(int x = start[0]; x < end[0]; x++) {
      int pt = get_point(Vec2d(x, y));
      if(pt > 0) res.push_back(pt); // index = 0の点が選択できない
    }
  }
  if(m_select_in_bbox) {
    const auto bbox = get_bbox_from_indices(res);
    return get_inside_bbox(bbox);
  }
  if(m_select_nearby) {
    const auto bbox = get_bbox_from_indices(res);
    const auto r2   = get_nearby(bbox.center<double>());
    for(const auto& p : r2) res.push_back(p);
  }
  return res;
}

std::vector<int> PointCloudPicker::get_point_circle(const Vec2d& pos, const float radius) {
  std::vector<int> res;
  for(size_t y = pos[1] - radius; y < pos[1] + radius; y++) {
    for(size_t x = pos[0] - radius; x < pos[0] + radius; x++) {
      int pt = get_point(Vec2d(x, y));
      if(pt >= 0) res.push_back(pt);
    }
  }
  if(m_use_clusters && m_clusters != nullptr){ 
    const int pt = get_point(pos); 
    if(pt < 0) return res;
    const auto idx = m_clusters->get_cluster_index(pt);
    if(idx < 0) return res;
    const auto same_cluster_indices = m_clusters->get_indices_of_cluster_idx(idx);
    for(const auto& p : same_cluster_indices) res.push_back(p);
  }
  return res;
}

void PointCloudPicker::set_cluster_manager(const Magi::ClusterManagerBase* cl) {
  m_clusters     = cl;
  m_use_clusters = true;
}

PointCloudPicker::~PointCloudPicker() { r.terminate(); }
} // namespace mu::experimental
