#pragma once

#include <opencv2/core/persistence.hpp>
#ifdef WITH_OPENGL // CMakeLists.txtをみてね
#define GLEW_STATIS
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#else
#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#endif

#include <stb_image.h>

#include <movutl/core/rect.hpp>
#include <movutl/core/vector.hpp>
#include <movutl/db/body.hpp>
#include <movutl/db/camera.hpp>
#include <movutl/db/image.hpp>
#include <movutl/db/mesh.hpp>
#include <movutl/db/shader.hpp>

#include <movutl/render/vk/buffer.hpp>

#include <opencv2/opencv.hpp>

#define VK_ENGINE_MAX_FRAMES_IN_FLIGHT 2
#define VK_ENGINE_ENABLE_VALIDATION_LAYERS
#define VKUI_ENGINE_ENABLE_FPS_CALC
/* #define VKUI_ENGINE_USE_FLOAT_VERTEX */

namespace mu::render {

/*
 * 一つのpipelineを持ったレンダリング class
 * 1つのObjectにつき1つのRenderクラスがつく。
 * uiWindowクラスも1つのRenderインスタンスを持つ。
 *
 * usage:
 *  Render render;
 *  render.bind_window(uiWindow *wnd);
 *  render.new_frame();
 *  render.draw(DrawData *dd);
 *  render.draw(DrawData *dd);
 *  render.end();
 */
class Render {
  private:
    void create_default_mesh();
public:
  /* private: */
  struct DrawCmd {
    int start, size;
    int tex_id;
    int z_index;
    core::Vec2d scissor_top, scissor_btm;
  };

  db::Shader* shader = nullptr;
  int _last_end, _tex_id, _z_index;
  core::Vec<DrawCmd> drawlist;
  db::Mesh2D* mesh_ui;
  db::Mesh* mesh_3d;
  db::MeshCol* mesh_col;
  /* core::Vec<db::_MeshBase*> meshes; */
  std::vector<db::_MeshBase*> meshes;
  std::vector<db::Body*> bodies;
  db::CameraPosition camera;

  int gl_point_size{1};
  int gl_line_width{1};
  int m_width, m_height;
  GLuint m_framebuffer_id;
  core::Vec4f m_bg = {0, 0, 0, 1};

#ifdef WITH_OPENGL
  GLFWwindow* wnd = nullptr;
#else
  void cleanupSwapChain();
  void recreateSwapChain();
  void beforeRender();
#endif

  /* public: */
  Render();
  ~Render() = default;
  void clearVBOs();
  void render();

  void point_size(int x) { gl_point_size = x; }
  void line_width(int x) { gl_line_width = x; }
  int point_size() const { return gl_point_size; }
  int line_width() const { return gl_line_width; }

  /* inline int getCmdNum() { return vertex_array.size(); } */
  void init();
  void draw();
  void terminate();
  void createSurface(GLFWwindow* window);
  void update_wndsize() {}
  void background_color(const core::Vec3b& c) {
    for(int i = 0; i < 3; i++) m_bg[i] = c[i];
  }
  void background_color(const core::Vec4b& c) {
    for(int i = 0; i < 4; i++) m_bg[i] = c[i];
  }

  [[deprecated]] void set_mesh(db::Mesh2D* m) { mesh_ui = m; }
  [[deprecated]] void set_mesh(db::Mesh* m) { mesh_3d = m; }
  [[deprecated]] void set_mesh(db::MeshCol* m) { mesh_col = m; }
  void add_mesh(db::Mesh* m);
  void add_mesh(db::MeshCol* m);
  void add_mesh(db::Mesh2D* m);
  void add_body(db::Body* b);

  void set_shader(db::Shader* s) { shader = s; }

