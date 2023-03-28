#include <movutl/experimental/CSVreader.hpp>
#include "box_builder.hpp"
#include "movutl/ui/colors.hpp"

#include <filesystem>
#include <movutl/instance/instance.hpp>
#include <movutl/tools/colormap.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <vector>

// reference : https://3d.bk.tudelft.nl/liangliang/publications/2016/manhattan/manhattan.html
// https://www.researchgate.net/publication/220718565_From_3D_point_clouds_to_climbing_stairs_A_comparison_of_plane_segmentation_approaches_for_humanoids

using namespace mu;

BoxConstructor mod_box;

struct DrawConfig {
  bool draw_rect   = true;
  bool draw_box    = true;
  bool draw_planes = true;
  bool draw_baloon = true;
} draw_cfg;

int main(int argc, char* argv[]) {
  /* if(argc < 2) { */
  /*   std::cerr << "引数にファイル名を指定してください" << std::endl; */
  /*   return EXIT_FAILURE; */
  /* } */

  mu::instance::init();
  auto wnd = mu::ui::create_window("hello", 900, 900);
  auto cam = wnd->getCameraPtr();
  auto r   = wnd->get_renderer();

  // clang-format off
  auto frame = mu::ui::frame("main", {
      mu::ui::collapse("camera", {
        mu::ui::uivec3("pos", &cam->pos, {-999, 999}, 1),
        mu::ui::uivec3("dir", &cam->dir, {-999, 999}, 1),
        mu::ui::slider("scale", &cam->scale, {0.1, 8}, 1),

        mu::ui::check("plane", &draw_cfg.draw_planes),
        mu::ui::check("rect", &draw_cfg.draw_rect),
        mu::ui::check("box", &draw_cfg.draw_box),
        mu::ui::check("baloon", &draw_cfg.draw_baloon),

        mu::ui::slider("margin", &mod_box.margin, {0, 0.5}, 0.05),
        mu::ui::slider("error", &mod_box.min_err_between_bondary, {0, 1}, 0.05),
      }),
  });
  // clang-format on
  //
  frame->setPos({0, 0});
  frame->setSize({200, 900});

  wnd->addWidget(frame);
  cam->pos   = {10, 30, 30};
  cam->dir   = {0, 0, 0};
  cam->u     = {0, -1, 0};
  cam->scale = 3.4;


  std::vector<Plane> planes;
  {
    const char* fname = argc > 1 ? argv[1] : "./planes.csv";

    CSVreader<float, 8> csv(fname);
    DISP(csv.els.size());
    planes.resize(csv.els.size());
    for(size_t i = 0; i < csv.els.size(); i++) {
      planes[i].r.x.min = csv.els[i].value[0];
      planes[i].r.x.max = csv.els[i].value[1];
      planes[i].r.y.min = csv.els[i].value[2];
      planes[i].r.y.max = csv.els[i].value[3];
      planes[i].r.z.min = csv.els[i].value[4];
      planes[i].r.z.max = csv.els[i].value[5];
      planes[i].nPoints = csv.els[i].value[6];
      planes[i].dir     = (PlaneNormalDir)csv.els[i].value[7];
    }
  }

  mod_box.min_err_between_bondary = 0.3;
  mod_box.margin                  = 0;

  while(!wnd->should_close()) {
    r->clear_not_default_mesh();
    wnd->draw_ui();

    {
      mod_box.set_plane(planes);
      mod_box.compute();
    }

    if(draw_cfg.draw_rect) {
      for(size_t i = 0; i < planes.size(); i++) {
        const auto rect = planes[i].r.margin(mod_box.margin);
        const auto dir  = planes[i].dir;
        if(dir == NORMAL_NOT_DEFINED) continue;
        const auto c   = rect.center<double>();
        const auto col = mu::colors::colormap[dir % mu::colors::colormap.size()];
        r->cube(c, rect.size<double>(), col, 0.01);
        /* r->baloon("plane" + std::to_string(i), c, 1.0); */
      }
    }

    if(draw_cfg.draw_planes) {
      for(size_t i = 0; i < planes.size(); i++) {
        const auto rect = planes[i].r;
        const auto dir  = planes[i].dir;
        if(dir == NORMAL_NOT_DEFINED) continue;
        const auto c = rect.center<double>();
        auto size    = rect.size<double>();
        if(dir == NORMAL_X) size[0] = 0.0001;
        if(dir == NORMAL_Y) size[1] = 0.0001;
        if(dir == NORMAL_Z) size[2] = 0.0001;
        const auto col = mu::colors::colormap[dir % mu::colors::colormap.size()];
        r->cube(c, size, col);
      }
    }

    if(draw_cfg.draw_box) {
      for(size_t i = 0; i < mod_box.boxes.size(); i++) {
        const auto rect = mod_box.boxes[i];
        const auto c    = rect.center<double>();
        auto size       = rect.size<double>();
        const auto col  = mu::colors::colormap[i % mu::colors::colormap.size()];
        r->cube(c, size, col, 0.03);
        if(draw_cfg.draw_baloon) r->baloon("b" + std::to_string(i), c, 1.0);
      }
    }
    r->coord({0, 0, 0}, {0, 0, 2}, 0.05);
    mu::ui::update(true);
  }
  wnd->terminate();
  glfwTerminate();
  return 0;
}
