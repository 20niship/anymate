#include <movutl/db/mesh.hpp>
#include <movutl/instance/instance.hpp>

namespace mu::db {

_MeshBase::~_MeshBase() {
  glDeleteBuffers(1, &indices_vbo_id);
  glDeleteBuffers(1, &vertex_vbo_id);
  glDeleteVertexArrays(1, &vao);
}

GLenum _MeshBase::get_gl_primitive_type() {
  switch(type) {
    case PrimitiveType::TRIANGLES: return GL_TRIANGLES;
    case PrimitiveType::TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
    case PrimitiveType::POINTS: return GL_POINTS;
    case PrimitiveType::LINES: return GL_LINES;
    case PrimitiveType::LINE_STRIP: return GL_LINE_STRIP;
  }
  return GL_TRIANGLES;
}

void Mesh2D::bind() { glBindVertexArray(vao); }
void Mesh::bind() { glBindVertexArray(vao); }
void MeshCol::bind() { glBindVertexArray(vao); }

void Mesh2D::set_attribute(int pos_loc, int col_loc, int uv_loc) {
  MU_ASSERT(pos_loc >= 0);
  MU_ASSERT(col_loc >= 0);
  MU_ASSERT(uv_loc >= 0);

  glEnableVertexAttribArray(pos_loc);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo_id);
  glVertexAttribPointer(pos_loc, 2, GL_SHORT, GL_FALSE, db::Mesh2D::vertex_byte(), (GLvoid*)db::Mesh2D::offset_pos());

  glEnableVertexAttribArray(col_loc);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo_id);
  glVertexAttribPointer(col_loc, 3, GL_UNSIGNED_BYTE, GL_FALSE, db::Mesh2D::vertex_byte(), (GLvoid*)db::Mesh2D::offset_col());

  glEnableVertexAttribArray(uv_loc);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo_id);
  glVertexAttribPointer(uv_loc, 2, GL_UNSIGNED_SHORT, GL_FALSE, db::Mesh2D::vertex_byte(), (GLvoid*)db::Mesh2D::offset_uv());
}

void MeshCol::set_attribute(int pos_loc, int col_loc, int uv_loc) {
  MU_ASSERT(pos_loc >= 0);
  /* MU_ASSERT(col_loc >= 0); */
  /* MU_ASSERT(uv_loc >= 0); */

  glEnableVertexAttribArray(pos_loc);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo_id);
  glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, db::MeshCol::vertex_byte(), (GLvoid*)db::MeshCol::offset_pos());

  if(col_loc >= 0) {
    glEnableVertexAttribArray(col_loc);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo_id);
    glVertexAttribPointer(col_loc, 3, GL_UNSIGNED_BYTE, GL_FALSE, db::MeshCol::vertex_byte(), (GLvoid*)db::MeshCol::offset_col());
  }else{
    spdlog::warn("mesh color not used by shader");
  }
  if(uv_loc >= 0) {
    glEnableVertexAttribArray(uv_loc);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo_id);
    glVertexAttribPointer(uv_loc, 2, GL_FLOAT, GL_FALSE, db::MeshCol::vertex_byte(), (GLvoid*)db::MeshCol::offset_uv());
  }else{
    spdlog::warn("mesh uv not used by shader");
  }
}


void Mesh::set_attribute(int pos_loc, int norm_loc, int uv_loc, [[maybe_unused]]int tangent_loc, [[maybe_unused]]int bone_loc, [[maybe_unused]]int weight_loc) {
  glEnableVertexAttribArray(pos_loc);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo_id);
  glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, db::Mesh::vertex_byte(), (GLvoid*)db::Mesh::offset_pos());

  glEnableVertexAttribArray(norm_loc);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo_id);
  glVertexAttribPointer(norm_loc, 3, GL_FLOAT, GL_FALSE, db::Mesh::vertex_byte(), (GLvoid*)db::Mesh::offset_norm());

  glEnableVertexAttribArray(uv_loc);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo_id);
  glVertexAttribPointer(uv_loc, 2, GL_FLOAT, GL_FALSE, db::Mesh::vertex_byte(), (GLvoid*)db::Mesh::offset_uv());
}

// ----------------  Buffer Creation ------------- //

void Mesh2D::create_buffer() {
  if(_gpu_buf_created) return;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vertex_vbo_id);
  glGenBuffers(1, &indices_vbo_id);
  _gpu_buf_created = true;
}

void Mesh::create_buffer() {
  if(_gpu_buf_created) return;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vertex_vbo_id);
  glGenBuffers(1, &indices_vbo_id);
  _gpu_buf_created = true;
}

void MeshCol::create_buffer() {
  if(_gpu_buf_created) return;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vertex_vbo_id);
  glGenBuffers(1, &indices_vbo_id);
  _gpu_buf_created = true;
}

void Mesh::set_to_gpu() {
  glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo_id);
  glBufferData(GL_ARRAY_BUFFER, vertex.size_in_bytes(), (const GLvoid*)vertex.Data, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_in_bytes(), (const GLvoid*)indices.Data, GL_STATIC_DRAW);
  _gpu_buf_updated = true;
}

