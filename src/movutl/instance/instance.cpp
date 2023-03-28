#include <movutl/instance/instance.hpp>

namespace mu::instance {

muContext ctx;

MU_API muContext* get_mu_context() { return &ctx; }
MU_API db::Image* get_font_texture() {
  MU_ASSERT(ctx.engine.init_finished);
  MU_ASSERT(ctx.engine.textures.size() >= ctx.engine.font_texture_idx);
  return &ctx.engine.textures[ctx.engine.font_texture_idx];
}

MU_API db::Image* get_text_texture() {
  auto e = get_engine();
  MU_ASSERT(e->textures.size() > e->font_texture_idx);
  return &e->textures[0];
  return &e->textures[e->font_texture_idx];
}

MU_API render::uiFont* get_text_renderer() { return &get_mu_context()->engine.text_renderer; };
MU_API core::Vec2d get_white_pixel_uv() { return get_mu_context()->engine.text_renderer.TexUvWhitePixel; }
MU_API void init() {get_mu_context()->engine.init(); }

MU_API db::Mesh2D* create_mesh_2d() {
  auto ptr = new db::Mesh2D();
  ctx.proj.meshes.push_back(ptr);
  return ptr;
}

MU_API db::MeshCol* create_mesh_col() {
  auto ptr = new db::MeshCol();
  ctx.proj.meshes.push_back(ptr);
  return ptr;
}

MU_API db::Mesh* create_mesh_3d() {
  auto ptr = new db::Mesh();
  ctx.proj.meshes.push_back(ptr);
  return ptr;
}

MU_API db::Image* create_texture(const char* filename) {
  ctx.engine.textures.push_back(db::Image(filename));
  auto e = ctx.engine.textures.end();
  e->set_to_gpu();
  return e;
}

MU_API render::Engine* get_engine() { return &ctx.engine; }

MU_API void terminate() {
  ctx.engine.terminate();
  ctx.editor.terminate();
}

MU_API db::Shader* get_ui_shader() {
  MU_ASSERT(ctx.engine.shaders.size() >= 3);
  return &(ctx.engine.shaders[0]);
}

MU_API db::Shader* get_col_shader() {
  MU_ASSERT(ctx.engine.shaders.size() >= 3);
  return &(ctx.engine.shaders[2]);
}

MU_API db::Shader* get_3d_shader() {
  MU_ASSERT(ctx.engine.shaders.size() >= 3);
  return &(ctx.engine.shaders[1]);
}

MU_API ui::uiStyle* get_style() { return &ctx.editor.style; }
MU_API bool engine_error_check() { return ctx.engine.error_check(); }
MU_API ui::Editor* get_editor() { return &ctx.editor; }
MU_API ui::EditorIO* get_io() { return &ctx.editor_io; }

MU_API db::Body* create_body() {
  ctx.proj.bodies.push_back(db::Body());
  auto e = ctx.proj.bodies.end();
  return e;
}

MU_API ui::uiWindow* get_window(GLFWwindow* glfw_window) {
  if(ctx.editor.windows.size() == 0) return nullptr;
  for(const auto& w : ctx.editor.windows) {
    if(w->getGLFWwindow() == glfw_window) return w;
  }
  return nullptr;
}

} // namespace mu::instance
