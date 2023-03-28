#include <movutl/core/octree.hpp>
#include <movutl/render/render.hpp>

namespace mu::tools {

template <typename T, int DEPTH> void display_octree(render::Render*, const mu::core::Octree2<T, DEPTH>*);

template<> void display_octree(render::Render*, const mu::core::Octree2<float, 10>*);
template<> void display_octree(render::Render*, const mu::core::Octree2<double, 10>*);


} // namespace mu::tools
