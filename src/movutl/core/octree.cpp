#include <movutl/core/octree.hpp>

namespace mu::core {
template <typename T, int DEPTH> bool Octree2<T, DEPTH>::insert(const core::_Vec<T, 3>& p, IndicesT idx) {
  MU_ASSERT(r.valid());
  MU_ASSERT(r.contains(p));
  MU_ASSERT(m_root != nullptr);

  Node** n       = &m_root;
  uint16_t depth = DEPTH;
  const auto bin = apply_bbox(p);

  while(depth) {
    if(*n == nullptr) {
      *n = new(&branch_pool) Branch();
      ++branch_count;
    } else {
      --depth;
      const unsigned size = (1 << depth);
      unsigned i          = ((bin[0] & size) ? 1 : 0) + ((bin[1] & size) ? 2 : 0) + ((bin[2] & size) ? 4 : 0);
      n                   = &reinterpret_cast<Branch*>(*n)->children[i];
    }
  }
  if(*n == nullptr) {
    MU_ASSERT(depth == 0);
    *n = new(&leaves_pool) Leave(idx);
    ++leaf_count;
  } else {
    reinterpret_cast<Leave*>(*n)->append(idx);
  }
  return true;
}
template <typename T, int DEPTH> void Octree2<T, DEPTH>::set_dataset(const core::Vec<_Vec<T, 3> >* d) {
  MU_ASSERT(d != nullptr);
  if(d->size() == 0) {
    spdlog::warn("octree input size is 0");
    return;
  }
  r = Rect3D();
  for(size_t i = 0; i < d->size(); i++) r.expand((*d)[i]);
  for(size_t i = 0; i < d->size(); i++) insert((*d)[i], i);
}

template <typename T, int DEPTH> void Octree2<T, DEPTH>::set_dataset(const db::MeshCol* m) {
  MU_ASSERT(m != nullptr);
  if(m->vertex.size() == 0) {
    spdlog::warn("octree input size is 0");
    return;
  }
  r = Rect3D();
  for(size_t i = 0; i < m->vertex.size(); i++) r.expand(_Vec<T, 3>(m->vertex[i].pos[0], m->vertex[i].pos[1], m->vertex[i].pos[2]));
  for(size_t i = 0; i < m->vertex.size(); i++) insert(_Vec<T, 3>(m->vertex[i].pos[0], m->vertex[i].pos[1], m->vertex[i].pos[2]), i);
}


template <typename T, int DEPTH> std::vector<IndicesT> Octree2<T, DEPTH>::findNearest(const _Vec<T, 3>& p, const SearchMethod& method, double radius) const {
  MU_ASSERT(r.valid());
  const auto bin = apply_bbox(p);

  if(root() == nullptr) return {};

  switch(method) {
    case SearchMethod::Nearest:

    {
      NearestSearch searcher(bin);
      searcher.bbox_min = Vec3d(r.x.min, r.y.min, r.z.min);
      searcher.bbox_max = Vec3d(r.x.max, r.y.max, r.z.max);
      searcher.bbox_min = bin - resolution() / 20.0;
      searcher.bbox_max = bin + resolution() / 20.0;
      searcher.search(root(), bin, resolution() / 2);

      std::vector<IndicesT> res;
      for(const auto& l : searcher.nn_leaves) {
        for(size_t i = 0; i < l->indices.size(); i++) res.push_back((*l)[i]);
      }
      return res;
    }

    case SearchMethod::Sphere:
    case SearchMethod::Volume:

    {
      const auto radius_bin = raw_to_bin_scale(radius);
      RadiusSearch searcher(bin, radius_bin);
      searcher.search(root(), {0, 0, 0}, resolution() / 2);
      std::vector<IndicesT> res;
      for(const auto& l : searcher.nn_leaves)
        for(size_t i = 0; i < l->indices.size(); i++) res.push_back((*l)[i]);
      return res;
    }

    case SearchMethod::NearestRecursive: {
      float r = radius;
      for(int i = 0; i < 10; i++) {
        const auto radius_bin = raw_to_bin_scale(r);
        RadiusSearch searcher(bin, radius_bin);
        searcher.search(root(), {0, 0, 0}, resolution() / 2);
        std::vector<IndicesT> res;
        for(const auto& l : searcher.nn_leaves)
          for(size_t i = 0; i < l->indices.size(); i++) res.push_back((*l)[i]);
        if(res.size() > 0) return res;
        r *= 3;
      }
      spdlog::warn("not found NearestRecursive radius(first) = {} , radius(last) = {}", radius, r);
      return {};
    }

    default: MU_ASSERT("Not defined SearchMethod!"); exit(1);
  }
  return {};
}

template <typename T, int DEPTH> void Octree2<T, DEPTH>::report() const {
  std::cout << "----------  octree report ---------------" << std::endl;
  std::cout << "depth, resolution = " << depth() << " , " << resolution() << std::endl;
  /* std::cout << "point size      = " << ((dataset != nullptr) ? dataset->size() : -1) << std::endl; */
  std::cout << "branch size       = " << branch_count << std::endl;
  std::cout << "leaf size         = " << leaf_count << std::endl;
  std::cout << "----------  octree report ---------------" << std::endl;
}

template <typename T, int DEPTH> bool Octree2<T, DEPTH>::hasNode(const core::_Vec<T, 3>& p) const {
  MU_ASSERT(r.valid());
  MU_ASSERT(r.contains(p));
  MU_ASSERT(root() != nullptr);
  const auto bin = apply_bbox(p);
  Branch* b      = root();

  for(int _dw = resolution() / 2; _dw > 1; _dw = _dw << 1) {
    auto idx   = !!(bin[0] & _dw) * 1 + !!(bin[1] & _dw) * 2 + !!(bin[2] & _dw) * 4;
    auto child = b->children[idx & 7];
    if(b == nullptr) return false;
    b = reinterpret_cast<Branch*>(child);
  }
  return true;
}

template class Octree2<double, 10>;
template class Octree2<float, 10>;
template class Octree2<int, 10>;
template class Octree2<short, 10>;
template class Octree2<size_t, 10>;

} // namespace mu::core
