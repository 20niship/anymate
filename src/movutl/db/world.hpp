#pragma once
#include <movutl/db/block.hpp>
#include <movutl/db/body.hpp>

namespace mu::db {

class World : public Block{
  core::Vec<Body *> bodies;
  void draw();
};

} // namespace mu::db
