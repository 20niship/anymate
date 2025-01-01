#pragma once
#include "spdlog/fmt/bundled/format.h"
#include <movutl/db/body.hpp>
#include <movutl/db/mesh.hpp>

namespace mu::tools {

void copy_body(mu::db::Body* in, mu::db::Body* out);
void copy_mesh(const db::_MeshBase* in, db::_MeshBase* out);
void copy_mesh_col(const db::MeshCol* in, db::MeshCol* out);
void copy_mesh_3d(const db::Mesh* in, db::Mesh* out);
void downsample_all_mesh(const mu::db::Body* in, mu::db::Body* out, int n);
void clip_all_mesh(const mu::db::Body* in, mu::db::Body* out, const core::Rect3D&);

} // namespace mu::tools
