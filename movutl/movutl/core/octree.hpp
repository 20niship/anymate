#pragma once
#include <cmath> //for sqrt
#include <limits>
#include <vector>

#include <movutl/core/assert.hpp>
#include <movutl/core/pool.hpp>
#include <movutl/core/rect.hpp>
#include <movutl/db/mesh.hpp>
#include <spdlog/spdlog.h>

namespace mu::core {

using IndicesT = size_t;

inline bool isPow2(unsigned i) { return ((i - 1) & i) == 0; }

enum class SearchMethod {
  Nearest = 1,
  Sphere,
  Volume,
  NearestRecursive
};

template <typename T = float, int DEPTH = 10> class Octree2 {
public:
  class Node {};

  struct Branch : public Node {
    Node* children[8];
    Branch() : children() {}
    void* operator new([[maybe_unused]] size_t num_bytes, Pool<Branch>* mem) {
      MU_ASSERT(sizeof(Branch) == num_bytes);
      return mem->alloc_item();
    }
    Node* operator[](unsigned i) const {
      MU_ASSERT(i < 8);
      return children[i];
    }
  };

  struct Leave : public Node {
    core::Vec<IndicesT> indices;
    inline Leave() { indices.clear(); }
    inline Leave(const IndicesT v) {
      indices.resize(1);
      indices[0] = v;
    }
    void* operator new([[maybe_unused]] size_t num_bytes, Pool<Leave>* mem) {
      MU_ASSERT(sizeof(Leave) == num_bytes);
      return mem->alloc_item();
    }
    inline IndicesT operator[](unsigned i) const { return indices[i]; }
    inline void append(const IndicesT v) { indices.push_back(v); }
  };

  typedef typename Pool<Leave>::iterator leaves_iterator;
  typedef typename Pool<Branch>::iterator branches_iterator;

public:
  Node* m_root;
  unsigned leaf_count;
  unsigned branch_count;

  Pool<Leave> leaves_pool;
  Pool<Branch> branch_pool;
  Rect3D r;

  Vec3d apply_bbox(const core::_Vec<T, 3>& p) const {
    Vec3d bin;
    bin[0] = (p[0] - r.x.min) * resolution() / r.x.length();
    bin[1] = (p[1] - r.y.min) * resolution() / r.y.length();
    bin[2] = (p[2] - r.z.min) * resolution() / r.z.length();
    return bin;
  }
  Vec3d raw_to_bin_scale(const _Vec<T, 3>& p) const { return Vec3d((double)p[0] * resolution() / r.x.length(), (double)p[1] * resolution() / r.y.length(), (double)p[2] * resolution() / r.z.length()); }
  Vec3d raw_to_bin_scale(const T& p) const { return raw_to_bin_scale({p, p, p}); }

  _Vec<T, 3> bin_to_raw(const Vec3d& p) const {
    _Vec<T, 3> raw;
    raw[0] = (float)p[0] * r.x.length() / resolution() + r.x.min;
    raw[1] = (float)p[1] * r.y.length() / resolution() + r.y.min;
    raw[2] = (float)p[2] * r.z.length() / resolution() + r.z.min;
    return raw;
  }

  class SearchBase {
  public:
    Vec3d target_pos;
    std::vector<const Leave*> nn_leaves;
    uint64_t nn_sq_distance;
    Vec3d bbox_min, bbox_max; // target_pos からnn_sq_distance + 1 離れた領域。これの中に点群が入っていればNearestになるかも
    uint64_t r_sq;            // 球体中の一覧を取得するときに、この半径**2いないの点群を取得する

    /// Check if any point of the position is in the search box.
    /// bbox_min,  bbox_maxで示された領域ないに一部が含まれるか？
    bool check_branch(const int x, const int y, const int z, const int w) const { return !(bbox_max[0] < x || bbox_min[0] > x + w || bbox_max[1] < y || bbox_min[1] > y + w || bbox_max[2] < z || bbox_min[2] > z + w); }

    bool check_branch_radius(const Vec3d& min, const int w, const Vec3d& r) const {
      const bool x_contains = Range(min[0] - r[0], min[0] + w + r[0]).contains(target_pos[0]);
      const bool y_contains = Range(min[1] - r[1], min[1] + w + r[1]).contains(target_pos[1]);
      const bool z_contains = Range(min[2] - r[2], min[2] + w + r[2]).contains(target_pos[2]);
      return x_contains && y_contains && z_contains;
    }

    uint8_t get_start_idx(const int size) const {
      const uint8_t res = !!(target_pos[0] & size) * 1 + !!(target_pos[1] & size) * 2 + !!(target_pos[2] & size) * 4;
      return res;
    }

    void add(const Leave* l) { nn_leaves.push_back(l); }

    SearchBase()  = default;
    ~SearchBase() = default;
  };

  // 探したい場所（ここから最も近い点を探す
  class NearestSearch : public SearchBase {
  public:
    NearestSearch() = delete;
    NearestSearch(const Vec3d& tar) {
      this->target_pos     = tar;
      this->nn_sq_distance = std::numeric_limits<uint64_t>::max();
    }

    // 指定されたLeaveが目的に合ったものかどうかを調べ、もし条件に適している場合はpush_backする
    void update_nearest(Leave* l, const Vec3d& p) {
      const auto d               = this->target_pos - p;
      const unsigned sq_distance = d.norm_sq();

      if(sq_distance < this->nn_sq_distance) {
        if(this->nn_leaves.size() == 0) this->nn_leaves.resize(1);
        this->nn_leaves[0]   = l;
        this->nn_sq_distance = sq_distance;

        const int r    = std::sqrt(sq_distance) + 1.0;
        this->bbox_min = this->target_pos - r;
        this->bbox_max = this->target_pos + r;
      }
    }

    void search(const Branch* b, Vec3d t, const unsigned size) {
      MU_ASSERT(b != nullptr);
      MU_ASSERT(isPow2(size));
      const uint8_t start_i = this->get_start_idx(size);
      for(uint8_t i = start_i; i < (start_i + 8); ++i) {
        Node* n = b->children[i & 7];
        if(n == nullptr) continue;

        const unsigned child_x = (i & 1) ? (t[0] | size) : t[0];
        const unsigned child_y = (i & 2) ? (t[1] | size) : t[1];
        const unsigned child_z = (i & 4) ? (t[2] | size) : t[2];
        const Vec3d branch_bbox_min_pos(child_x, child_y, child_z);
        if(size == 1) {
          update_nearest(reinterpret_cast<Leave*>(n), branch_bbox_min_pos);
        } else if(this->check_branch(child_x, child_y, child_z, size)) {
          search(reinterpret_cast<Branch*>(n), branch_bbox_min_pos, (size / 2));
        }
      }
    }
  };

  class RadiusSearch : public SearchBase {
  public:
    Vec3d radius;
    RadiusSearch() = delete;
    RadiusSearch(const Vec3d& tar, Vec3d radius_) {
      this->target_pos = tar;
      radius           = radius_;
    }

    void search(const Branch* b, Vec3d t, const unsigned size) {
      MU_ASSERT(b != nullptr);
      MU_ASSERT(isPow2(size));
      const uint8_t start_i = this->get_start_idx(size);
      for(uint8_t i = start_i; i < (start_i + 8); ++i) {
        Node* n = b->children[i & 7];
        if(n == nullptr) continue;
        const unsigned child_x = (i & 1) ? (t[0] | size) : t[0];
        const unsigned child_y = (i & 2) ? (t[1] | size) : t[1];
        const unsigned child_z = (i & 4) ? (t[2] | size) : t[2];
        const Vec3d branch_bbox_min_pos(child_x, child_y, child_z);
        if(size == 1) {
          this->add(reinterpret_cast<Leave*>(n));
        } else if(this->check_branch_radius(branch_bbox_min_pos, size, radius)) {
          search(reinterpret_cast<Branch*>(n), branch_bbox_min_pos, (size / 2));
        }
      }
    }
  };

  bool insert(const core::_Vec<T, 3>& p, IndicesT idx);

public:
  Octree2() {
    m_root       = new(&branch_pool) Branch();
    branch_count = 0;
    leaf_count   = 0;
    r            = Rect3D();
  }
  ~Octree2() {}

  inline unsigned resolution() const { return 1 << DEPTH; }
  inline unsigned depth() const { return DEPTH; }
  inline unsigned capacity() const {
    auto w = resolution();
    return w * w * w;
  }
  inline void setVolume(const Rect3D& r_) { r = r_; }
  inline Branch* root() const { return reinterpret_cast<Branch*>(m_root); }
  inline leaves_iterator leaf_begin() const { return leaves_pool.begin(); }
  inline leaves_iterator leaf_end() const { return leaves_pool.end(); }
  inline branches_iterator branch_begin() const { return branch_pool.begin(); }
  inline branches_iterator branch_end() const { return branch_pool.end(); }
  inline unsigned countLeave() const { return leaves_pool.size(); }
  inline unsigned countBranches() const { return branch_pool.size(); }
  inline Rect3D get_bbox() const { return r; }

  /// 背景削除に用いる
  /// 与えられた点(x, y, z)を含むBox(Depth＝MAX)が存在するか
  bool hasNode(const core::_Vec<T, 3>& p) const;

  void set_dataset(const core::Vec<_Vec<T, 3> >* d);
  void set_dataset(const db::MeshCol* m);

  std::vector<IndicesT> findNearest(const _Vec<T, 3>& p, const SearchMethod& method, double radius = 0.0) const;
  void report() const;
};

} // namespace mu::core
