#pragma once
#include <movutl/core/rect.hpp>
#include <movutl/core/vector.hpp>
#include <movutl/db/block.hpp>
#include <movutl/db/image.hpp>
#include <movutl/db/shader.hpp>
#include <istream>

namespace mu::render {
class Render;
}

namespace mu::db {

using IndicesT = uint32_t;

#define MAX_BONE_INFLUENCE 3

class _MeshBase {
  friend class render::Render;

public:
  enum class MeshType {
    Col,   /// それぞれの頂点に色がついている。MeshCol type
    UI,    /// UI用、二次元用 Mesh2D type
    Object /// Object用、Boneなどがついている。 Mesh type
  };

  enum class PrimitiveType {
    TRIANGLES,
    TRIANGLE_STRIP,
    LINES,
    LINE_STRIP,
    POINTS,
  };

protected:
  unsigned int vao;
  unsigned int vertex_vbo_id;
  unsigned int indices_vbo_id;

  /// primititive type (used in glDrawElements() first arg)
  PrimitiveType type;
  bool _gpu_buf_updated, _gpu_buf_created;
  virtual void bind()          = 0;
  virtual void set_to_gpu()    = 0;
  virtual void create_buffer() = 0;

  GLenum get_gl_primitive_type();

public:
  core::Vec<IndicesT> indices;
  core::Vec<Image*> textures;
  virtual void draw(Shader* shader)             = 0;
  virtual MeshType get_mesh_type()              = 0;
  virtual core::Vec3 get_center_of_geom() const = 0;
  virtual core::Rect3D get_bbox() const         = 0;
  virtual void move_geom(const core::Vec3&)     = 0; /// すべてのMeshについて 第1引数だけ移動する
  virtual void scale_geom(const core::Vec3&)    = 0; /// すべてのMeshについて 第1引数scale倍する
  virtual size_t get_vertices_size()            = 0;
  virtual void rot_geom(const core::Vec3&)      = 0; /// すべてのMeshについて 第1引数だけ回転する

  size_t get_indices_size() { return indices.size(); }
  bool gpu_buf_updated() { return _gpu_buf_updated; }
  bool gpu_buf_created() { return _gpu_buf_created; }
  auto primitive_type() const{ return type; }
  void primitive_type(const PrimitiveType& t) { type = t; }
  _MeshBase() {
    type = PrimitiveType::TRIANGLES;
    vao = vertex_vbo_id = indices_vbo_id = 0;
    _gpu_buf_updated = _gpu_buf_created = false;
  }
  ~_MeshBase();
};


/**
 * @class Meshクラス。3Dのオブジェクトデータを保存する。ボーン情報などが含まれる。
 * @detail
 */
class Mesh : public Block, public _MeshBase {
private:
  virtual void bind() override;
  void set_attribute(int pos_loc, int norm_loc, int uv_loc, int tangent_loc, int bone_loc, int weight_loc);
  virtual void set_to_gpu() override;
  virtual void create_buffer() override;

public:
  struct Vertex {
    float pos[3];
    float norm[3];
    float uv[2];
    float tangent[3];
    float bitangent[3];
    uint16_t bone[MAX_BONE_INFLUENCE];
    float weight[MAX_BONE_INFLUENCE];
  };
  core::Vec<Vertex> vertex;

  static auto offset_pos() { return offsetof(Vertex, pos); }
  static auto offset_norm() { return offsetof(Vertex, norm); }
  static auto offset_uv() { return offsetof(Vertex, uv); }
  static auto offset_tangent() { return offsetof(Vertex, tangent); }
  static auto offset_bitangent() { return offsetof(Vertex, bitangent); }
  static auto offset_bone() { return offsetof(Vertex, bone); }
  static auto offset_weight() { return offsetof(Vertex, weight); }
  static size_t vertex_byte() { return sizeof(Vertex); }

  Mesh();
  ~Mesh();
  Mesh(const core::Vec<Vertex>& vertices, const core::Vec<IndicesT>& indices);
  virtual void draw(Shader*) override;
  virtual MeshType get_mesh_type() override { return MeshType::Object; }
  virtual size_t get_vertices_size() override { return vertex.size(); }

  virtual core::Vec3 get_center_of_geom() const override;
  virtual core::Rect3D get_bbox() const override;
  virtual void move_geom(const core::Vec3&) override;  /// すべてのMeshについて 第1引数だけ移動する
  virtual void scale_geom(const core::Vec3&) override; /// すべてのMeshについて 第1引数scale倍する
  virtual void rot_geom(const core::Vec3&) override;   /// すべてのMeshについて 第1引数だけ回転する

  Mesh::Vertex* get_vertices_data() { return vertex.data(); }
  size_t get_vertices_size_in_bytes() { return vertex.size() * sizeof(Vertex); }
  void clear() {
    vertex.clear();
    indices.clear();
  }
};


// ------------------------------------------ //


/**
 * @class Meshクラス。色情報のみで点群データなどに使用される
 */
class MeshCol : public Block, public _MeshBase {
private:
  virtual void bind() override;
  virtual void set_to_gpu() override;
  virtual void create_buffer() override;
  void set_attribute(int pos_loc, int col_loc, int uv_loc);

public:
  struct Vertex {
    float pos[3];
    uint8_t col[3];
    float uv[2];
    Vertex() {
      pos[0] = pos[1] = pos[2] = 0.0;
      col[0] = col[1] = col[2] = 0.0;
      uv[0] = uv[1] = 0.0;
    }
    template <typename T, typename U> Vertex(const core::_Vec<T, 3>& _pos, const core::Vec3b& _col, const core::_Vec<U, 2>& _uv) {
      pos[0] = _pos[0];
      pos[1] = _pos[1];
      pos[2] = _pos[2];
      col[0] = _col[0];
      col[1] = _col[1];
      col[2] = _col[2];
      uv[0]  = _uv[0];
      uv[1]  = _uv[1];
    }

