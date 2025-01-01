#pragma once
#include <movutl/db/block.hpp>
#include <movutl/db/body.hpp>

namespace mu::db {

class LayerBase {};
class AnimRoot {};

class Collection : public LayerBase {
private:
  int start_frame;
  int end_frame;
  AnimRoot* anim;
  const char* name;
  std::vector<Body *>body_list;
public:
  Collection();
  Collection(const char* name_);
  ~Collection() = default;
  void draw();
};

} // namespace mu::db
