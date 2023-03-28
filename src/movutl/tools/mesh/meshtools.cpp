#include <movutl/tools/mesh/meshtools.hpp>
#include <spdlog/spdlog.h>
#include <movutl/core/logger.hpp>

using namespace mu::core;
using namespace mu::db;
namespace mu::tools {

void copy_body(mu::db::Body* in, mu::db::Body* out) {
  MU_ASSERT(in != nullptr);
  MU_ASSERT(out != nullptr);
  in->meshs    = out->meshs;
  in->textures = out->textures;
  in->pos      = out->pos;
  in->bones    = out->bones;
}

void copy_mesh_col(const db::MeshCol* in, db::MeshCol* out) {
  MU_ASSERT(in != nullptr);
  MU_ASSERT(out != nullptr);
  out->vertex  = in->vertex;
  out->indices = in->indices;
  out->textures = in->textures;
  out->primitive_type(in->primitive_type());
}

void copy_mesh_3d(const db::Mesh* in, db::Mesh* out){
  MU_ASSERT(in != nullptr);
  MU_ASSERT(out != nullptr);
  out->vertex  = in->vertex;
  out->indices = in->indices;
  out->textures = in->textures;
  out->primitive_type(in->primitive_type());
}

void copy_mesh(const mu::db::_MeshBase* in, mu::db::_MeshBase* out) {
  MU_ASSERT(in != nullptr);
  MU_ASSERT(out != nullptr);
  out->indices = in->indices;
  out->textures = in->textures;
  out->primitive_type(in->primitive_type());
  spdlog::critical("not implemented copy_mesh");
  LOGE << "not implemented copy_mesh";
}

} // namespace mu::tools