  /// Objectがロードされた時、add_cmd()でドローコマンドを追加する
  void add_cmd(const db::_MeshBase* meshptr, int start_idx, int size);
  void set_tex_id(const int id) {
    if(id != _tex_id) push();
    _tex_id = id;
  }
  void set_z_index(const int z) {
    if(z != _z_index) push();
    _z_index = z;
  }
  void push() {
    DrawCmd d;
    d.start       = _last_end;
    d.size        = mesh_ui->get_vertices_size() - _last_end;
    d.tex_id      = _tex_id;
    d.scissor_btm = {0, 0};
    d.scissor_top = {0, 0};
    d.z_index     = _z_index;
    drawlist.push_back(d);
    _last_end = mesh_ui->get_vertices_size() - 1;
  }
  void clear_drawlist() {
    drawlist.clear();
    /* mesh_ui->clear(); */
  }
  auto sort() {
    std::sort(drawlist.begin(), drawlist.end(), [](const auto& x, const auto& y) { return x.z_index < y.z_index; });
  }
  auto get_n_vertices_ui() const {
    MU_ASSERT(mesh_ui != nullptr);
    return mesh_ui->get_vertices_size();
  }
  auto get_n_vertices() const {
    MU_ASSERT(mesh_col != nullptr);
    return mesh_col->get_vertices_size();
  }
  auto get_n_commands() const { return drawlist.size(); }
  void clear_not_default_mesh();
  void summary() const; /// prints drawlist commands summary


  cv::Mat get_depth_image();
  cv::Mat get_color_image();

  // ------------------------------------------------------------
  // -------- drawing commands (Add to MeshCol) -----------------
  // ------------------------------------------------------------

  void line(const core::Vec3& pos1, const core::Vec3& pos2, const core::Vec3b& col1, const core::Vec3b& col2, const float width = 1.0);
  void line(const core::Vec3& pos1, const core::Vec3& pos2, const core::Vec3b& col, const float width = 1.0);

  void point(const core::Vec3& pos, const core::Vec3b& col, const double size = 1.0);
  void point(const core::Vec3& pos, const core::Vec3b& col, const core::Vec3b& col2, const double size = 1.0);
  void sphere_20(const core::Vec3& pos, const float size, const core::Vec3b& col);
  void sphere_20(const core::Vec3& pos, const float size, const core::Vec3b& col, const float width);

  void arrow(const core::Vec3& pos1, const core::Vec3& pos2, const core::Vec3b& col, float width = 1.0);
  void arrow_to(const core::Vec3& pos, const core::Vec3& dir, const core::Vec3b& col, float width);

  void cube(const core::Vec3& pos, const core::Vec3& whd, const core::Vec3b& col);
  void cube(const core::Vec3& pos, const float size, const core::Vec3b& col);
  void cube(const core::Vec3& pos, const core::Vec3& size, const core::Vec3& pry, const core::Vec3b& col);

  void cube(const core::Vec3& pos, const core::Vec3& whd, const core::Vec3b& col, const float width);
  void cube(const core::Vec3& pos, const float size, const core::Vec3b& col, const float width);
  void cube(const core::Vec3& pos, const core::Vec3& size, const core::Vec3& pry, const core::Vec3b& col, const float width);
  void cube(const core::Rect3D& r, const core::Vec3b& col, const float width);

  void plane(const core::Vec3& pos, const float size, const core::Vec3& normal, const core::Vec3b& col);
  void plane(const core::Vec3& pos, const float size, const core::Vec3& normal, const core::Vec3b& col, float width);
  void plane(const core::Rect& r, const core::Vec3& normal, const core::Vec3b& col);

  void circle(const core::Vec3& pos, const core::Vec3& normal, const core::Vec3b& col);
  void circle(const core::Vec3& pos, const core::Vec3& normal, const core::Vec3b& col, const int width);

