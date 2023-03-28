#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cwchar>
#include <filesystem>

#include <movutl/db/body.hpp>
#include <movutl/instance/instance.hpp>
#include <movutl/io/object_loader.hpp>

namespace mu::db {

void Body::move_geom(const Vec3& pos) {
  for(auto&& m : meshs) m->move_geom(pos);
}
void Body::scale_geom(const Vec3& scale) {
  for(auto&& m : meshs) m->scale_geom(scale);
}
void Body::rot_geom(const Vec3& rot) {
  for(auto&& m : meshs) m->rot_geom(rot);
}
void Body::set_geom_to_origin() {
  const auto center = get_center_of_geom();
  move_geom(center * -1);
}
void Body::set_origin_to_geom() {
  const auto center = get_center_of_geom();
  move_geom(pos - center);
  pos = {(float)center[0], (float)center[1], (float)center[2]};
}

void Body::apply_scale() {
  for(auto&& m : meshs) m->scale_geom(scale);
  scale = {1, 1, 1};
}
void Body::apply_position() {
  move_geom(pos);
  pos= {0, 0, 0};
}
void Body::apply_rotation() {
  rot_geom(pos);
  rotation = {0, 0, 0};
}
void Body::apply_all_transform() {
  apply_scale();
  apply_rotation();
  apply_position();
}
void Body::clear_position() { pos = {0, 0, 0}; }
void Body::clear_rotation() { rotation = {0, 0, 0}; }
void Body::clear_scale() { scale = {1, 1, 1}; }

core::Rect3D Body::get_bbox() const {
  Rect3D r;
  for(auto m : meshs) r.merge(m->get_bbox());
  return r;
}
core::Vec3 Body::get_center_of_geom() const {
  Vec3 c(0, 0, 0);
  int n = 0;
  for(auto m : meshs) {
    const auto nv = m->get_vertices_size();
    c += m->get_center_of_geom() * nv;
    n += nv;
  }
  return c / (float)n;
}

/* void Body::draw() { */
  /* Shader* shader_use; */
  /* if(shader) */
  /*   shader_use = shader; */
  /* else */
  /*   LOGE << "ここのshader == nullptrのときにshader_useもnullptrになってセグフォルので注意"; */
  /* for(auto&& m : meshs) m->draw(shader_use); */
/* } */

core::Mat4x4f Body::get_model_transform_matrix() const {
  const auto m = core::transform_mat(pos, rotation, scale);
  return Mat4x4f(m);
}

} // namespace mu::db
