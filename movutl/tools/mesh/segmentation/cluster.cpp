#include <movutl/tools/colormap.hpp>
#include <movutl/tools/mesh/normal_estimation.hpp>
#include <movutl/tools/mesh/segmentation/cluster.hpp>
#include <movutl/ui/colors.hpp>

namespace mu::Magi {

using namespace mu::core;
using namespace mu::db;
void ClusterManagerBase::get_colored_output(db::MeshCol* m) const {
  MU_ASSERT(m != nullptr);
  if(m != m_mesh) {
    m->vertex.resize(m_mesh->get_vertices_size());
    m->primitive_type(m_mesh->primitive_type());
    for(size_t i = 0; i < m_mesh->get_vertices_size(); i++) {
      m->vertex[i].pos[0] = m_mesh->vertex[i].pos[0];
      m->vertex[i].pos[1] = m_mesh->vertex[i].pos[1];
      m->vertex[i].pos[2] = m_mesh->vertex[i].pos[2];
    }
  }
  for(size_t i = 0; i < m_clusters.size(); i++) {
    const auto indices = m_clusters[i].indices;
    const auto col     = colors::colormap[i % colors::colormap.size()];
    for(size_t k = 0; k < indices.size(); k++) {
      m->vertex[indices[k]].col[0] = col[0];
      m->vertex[indices[k]].col[1] = col[1];
      m->vertex[indices[k]].col[2] = col[2];
    }
  }
}

int ClusterManagerBase::get_cluster_index(const db::IndicesT idx) const {
  for(size_t i = 0; i < m_clusters.size(); i++) {
    const auto v = m_clusters[i].indices;
    if(std::find(v.begin(), v.end(), idx) != v.end()) return i;
  }
  return -1;
}

std::vector<db::IndicesT> ClusterManagerBase::get_indices_of_cluster_idx(const int cluster_idx) const {
  if(cluster_idx < 0) return {};
  MU_ASSERT(cluster_idx < m_clusters.size());
  std::vector<db::IndicesT> res;
  for(const auto& p : m_clusters[cluster_idx].indices) res.push_back(p);
  return res;
}

} // namespace mu::Magi
