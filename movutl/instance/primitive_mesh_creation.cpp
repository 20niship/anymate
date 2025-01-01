#include <movutl/instance/instance.hpp>

#ifndef M_PI
#define M_PI 3.1415926535
#endif

namespace mu::instance {

MU_API void add_mesh_sphere(db::Mesh* m, const core::Vec3& pos, float r, const int stack_count, const int sector_count) {
  std::vector<float> vertices, normals;
  std::vector<uint32_t> indices;
  MU_ASSERT(m != nullptr);
  m->primitive_type(db::_MeshBase::PrimitiveType::TRIANGLES); // idx_offsetがおかしくなるため
  const auto idx_offset  = vertices.size() / 3;
  const float sectorStep = 2 * M_PI / sector_count;
  const float stackStep  = M_PI / stack_count;
  const float lengthInv  = 1.0f / r; // vertex normal

  for(int i = 0; i <= stack_count; ++i) {
    const float stackAngle = M_PI / 2 - i * stackStep; // starting from pi/2 to -pi/2
    const float xy         = r * std::cos(stackAngle); // r * cos(u)
    const float z          = r * std::sin(stackAngle); // r * sin(u)
    for(int j = 0; j <= sector_count; ++j) {
      const float angle = j * sectorStep;       // starting from 0 to 2pi
      const float x     = xy * std::cos(angle); // r * cos(u) * cos(v)
      const float y     = xy * std::sin(angle); // r * cos(u) * sin(v)
      const float nx    = x * lengthInv;
      const float ny    = y * lengthInv;
      const float nz    = z * lengthInv;
      db::Mesh::Vertex vert;
      vert.pos[0]  = x + pos[0];
      vert.pos[1]  = y + pos[1];
      vert.pos[2]  = z + pos[2];
      vert.norm[0] = nx;
      vert.norm[1] = ny;
      vert.norm[2] = nz;
      m->vertex.push_back(vert);
    }
  }

  for(int i = 0; i < stack_count; ++i) {
    int k1 = idx_offset + i * (sector_count + 1); // beginning of current stack
    int k2 = k1 + sector_count + 1;               // beginning of next stack
    for(int j = 0; j < sector_count; ++j, ++k1, ++k2) {
      if(i != 0) {
        m->indices.push_back(k1);
        m->indices.push_back(k2);
        m->indices.push_back(k1 + 1);
      }
      if(i != (stack_count - 1)) {
        m->indices.push_back(k1 + 1);
        m->indices.push_back(k2);
        m->indices.push_back(k2 + 1);
      }
    }
  }
}

#if 0
// TODO: primitiveな図形をBodyに追加する関数
MU_API void add_mesh_cube(const db::Mesh* m, const core::Vec3& pos, const core::Vec3& size) {
  LOGE << "NOT_IMPLEMENTED!";
  exit(1);
}
MU_API void add_mesh_suzanne(const db::Mesh* m, const core::Vec3& pos, const core::Vec3& size) {
  LOGE << "NOT_IMPLEMENTED!";
  exit(1);
}
MU_API void add_mesh_points(const db::Mesh* m, const core::Vec3& pos, const core::Vec3& size) {
  LOGE << "NOT_IMPLEMENTED!";
  exit(1);
}
#endif

} // namespace mu::instance