    template <typename T> Vertex(const core::_Vec<T, 3>& _pos, const core::Vec3b& _col) {
      pos[0] = _pos[0];
      pos[1] = _pos[1];
      pos[2] = _pos[2];
      col[0] = _col[0];
      col[1] = _col[1];
      col[2] = _col[2];
      uv[0] = uv[1] = 0;
    }
  };
  core::Vec<Vertex> vertex;

  static auto offset_pos() { return offsetof(Vertex, pos); }
  static auto offset_col() { return offsetof(Vertex, col); }
  static auto offset_uv() { return offsetof(Vertex, uv); }
  static size_t vertex_byte() { return sizeof(Vertex); }
  MeshCol();
  ~MeshCol();
  MeshCol(const core::Vec<Vertex>& vertices, const core::Vec<IndicesT>& indices);
  virtual void draw(Shader*) override;
  virtual MeshType get_mesh_type() override { return MeshType::Col; }
  virtual size_t get_vertices_size() override { return vertex.size(); }
  virtual core::Vec3 get_center_of_geom() const override;
  virtual core::Rect3D get_bbox() const override;
  virtual void move_geom(const core::Vec3&) override;  /// すべてのMeshについて 第1引数だけ移動する
  virtual void scale_geom(const core::Vec3&) override; /// すべてのMeshについて 第1引数scale倍する
  virtual void rot_geom(const core::Vec3&) override;   /// すべてのMeshについて 第1引数だけ回転する
  MeshCol::Vertex* get_vertices_data() { return vertex.data(); }
  size_t get_vertices_size_in_bytes() { return vertex.size() * sizeof(Vertex); }
  template <typename T, typename U> inline void add_point(const core::_Vec<T, 3>& pos, const core::_Vec<U, 3>& col, const core::_Vec<T, 2>& uv) { vertex.push_back(std::move(Vertex(pos, col, uv))); }
  template <typename T, typename U> inline void add_point(const core::_Vec<T, 3>& pos, const core::_Vec<U, 3>& col) { vertex.push_back(std::move(Vertex(pos, col))); }
  template <typename T, typename U> inline void add_point(const core::_Vec<T, 3>& pos, const core::_Vec<U, 4>& col, const core::_Vec<T, 2>& uv) { vertex.push_back(std::move(Vertex(pos, col, uv))); }
  template <typename T, typename U> inline void add_point(const core::_Vec<T, 3>& pos, const core::_Vec<U, 4>& col) { vertex.push_back(std::move(Vertex(pos, col))); }
  void clear() {
    vertex.clear();
    indices.clear();
  }
};


/**
 * @class Meshクラス。UI用
 */
class Mesh2D : public Block, public _MeshBase {
private:
  virtual void bind() override;
  virtual void set_to_gpu() override;
  virtual void create_buffer() override;
  void set_attribute(int pos_loc, int col_loc, int uv_loc);

public:
  struct Vertex {
    int16_t pos[2];
    uint8_t col[3];
    uint16_t uv[2];
    inline Vertex(const core::Vec2d& _pos, const core::Vec3b& _col, const core::Vec2d& _uv) {
      pos[0] = _pos[0];
      pos[1] = _pos[1];
      col[0] = _col[0];
      col[1] = _col[1];
      col[2] = _col[2];
      uv[0]  = _uv[0];
      uv[1]  = _uv[1];
    }
    inline Vertex(const core::Vec2d& _pos, const core::Vec3b& _col) {
      pos[0] = _pos[0];
      pos[1] = _pos[1];
      col[0] = _col[0];
      col[1] = _col[1];
      col[2] = _col[2];
      uv[0] = uv[1] = 0;
    }
  };
  core::Vec<Vertex> vertex;

  static auto offset_pos() { return offsetof(Vertex, pos); }
  static auto offset_col() { return offsetof(Vertex, col); }
  static auto offset_uv() { return offsetof(Vertex, uv); }
  static size_t vertex_byte() { return sizeof(Vertex); }

  Mesh2D();
  ~Mesh2D();
  Mesh2D(const core::Vec<Vertex>& vertices, const core::Vec<IndicesT>& indices);
  virtual void draw(Shader*) override;
  virtual MeshType get_mesh_type() override { return MeshType::UI; }
  virtual size_t get_vertices_size() override { return vertex.size(); }
  virtual core::Vec3 get_center_of_geom() const override;
  virtual core::Rect3D get_bbox() const override;
  virtual void move_geom(const core::Vec3&) override;  /// すべてのMeshについて 第1引数だけ移動する
  virtual void scale_geom(const core::Vec3&) override; /// すべてのMeshについて 第1引数scale倍する
  virtual void rot_geom(const core::Vec3&) override;   /// すべてのMeshについて 第1引数だけ回転する
  Mesh2D::Vertex* get_vertices_data() { return vertex.data(); }
  size_t get_vertices_size_in_bytes() { return vertex.size() * sizeof(Vertex); }
  inline void add_point(const core::Vec2d& pos, const core::Vec3b& col, const core::Vec2d& uv) { vertex.push_back(std::move(Vertex(pos, col, uv))); }
  inline void add_point(const core::Vec2d& pos, const core::Vec3b& col) { vertex.push_back(std::move(Vertex(pos, col))); }
  void clear() {
    vertex.clear();
    indices.clear();
  }
};

} // namespace mu::db
