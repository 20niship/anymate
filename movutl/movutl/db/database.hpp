/**
 * @file db.hpp
 * @brief すべてのデータを格納する元となる構造体の定義
 * @author 書いた人
 * @date 日付（開始日？）
 */

#pragma once

#include <cstdint>
#include <vector>


/**
 * @namespace mu::database 
 * @brief  Databaseの説明
 *
 * 詳細な説明
 * 詳細な説明
 * 詳細な説明
 * 詳細な説明
 * 詳細な説明
 * 詳細な説明
 */
namespace mu::database{

enum class DataType {

};

/**
 * @brief
 */

enum class ObjectType : uint32_t {
  ProjectSetting,

  Composition,
  Image,
  Video,
  Audio,
  Font,
  Mask,

  Particle,
  Mesh,
  Light,
  Texture,
  Material,
  Collection,
  PhysicsWorld,
  Speaker,

  Effects,
  Node,
  CustomObject,

  Leyer,
};

class ObjectBase {
public :
  struct ID {
    uint16_t object_type; /// データタイプごとに異なる値を取る。
    uint16_t custom_type; /// ユーザー定義のオブジェクトについて異なる値を取る。このときobject_type == OBJECT_TYPE_USERDEINEDになる
    uint64_t instance_id; ///インスタンスごとに任意の値が設定される。
    const char *name;        /// property name
  };
  ID id; // IDはそれ全体でプロジェクト内で重複しない値となり、これでリンクすることができる。
};



} // namespace MU::DB
