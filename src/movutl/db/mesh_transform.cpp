#include <movutl/db/mesh.hpp>

namespace mu::db {

using namespace core;

// clang-format off

#define IMPLEMENT_MESH_TRANSFORM_FUNCTIONS(classname) \
Vec3 classname::get_center_of_geom() const { \
  if(vertex.size() == 0)return {0, 0, 0}; \
  Vec3 p(0, 0, 0); \
  const Vertex* end = vertex.end(); \
  for(auto* ptr = vertex.begin(); ptr < end; ptr++) { \
    p[0] += ptr->pos[0];   p[1] += ptr->pos[1];  p[2] += ptr->pos[2]; \
  } \
  return p / vertex.size(); \
} \
core::Rect3D classname::get_bbox() const { \
  Rect3D r; \
  const Vertex* end = vertex.end(); \
  for(auto* ptr = vertex.begin(); ptr <= end; ptr++) { \
    r.expand(ptr->pos[0], ptr->pos[1], ptr->pos[2]); \
  } \
  return r; \
} \
void classname::move_geom(const core::Vec3& dp) { \
  const Vertex* end = vertex.end(); \
  for(auto* ptr = vertex.begin(); ptr <= end; ptr++) { \
    ptr->pos[0] += dp[0];    ptr->pos[1] += dp[1];    ptr->pos[2] += dp[2]; \
  } \
} \
void classname::scale_geom(const core::Vec3& dp) { \
  const Vertex* end = vertex.end(); \
  for(auto* ptr = vertex.begin(); ptr <= end; ptr++) { \
    ptr->pos[0] *= dp[0];    ptr->pos[1] *= dp[1];    ptr->pos[2] *= dp[2]; \
  } \
} \
void classname::rot_geom(const core::Vec3& rot) { \
  const float sin = std::sin(rot[2]); \
  const float cos = std::cos(rot[2]); \
  const auto* end = vertex.end(); \
  for(auto* ptr = vertex.begin(); ptr <= end; ptr++) { \
    const auto x = ptr->pos[0]; \
    const auto y = ptr->pos[1]; \
    ptr->pos[0]  = cos * x - sin * y; \
    ptr->pos[1]  = sin * x + cos * y; \
  } \
}


// clang-format on

IMPLEMENT_MESH_TRANSFORM_FUNCTIONS(Mesh);
IMPLEMENT_MESH_TRANSFORM_FUNCTIONS(MeshCol);

Vec3 Mesh2D::get_center_of_geom() const {
  Vec3 p;
  const Vertex* end = vertex.end();
  for(auto* ptr = vertex.begin(); ptr <= end; ptr++) {
    p[0] += ptr->pos[0];
    p[1] += ptr->pos[1];
  }
  return p / vertex.size();
}

core::Rect3D Mesh2D::get_bbox() const {
  Rect3D r;
  const Vertex* end = vertex.end();
  for(auto* ptr = vertex.begin(); ptr <= end; ptr++) {
    r.expand(ptr->pos[0], ptr->pos[1], 0);
  }
  return r;
}
void Mesh2D::move_geom(const core::Vec3& dp) {
  const Vertex* end = vertex.end();
  for(auto* ptr = vertex.begin(); ptr <= end; ptr++) {
    ptr->pos[0] += dp[0];
    ptr->pos[1] += dp[1];
  }
}
void Mesh2D::scale_geom(const core::Vec3& dp) {
  const Vertex* end = vertex.end();
  for(auto* ptr = vertex.begin(); ptr <= end; ptr++) {
    ptr->pos[0] *= dp[0];
    ptr->pos[1] *= dp[1];
  }
}
void Mesh2D::rot_geom(const core::Vec3& rot) {
  const float sin = std::sin(rot[2]);
  const float cos = std::cos(rot[2]);
  const auto* end = vertex.end();
  for(auto* ptr = vertex.begin(); ptr <= end; ptr++) {
    const auto x = ptr->pos[0];
    const auto y = ptr->pos[1];
    ptr->pos[0]  = cos * x - sin * y;
    ptr->pos[1]  = sin * x + cos * y;
  }
}

} // namespace mu::db
