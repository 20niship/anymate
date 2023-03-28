#pragma once

#include <movutl/core/vector.hpp>
#include <movutl/db/body.hpp>
#include <movutl/db/camera.hpp>
#include <movutl/db/light.hpp>
#include <movutl/db/mesh.hpp>
#include <movutl/db/particle.hpp>
#include <movutl/db/shader.hpp>
#include <movutl/db/world.hpp>


namespace mu::db {

using namespace mu::core;

class DBMain {
  DBMain()  = default;
  ~DBMain() = default;
  Vec<Mesh> meshes;
  Vec<Body> bodies;
};

auto get_render_context();
auto get_meshes();
auto get_objects();
auto get_lights();
auto get_worlds();
auto get_shaders();
auto get_particles();
auto get_cameras();

auto get_app();

}; // namespace mu::db