void Mesh2D::set_to_gpu() {
  glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo_id);
  glBufferData(GL_ARRAY_BUFFER, vertex.size_in_bytes(), (const GLvoid*)vertex.Data, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_in_bytes(), (const GLvoid*)indices.Data, GL_STATIC_DRAW);
  _gpu_buf_updated = true;
}

void MeshCol::set_to_gpu() {
  glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo_id);
  glBufferData(GL_ARRAY_BUFFER, vertex.size_in_bytes(), (const GLvoid*)vertex.Data, GL_STATIC_DRAW);
  _gpu_buf_updated = true;
}


Mesh::Mesh() { create_buffer(); }
MeshCol::MeshCol() { create_buffer(); }
Mesh2D::Mesh2D() { create_buffer(); }

Mesh::~Mesh() {
  glDeleteBuffers(1, &vao);
  glDeleteBuffers(1, &indices_vbo_id);
  glDeleteBuffers(1, &vertex_vbo_id);
}

Mesh2D::~Mesh2D() {
  glDeleteBuffers(1, &vao);
  glDeleteBuffers(1, &indices_vbo_id);
  glDeleteBuffers(1, &vertex_vbo_id);
}

MeshCol::~MeshCol() {
  glDeleteBuffers(1, &vao);
  glDeleteBuffers(1, &indices_vbo_id);
  glDeleteBuffers(1, &vertex_vbo_id);
}

Mesh::Mesh(const core::Vec<Vertex>& vertices, const core::Vec<IndicesT>& indices) {
  this->vertex  = vertices;
  this->indices = indices;
  create_buffer();
}

Mesh2D::Mesh2D(const core::Vec<Vertex>& vertices, const core::Vec<IndicesT>& indices) {
  this->vertex  = vertices;
  this->indices = indices;
  create_buffer();
}

MeshCol::MeshCol(const core::Vec<Vertex>& vertices, const core::Vec<IndicesT>& indices) {
  this->vertex  = vertices;
  this->indices = indices;
  create_buffer();
}


void Mesh2D::draw(Shader* s) {
  if(s == nullptr) s = instance::get_ui_shader();
  MU_ASSERT(s != nullptr);
  MU_ASSERT(s->enabled());

  glBindVertexArray(vao);
  set_to_gpu();
  auto tx = instance::get_text_texture();
  s->set_value_img("mysampler", tx->get_tex_id(), 0);

  const auto pos_loc = s->get_attrib_loc("position");
  const auto uv_loc  = s->get_attrib_loc("vuv");
  const auto col_loc = s->get_attrib_loc("color");
  MU_ASSERT(pos_loc >= 0);
  MU_ASSERT(uv_loc >= 0);
  MU_ASSERT(col_loc >= 0);
  MU_ASSERT(gpu_buf_created());

  set_attribute(pos_loc, col_loc, uv_loc);
  glBindTexture(GL_TEXTURE_2D, tx->get_tex_id());

  glDrawArrays(get_gl_primitive_type(), 0, vertex.size());
  glBindVertexArray(0);
}

void MeshCol::draw(Shader* s) {
  if(s == nullptr) s = instance::get_col_shader();
  MU_ASSERT(s != nullptr);
  MU_ASSERT(s->enabled());

  glBindVertexArray(vao);
  set_to_gpu();
  auto tx = instance::get_text_texture();
  s->set_value_img("mysampler", tx->get_tex_id(), 0);

  const auto pos_loc = s->get_attrib_loc("position");
  const auto uv_loc  = s->get_attrib_loc("vuv");
  const auto col_loc = s->get_attrib_loc("color");
  MU_ASSERT(pos_loc >= 0);
  MU_ASSERT(uv_loc >= 0);
  MU_ASSERT(col_loc >= 0);
  MU_ASSERT(gpu_buf_created());

  set_attribute(pos_loc, col_loc, uv_loc);
  glBindTexture(GL_TEXTURE_2D, tx->get_tex_id());
  glDrawArrays(get_gl_primitive_type(), 0, vertex.size());
  glBindVertexArray(0);
}

void Mesh::draw(Shader* s) {
  if(s == nullptr) s = instance::get_3d_shader();
  MU_ASSERT(s != nullptr);
  MU_ASSERT(s->enabled());
  glBindVertexArray(vao);
  set_to_gpu();
  auto tx = instance::get_text_texture();
  s->set_value_img("mysampler", tx->get_tex_id(), 0);

  const auto pos_loc  = s->get_attrib_loc("position");
  const auto norm_loc = s->get_attrib_loc("norm");
  const auto uv_loc   = s->get_attrib_loc("vuv");
  MU_ASSERT(pos_loc >= 0);
  MU_ASSERT(uv_loc >= 0);
  MU_ASSERT(norm_loc >= 0);
  MU_ASSERT(gpu_buf_created());
  set_attribute(pos_loc, norm_loc, uv_loc, -1, -1, -1);
  glBindTexture(GL_TEXTURE_2D, tx->get_tex_id());
  if(indices.size() > 0)
    glDrawElements(get_gl_primitive_type(), indices.size(), GL_UNSIGNED_SHORT, (void*)0);
  else
    glDrawArrays(get_gl_primitive_type(), 0, vertex.size());
  glBindVertexArray(0);
}

} // namespace mu::db
