#pragma once 
#include <iostream>
#include <movutl/db/mesh.hpp>
#include <movutl/tools/magi/fitting.hpp>
#include <movutl/core/vector.hpp>
#include <vector>

namespace mu::Magi{

void convex_hull(const db::MeshCol *min, db::Mesh *mout);
template<typename T>
std::vector<size_t> convex_hull(const std::vector<core::_Vec<T, 2>> &);


} //namespace mu::Magi
