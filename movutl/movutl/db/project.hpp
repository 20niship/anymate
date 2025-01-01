#pragma once 

#include <movutl/db/anim.hpp>
#include <movutl/db/body.hpp>
#include <movutl/db/effect.hpp>
#include <movutl/db/collection.hpp>
#include <movutl/db/mesh.hpp>
#include <movutl/db/world.hpp>

namespace mu::db {

struct Project : Block {
  core::Vec<_MeshBase *> meshes;
  core::Vec<World> worlds;
  core::Vec<Body> bodies;
  core::Vec<Collection> layers;

  Project() = default;
  ~Project() = default;
};

} // namespace mu::db
