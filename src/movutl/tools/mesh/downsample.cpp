#include <movutl/tools/mesh/meshtools.hpp>
#include <movutl/instance/instance.hpp>

using namespace mu::core;
using namespace mu::db;
namespace mu::tools {

void downsample_all_mesh(const mu::db::Body* in, mu::db::Body* out, int n) {
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
  mo->vertex.clear();

  MeshCol::Vertex* ptr       = mi->vertex.begin();
  const MeshCol::Vertex* end = mi->vertex.end();
  for(; ptr < end; ptr += n) {
    mo->vertex.push_back(*ptr);
  }
}

} // namespace mu::tools
