#pragma once
#include <cwchar>
#include <movutl/core/vector.hpp>
#include <movutl/db/block.hpp>
#include <movutl/db/bone.hpp>
#include <movutl/db/image.hpp>
#include <movutl/db/mesh.hpp>
#include <movutl/db/primitives.hpp>
#include <movutl/db/shader.hpp>

namespace mu::db {

class Body : public Block {
public:
  core::Vec3 pos      = core::Vec3(0, 0, 0);
  core::Vec3 rotation = core::Vec3(0, 0, 0);
  core::Vec3 scale    = core::Vec3(1, 1, 1);

  std::vector<_MeshBase*> meshs;
  std::vector<Image*> textures;
  /* std::vector<Shader*> shaders; */
  Shader* shader;
  std::vector<Bone*> bones;
  std::string directory;
  bool gammaCorrection;

  std::vector<Primitive*> primitives;

  Body() = default;
  /* void draw(); */

  core::Vec3 get_center_of_geom() const;
  core::Rect3D get_bbox() const;

  void move_geom(const core::Vec3&);  /// すべてのMeshについて 第1引数だけ移動する
  void scale_geom(const core::Vec3&); /// すべてのMeshについて 第1引数scale倍する
  void rot_geom(const core::Vec3&);   /// すべてのMeshについて 第1引数だけ回転する

  void set_geom_to_origin(); /// Meshの平均値を0にする
  void set_origin_to_geom(); /// 現在位置は変えずにMeshの各頂点の位置をシフトする

  void apply_scale();         /// すべてのMeshに対して、scale倍して、scale = (1, 1, 1)にする
  void apply_position();      /// すべてのMeshに対して +positionだけ移動して、position = (0, 0, 0)にする
  void apply_rotation();      /// rotationをすべてのMeshに対して適応させる
  void apply_all_transform(); /// scale, position, rotationを適応し、0/1にする

  void clear_position();
  void clear_rotation();
  void clear_scale();
  core::Mat4x4f get_model_transform_matrix() const;
};
} // namespace mu::db
