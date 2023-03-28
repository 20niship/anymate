#include <movutl/instance/instance.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>

using namespace mu::core;
using namespace mu::colors;
using namespace mu::db;

int main() {
  mu::instance::init();
  auto wnd = mu::ui::create_window("hello", 640, 480);
  auto cam = wnd->getCameraPtr();

  bool sphere   = true;
  bool line     = false;
  bool triangle = false;
  bool cube     = false;
  bool coord    = false;
  bool cylinder = false;
  bool wire     = false;

  float width = 1.0;

  // clang-format off
  Vec3 pos, target;
  auto frame = mu::ui::frame("main", {
      mu::ui::collapse("camera", {
        mu::ui::uivec3("pos", &cam->pos, {-999, 999}, 1),
        mu::ui::uivec3("dir", &cam->dir, {-999, 999}, 1),
        mu::ui::slider("scale", &cam->scale, {0.1, 10}, 1),
        mu::ui::slider("near", &cam->zNear, {0.1, 100}, 1),
        mu::ui::slider("far", &cam->zFar, {0.1, 900}, 10),
      }),
      mu::ui::collapse("camera", {
        mu::ui::slider("width", &width, {0.1, 10}, 1),
        mu::ui::check("sphere", &sphere),
        mu::ui::check("line", &line),
        mu::ui::check("triangle", &triangle),
        mu::ui::check("cube", &cube),
        mu::ui::check("coord", &coord),
        mu::ui::check("cylinder", &cylinder),
        mu::ui::check("WIRED", &wire),
      })
  });
  // clang-format on

  frame->setSize(200, 480);
  wnd->addWidget(frame);
  cam->pos = {10, 10, 10};
  cam->dir = {0, 0, 0};

  while(!wnd->should_close()) {
    wnd->draw_ui();
    auto r = wnd->get_renderer();
    if(!wire) {
      if(cylinder) r->cylinder({0, 0, 0}, {1, 0, 0}, 0.3, green);
      if(coord) r->coord({0, 0, 0}, {0, 0, 10}, 1);
      if(cube) r->cube({0, 0, 0}, {1, 1, 1}, red);
      if(sphere) r->sphere_20({0, 0, 0}, 1.5, red);
      if(line) r->line(Vec3(0, 0, 0), {10, 0, 0}, red, width);
      if(triangle) r->triangle(Vec3(0, 0, 0), {5, 0, 0}, {0, 0, 5}, red);
    } else {
      if(cylinder) r->cylinder({0, 0, 0}, {1, 0, 0}, 0.3, green, width);
      if(coord) r->coord({0, 0, 0}, {0, 0, 10}, width);
      if(cube) r->cube({0, 0, 0}, {1, 1, 1}, red, width);
      if(sphere) r->sphere_20({0, 0, 0}, 1.5, red, width);
      if(line) r->line(Vec3(0, 0, 0), {10, 0, 0}, red, width);
      if(triangle) r->triangle(Vec3(0, 0, 0), {5, 0, 0}, {0, 0, 5}, red, width);
    }
    /* r->summary(); */

    r->baloon("origin", Vec3(0, 0, 0), 1.0);

    wnd->drawDevelopperHelps();
    mu::ui::update(true);
  }

  wnd->terminate();
  glfwTerminate();
  return 0;
}
