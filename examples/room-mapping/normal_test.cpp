#include "droid.hpp"
#include <movutl/experimental/CSVreader.hpp>
#include "movutl/ui/colors.hpp"
#include "plane_estimation.hpp"

#include <filesystem>
#include <movutl/tools/colormap.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/tools/mesh/normal_estimation.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

// reference : https://3d.bk.tudelft.nl/liangliang/publications/2016/manhattan/manhattan.html
// https://www.researchgate.net/publication/220718565_From_3D_point_clouds_to_climbing_stairs_A_comparison_of_plane_segmentation_approaches_for_humanoids

using namespace mu;

int main() {
  /* if(argc < 2) { */
  /*   std::cerr << "引数にファイル名を指定してください" << std::endl; */
  /*   return EXIT_FAILURE; */
  /* } */
  /* const char* fname = argv[1]; */
  const char* fname = "/home/test3/Downloads/iphone/ply/inside.ply";
  std::cout << "reading :" << fname << std::endl;

  instance::init();
  auto wnd = mu::ui::create_window("hello", 900, 900);
  auto cam = wnd->getCameraPtr();
  auto r   = wnd->get_renderer();

  TransformModifier mod_transform;
  ClipModifier mod_clip;
  DownsampleMod mod_downsample;
  ShelfBuild mod_shelf;
  ShelfAnalyze mod_ana;
  PlaneDetection mod_plane;

  // clang-format off
  auto frame = mu::ui::frame("main", {
      mu::ui::collapse("camera", {
        mu::ui::uivec3("pos", &cam->pos, {-999, 999}, 1),
        mu::ui::uivec3("dir", &cam->dir, {-999, 999}, 1),
        mu::ui::slider("scale", &cam->scale, {0.1, 8}, 1),
        mu::ui::slider("point", &r->gl_point_size, {1, 10}, 1),
      }),

      mu::ui::collapse("metric", {
        mu::ui::slider("down", &mod_downsample.n, {0.1, 100}, 1),
        mu::ui::slider("rot", &mod_transform.rot[2], {0, 1.57}, 0.05),
        mu::ui::uivec3("pos", &mod_transform.pos, {-1.0, 10}, 1),
      }),

      mu::ui::collapse("clip", {
        mu::ui::range2("x", &mod_clip.r.x, {-15, 15}, 0.05),
        mu::ui::range2("y", &mod_clip.r.y, {-15, 15}, 0.05),
        mu::ui::range2("z", &mod_clip.r.z, {-15, 15}, 0.05),
      }),

      mu::ui::collapse("shelf", {
        mu::ui::slider("shelf", &mod_shelf.minarea, {-1, 5}, 1),
      }),
  });
  // clang-format on
  //
  frame->setPos({0, 0});
  frame->setSize({200, 900});

  wnd->addWidget(frame);
  cam->pos      = {10, 30, 30};
  cam->dir      = {0, 0, 0};
  cam->u        = {0, -1, 0};
  cam->scale    = 1.4;
  db::Body* obj = io::impl_load_ply(fname);

  Odometry odometry("/home/test3/Downloads/iphone/odometry/outside1.txt", ' ');

  db::Body* downsampled = instance::create_body();
  db::Body* out         = instance::create_body();
  db::Body* out2        = instance::create_body();

  mod_downsample.apply(obj, downsampled);

  LOGD << "window creation end";
  /* wnd->addWidget(obj); */

  DISP(obj->meshs.size());
  mod_transform.apply(downsampled, out);
  mod_clip.apply(out, out2);


  LOGI << "start main loop!";

  std::cout << "end of shelf analyze" << std::endl;
  mu::core::Vec<mu::core::Vec3f> normal;
  DISP(out->meshs.size());
  const auto m = reinterpret_cast<db::MeshCol*>(out2->meshs[0]);
  {
    std::cout << "start normal estimation!" << std::endl;

    mu::tools::NormalEstimation n;
    n.setRadius(0.1);
    n.setMesh(m);
    n.compute();
    n.flip();
    std::cout << "end!" << std::endl;
    normal = n.normal;
    mod_plane.set(m, normal);
    mod_plane.colormap_by_normal();
  }

  while(!wnd->should_close()) {
    r->clear_not_default_mesh();
    wnd->draw_ui();
    r->cube(mod_clip.r.center<double>(), mod_clip.r.size<double>(), colors::red, 0.05);
    const auto m = reinterpret_cast<MeshCol*>(out2->meshs[0]);
    r->add_mesh(m);
    r->coord({0, 0, 0}, {0, 0, 3}, 0.1);
    mu::ui::update(true);
  }
  wnd->terminate();
  glfwTerminate();
  return 0;
}