  void cone(const core::Vec3& pos, const core::Vec3& dir, float size, const core::Vec3b& col);
  void baloon(const std::string& str, const core::Vec3& pos, const float size, const core::Vec3b& col = {255, 255, 255}, const core::Vec3b& line = {255, 255, 255});
  void baloon(const std::string& str, const core::Vec2d& pos, const float size, const core::Vec3b& col = {255, 255, 255}, const core::Vec3b& line = {255, 255, 255});
  void gridxy(const core::Vec2 range, const int n = 10, const core::Vec3b col = {255, 255, 0});
  void cross(const core::Vec3& pos, const core::Vec3b& col, const double n, const double width = 0.1);
  void coord(const core::Vec3& pos, const core::Vec3& axis = {0, 0, 10}, const double n = 0.5);

  void triangle(const core::Vec3& pos1, const core::Vec3& pos2, const core::Vec3& pos3, const core::Vec3b& col);
  void triangle(const core::Vec3& pos1, const core::Vec3& pos2, const core::Vec3& pos3, const core::Vec3b& col, const float width);
  void triangle(const core::Vec3& pos1, const core::Vec3& pos2, const core::Vec3& pos3, const core::Vec3b& col1, const core::Vec3b& col2, const core::Vec3b& col3);
  void triangle(const core::Vec3& pos1, const core::Vec3& pos2, const core::Vec3& pos3, const core::Vec3b& col1, const core::Vec3b& col2, const core::Vec3b& col3, const float width);
  void quad(const core::Vec3& pos1, const core::Vec3& pos2, const core::Vec3& pos3, const core::Vec3& pos4, const core::Vec3b& col, const float width);
  void quad(const core::Vec3& pos1, const core::Vec3& pos2, const core::Vec3& pos3, const core::Vec3& pos4, const core::Vec3b& col);
  void quad(const core::Vec3&, const core::Vec3&, const core::Vec3&, const core::Vec3&, const core::Vec3b&, const core::Vec3b&, const core::Vec3b&, const core::Vec3b&, const float width);
  void quad(const core::Vec3&, const core::Vec3&, const core::Vec3&, const core::Vec3&, const core::Vec3b&, const core::Vec3b&, const core::Vec3b&, const core::Vec3b&);
  void rect(const core::Vec3& pos, const core::Vec2& size, const core::Vec3& normal, const core::Vec3b& col);
  void cylinder(const core::Vec3& p, const core::Vec3& n, const double r, const core::Vec3b& col);
  void cylinder(const core::Vec3& p, const core::Vec3& n, const double r, const core::Vec3b& col, const float width);

  // [[deprecated]] inline void addWidget2D(::uiWidget w){ root_widget_ui.AddWidget(&w); }
  void point_triangle(const core::Vec3& pos, const core::Vec3b& col, const double size = 1.0);
  void cube_rotated(const core::Vec3& pos, const core::Vec3& size, const core::Vec3& pry, const core::Vec3b& col);
  void cube_rotated(const core::Vec3& pos, const core::Vec3& size, const core::Vec3& pry, const core::Vec3b& col, const float width);

  core::Vec2d get_text_size(const std::string& str, float size) const;

  // ------------------------------------------------------------
  // -------- 2D drawing commands (Add to MeshCol) --------------
  // ------------------------------------------------------------

#define VKUI_USE_CLIPPING_RECT // TODO: 最終的にはこれなしで動くようにする
                               // TODO: stackにしてpop/pushするべきかも

