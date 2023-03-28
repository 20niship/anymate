#include <movutl/db/db.hpp>
#include <movutl/db/image.hpp>
#include <movutl/db/project.hpp>
#include <movutl/render/engine.hpp>
#include <movutl/ui/io.hpp>
#include <movutl/ui/ui.hpp>

namespace mu::instance {

struct muContext {
  ui::Editor editor;
  ui::EditorIO editor_io;
  render::Engine engine;
  db::Project proj;
};

#ifndef MU_API
#define MU_API
#endif

MU_API muContext* get_mu_context();
MU_API render::uiFont* get_text_renderer();
MU_API db::Image* get_text_texture();
MU_API core::Vec2d get_white_pixel_uv();
MU_API db::Mesh2D* create_mesh_2d();
MU_API db::MeshCol* create_mesh_col();
MU_API db::Mesh* create_mesh_3d();

MU_API void add_mesh_sphere(db::Mesh* m, const core::Vec3& pos, float r, const int stack_count=20, const int sector_count=20);

MU_API db::Body* create_body();
MU_API db::Image* create_texture(const char* filename);
MU_API void init();
MU_API void terminate();
MU_API render::Engine* get_engine();
MU_API db::Shader* get_ui_shader();
MU_API db::Shader* get_col_shader();
MU_API db::Shader* get_3d_shader();

MU_API ui::Editor* get_editor();
MU_API ui::EditorIO* get_io();
MU_API ui::uiWindow* get_window(GLFWwindow* glfw_window);

MU_API ui::uiStyle* get_style();
MU_API bool engine_error_check();

} // namespace mu::instance
