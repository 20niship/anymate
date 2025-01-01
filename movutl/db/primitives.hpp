#pragma once
#include <cwchar>
#include <movutl/core/quotanion.hpp>
#include <movutl/core/vector.hpp>

namespace mu::db {

struct Primitive {
  enum class PrimitiveType {
    Mesh,
    Box,
    Sphere,
    Cone,
    Cylinder,
    Capsule,
    Point,
    PointCloud,
    NotDefined,
  };

  struct Point {
    core::Vec3 pos;
    Point(){};
  };

  struct Box {
    core::Vec3 center;
    core::Quat quat;
    core::Vec3 size;
    double radius;
    Box() {}
  };

  struct Sphere {
    core::Vec3 center;
    double radius;
    Sphere() {}
  };

  struct Cylinder {
    core::Vec3 center;
    core::Vec3 axis;
    double radius;
    double length;
    Cylinder() {}
  };

  struct Capsule {
    core::Vec3 center;
    core::Vec3 axis;
    double radius;
    double length;
    Capsule() {}
  };

  struct Cone {
    core::Vec3 center; // 円錐の底面の中央のこと
    core::Vec3 axis;
    double radius;
    double length;
    Cone() {}
  };

  struct Plane {
    core::Vec3 center; // 円錐の底面の中央のこと
    core::Quat quat;
    core::Vec2 size;
    Plane() {}
  };

  struct Triangle {
    core::Vec3 pos[3];
    Triangle() {}
  };

  struct TriMesh {
    std::vector<core::Vec3> mesh;
    TriMesh() {}
  };

  PrimitiveType type = PrimitiveType::NotDefined;
  union PrimitiveData {
    PrimitiveData() : pt() {}
    ~PrimitiveData() {}
    Point pt{};
    Box b;
    Sphere s;
    Cylinder c;
    Capsule cp;
    Cone co;
    Plane p;
    /* TriMesh m; */
    Triangle t;
  };
  PrimitiveData data;

  Primitive() {}
};

} // namespace mu::db