  void triangle(const core::Vec2d& pos1, const core::Vec2d& pos2, const core::Vec2d& pos3, const core::Vec3b& col1, const core::Vec3b& col2, const core::Vec3b& col3, const core::Vec2d& uv1, const core::Vec2d& uv2, const core::Vec2d& uv3);
  void triangle(const core::Vec2d& pos1, const core::Vec2d& pos2, const core::Vec2d& pos3, const core::Vec2d uv1, const core::Vec2d& uv2, const core::Vec2d& uv3, const core::Vec3b& col);
  void triangle(const core::Vec2d& pos1, const core::Vec2d& pos2, const core::Vec2d& pos3, const core::Vec3b& col1, const core::Vec3b& col2, const core::Vec3b& col3);
  void triangle(const core::Vec2d& pos1, const core::Vec2d& pos2, const core::Vec2d& pos3, const core::Vec3b& col);
  void quad(const core::Vec2d& p1, const core::Vec2d& p2, const core::Vec2d& p3, const core::Vec2d& p4, const core::Vec3b& col);
  void quad(const core::Vec2d& pos1, const core::Vec2d& pos2, const core::Vec2d& pos3, const core::Vec2d& pos4, const core::Vec3b& col, const float width);
  void quad(const core::Vec2d&, const core::Vec2d&, const core::Vec2d&, const core::Vec2d&, const core::Vec3b&, const core::Vec3b&, const core::Vec3b&, const core::Vec3b&);
  void quad(const core::Vec2d& pos1, const core::Vec2d& pos2, const core::Vec2d& pos3, const core::Vec2d& pos4, const core::Vec2d& uv1, const core::Vec2d& uv2, const core::Vec2d& uv3, const core::Vec2d& uv4, const core::Vec3b& col);
  void rectTB(const core::Vec2d& top, const core::Vec2d& btm, const core::Vec3b& col);
  void rectTB(const core::Vec2d& top, const core::Vec2d& btm, const core::Vec3b& col, const float width);
  void rectPS(const core::Vec2d& pos, const core::Vec2d& size, const core::Vec3b& col);
  void rectPS(const core::Vec2d& p, const core::Vec2d& size, const core::Vec2d& ui1, const core::Vec2d& ui2, const core::Vec3b& col = {255, 255, 255});
  void rectPS(const core::Vec2d& p, const core::Vec2d& size, const core::Vec3b& col1, const core::Vec3b& col2, const core::Vec3b& col3, const core::Vec3b& col4);
  void rotated_rectPS(const core::Vec2d& pos, const core::Vec2d& size, const double theta, const core::Vec3b& col);
  void rotated_rectPS(const core::Vec2d& pos, const core::Vec2d& size, const double theta, const core::Vec3b& col, const float width = 2.0f);
  void rectPS(const core::Vec2d& pos, const core::Vec2d& size, const core::Vec3b& col, float width);
  void line(const core::Vec2d& pos1, const core::Vec2d& pos2, const core::Vec3b& col1, const core::Vec3b& col2, const float width = 1.0);
  void line(const core::Vec2d& pos1, const core::Vec2d& pos2, const core::Vec3b& col, const float width = 1.0);
  void arrow(const core::Vec2d& from, const core::Vec2d& to, const core::Vec3b& col, const float width = 1.0);
  void check(const core::Vec2d& pos, const int size, const core::Vec3b& col, float line_width);
  void arrow_down(const core::Vec2d& pos, const int size, const core::Vec3b& col);
  void arrow_up(const core::Vec2d& pos, const int size, const core::Vec3b& col);
  void arrow_left(const core::Vec2d& pos, const int size, const core::Vec3b& col);
  void arrow_right(const core::Vec2d& pos, const int size, const core::Vec3b& col);
  void arrow_right2(const core::Vec2d& pos, const int size, const core::Vec3b& col);
  void cross_button(const core::Vec2d& pos, const int size, const core::Vec3b& bg_col, const core::Vec3b& line_col, const core::Vec3b& cross_col);
  void cross(const core::Vec2d& center, const int size, const core::Vec3b& col);
  void plus(const core::Vec2d& center, const int size, const core::Vec3b& col);
  void diamond(const core::Vec2d& center, const int size, const core::Vec3b& col);
  void circle(const core::Vec2d& pos, const int size, const core::Vec3b col);
  void circle(const core::Vec2d& pos, const int size, const core::Vec3b col, const int width);
  core::Vec2d put_text(const std::string& str, const core::Vec2d& pos, const float size, const core::Vec3b& col = {255, 255, 255}, const int xlim = std::numeric_limits<int>::max());
};

} // namespace mu::render
