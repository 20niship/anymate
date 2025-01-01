#pragma once

#include <movutl/core/defines.hpp>
#include <movutl/core/octree.hpp>
#include <movutl/db/mesh.hpp>

namespace mu::tools {

template <class MeshType, typename T> MU_API void colormap_by_normal(MeshType* min, db::MeshCol* mout, const core::Vec<core::_Vec<T, 3>>& normals) {
  MU_ASSERT(min != nullptr);
  MU_ASSERT(mout != nullptr);

  MU_ASSERT(min->vertex.size() == normals.size());
  MU_ASSERT(mout != nullptr);

  const auto vs = min->vertex.size();
  mout->vertex.resize(vs);
  mout->primitive_type(db::_MeshBase::PrimitiveType::POINTS);

  for(size_t i = 0; i < vs; i++) {
    mout->vertex[i].pos[0] = min->vertex[i].pos[0];
    mout->vertex[i].pos[1] = min->vertex[i].pos[1];
    mout->vertex[i].pos[2] = min->vertex[i].pos[2];

    const auto no      = normals[i];
    const auto v       = no - no.min();
    const double range = no.max() - no.min();
    const auto c       = v / range * 250;

    mout->vertex[i].col[0] = c[0];
    mout->vertex[i].col[1] = c[1];
    mout->vertex[i].col[2] = c[2];
  }
}

template MU_API void colormap_by_normal(db::Mesh* min, db::MeshCol* mout, const core::Vec<core::Vec3f>& normals);
template MU_API void colormap_by_normal(db::Mesh* min, db::MeshCol* mout, const core::Vec<core::Vec3>& normals);
template MU_API void colormap_by_normal(db::MeshCol* min, db::MeshCol* mout, const core::Vec<core::Vec3f>& normals);
template MU_API void colormap_by_normal(db::MeshCol* min, db::MeshCol* mout, const core::Vec<core::Vec3>& normals);

} // namespace mu::tools
