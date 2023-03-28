#pragma once
#include <movutl/core/vector.hpp>
#include <db/image.hpp>
#include <db/shader.hpp>
#include <db/mesh.hpp>

namespace mu::render {

struct Drawlist {

  /// one drawcall (glDrawArrays)
  struct DrawCmd {
    int start;          /// start index of mesh for draw
    int size;           /// size of mesh to draw
    db::Image* tex_ptr; /// texture id
  };

  struct Pipeline {
    db::Shader* shader_ptr;  /// shader
    core::Vec<DrawCmd> cmds; ///
  };
  std::vector<DrawCmd> cmd;
};

}; // namespace mu::render
   
