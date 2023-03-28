#include <movutl/core/assert.hpp>
#include <movutl/tools/mesh/meshtools.hpp>
#include <movutl/instance/instance.hpp>

using namespace mu::core;
using namespace mu::db;
namespace mu::tools {

void clip_all_mesh(const mu::db::Body* in, mu::db::Body* out, const Rect3D& r) {
  MU_ASSERT(r.valid());
  MU_ASSERT(in != nullptr);
  MU_ASSERT(out != nullptr);
  MU_ASSERT(in->meshs.size() == 1);
  MU_ASSERT(in->meshs[0]->get_mesh_type() == _MeshBase::MeshType::Col);

  if(out->meshs.size() == 0) {
    const auto m = mu::instance::create_mesh_col();
    m->primitive_type(_MeshBase::PrimitiveType::POINTS);
    out->meshs.push_back(m);
  }

  MeshCol* mi = reinterpret_cast<MeshCol*>(in->meshs[0]);
  MeshCol* mo = reinterpret_cast<MeshCol*>(out->meshs[0]);

  if(mi->get_vertices_size() == 0) return;

  auto* ptr      = mi->vertex.begin();
  const auto end = mi->vertex.end();
  mo->vertex.clear();
  for(; ptr <= end; ptr++)
    if(r.contains(ptr->pos[0], ptr->pos[1], ptr->pos[2])) mo->vertex.push_back(*ptr);
}

} // namespace mu::tools
