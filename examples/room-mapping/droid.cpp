#include "droid.hpp"
#include "box_builder.hpp"
#include "movutl/ui/colors.hpp"
#include <movutl/tools/mesh/normal_estimation.hpp>
#include "plane_estimation.hpp"

#include <filesystem>
#include <movutl/tools/colormap.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

// reference : https://3d.bk.tudelft.nl/liangliang/publications/2016/manhattan/manhattan.html
// https://www.researchgate.net/publication/220718565_From_3D_point_clouds_to_climbing_stairs_A_comparison_of_plane_segmentation_approaches_for_humanoids
// Structured Indoor Modeling : https://openaccess.thecvf.com/content_iccv_2015/papers/Ikehata_Structured_Indoor_Modeling_ICCV_2015_paper.pdf

using namespace mu;
using namespace mu::tools;

TransformModifier mod_transform;
ClipModifier mod_clip;
DownsampleMod mod_downsample;
ShelfBuild mod_shelf;
/* ShelfAnalyze mod_ana; */
PlaneDetection mod_plane;
BoxConstructor mod_box;

double fps_list[100];
float normal_search_radius = 0.1;
double box_num_list[100];

db::Body *downsampled, *out, *out2;

void preproc(db::Body* obj) {
  LOGD << "==========  PREPROC =========";
  mod_downsample.apply(obj, downsampled);
  DISP(obj->meshs.size());
  mod_transform.apply(downsampled, out);
  mod_clip.apply(out, out2);
  mod_shelf.apply(out2, nullptr);

  std::cout << "end of shelf analyze" << std::endl;
  mu::core::Vec<mu::core::Vec3f> normal;
  DISP(out->meshs.size());
  const auto m = reinterpret_cast<db::MeshCol*>(out2->meshs[0]);
  {
    std::cout << "start normal estimation!" << std::endl;
    normal_estimate(m, normal, normal_search_radius);
    std::cout << "end!" << std::endl;
    mu::tools::colormap_by_normal(m, m, normal);
  }

  {
    LOGW << 1;
    mod_plane.set(m, normal);
    mod_plane.params.min_pos_error_search  = 0.2;
    mod_plane.params.min_plane_point_error = 0.1;
    mod_plane.params.min_pos_error         = 0.1;
    LOGW << "start computing.....";
    mod_plane.compute();
    mod_plane.report();
    /* mod_plane.apply_colormap(); */
    mod_plane.colormap_by_normal();
  }
  LOGD << "========  PREPROC END =======";
}


int main(int argc, char* argv[]) {
  const char* fname = argc > 1 ? argv[1] : "/home/test3/Downloads/iphone/ply/inside.ply";
  std::cout << "reading :" << fname << std::endl;

  struct DrawConfig {
    bool draw_coord       = true;
    bool draw_pc          = true;
    bool draw_planes_rect = true;
    bool draw_planes      = true;
    bool draw_shelfs      = true;
    bool draw_clip        = true;
    bool draw_box         = true;
    bool draw_baloon      = true;
    float line_width      = 0.05;

    bool should_run_preproc = false;
  } draw_cfg;

  instance::init();
  auto wnd = mu::ui::create_window("hello", 1100, 950);
  auto cam = wnd->getCameraPtr();
  auto r   = wnd->get_renderer();

  auto plot = mu::ui::uiPlot("");

  // clang-format off
  auto frame = mu::ui::frame("main", {
      mu::ui::collapse("camera", {
        mu::ui::uivec3("pos", &cam->pos, {-999, 999}, 1),
        mu::ui::uivec3("dir", &cam->dir, {-999, 999}, 1),
        mu::ui::slider("scale", &cam->scale, {0.1, 8}, 1),
        mu::ui::slider("point", &r->gl_point_size, {1, 10}, 1),
      }),

      mu::ui::collapse("metric", {
        mu::ui::slider("down", &mod_downsample.n, {1, 40}, 1),
        mu::ui::slider("rot", &mod_transform.rot[2], {0, 1.57}, 0.05),
        mu::ui::uivec3("pos", &mod_transform.pos, {-1.0, 10}, 1),
        mu::ui::slider("scale", &mod_transform.scale, {1.0, 10}, 1),
      }),

      /* mu::ui::collapse("clip", { */
      /*   mu::ui::range2("x", &mod_clip.r.x, {-15, 15}, 0.05), */
      /*   mu::ui::range2("y", &mod_clip.r.y, {-15, 15}, 0.05), */
      /*   mu::ui::range2("z", &mod_clip.r.z, {-15, 15}, 0.05), */
      /* }), */

      mu::ui::collapse("plane", {
        mu::ui::slider("norm_R", &normal_search_radius, {0.1, 0.5}, 1),
        mu::ui::slider("psearch_R", &mod_plane.params.min_pos_error_search, {0.1, 1}, 0.05),
        mu::ui::slider("minPosErr", &mod_plane.params.min_pos_error, {0.01, 0.7}, 0.05),
        mu::ui::slider("minPoints", &mod_plane.params.min_plane_points, {5, 100}, 1),
        mu::ui::slider("shelf", &mod_shelf.minarea, {-1, 5}, 1),
      }),

      mu::ui::button("Proc", &draw_cfg.should_run_preproc),

      mu::ui::collapse("box", {
        mu::ui::slider("margin", &mod_box.margin, {0, 0.1}, 0.005),
        mu::ui::slider("error", &mod_box.min_err_between_bondary, {0, 1}, 0.05),
      }),

      mu::ui::collapse("draw", {
        mu::ui::check("coord", &draw_cfg.draw_coord),
        mu::ui::check("pc", &draw_cfg.draw_pc),
        mu::ui::check("plane", &draw_cfg.draw_planes),
        mu::ui::check("rect", &draw_cfg.draw_planes_rect),
        mu::ui::check("shelf", &draw_cfg.draw_shelfs),
        mu::ui::check("clip", &draw_cfg.draw_clip),
        mu::ui::check("box", &draw_cfg.draw_box),
        mu::ui::check("baloon", &draw_cfg.draw_baloon),
        mu::ui::slider("lineW", &draw_cfg.line_width, {0, 0.1}, 0.01),
      }),
      &plot
  });
  // clang-format on
  //
  auto table  = mu::ui::uiTable("id\tx\ty\tz\tn");
  auto frame2 = mu::ui::frame("testaa", {&table});

  frame->setPos({0, 0});
  frame->setSize({270, 950});

  frame2->setPos({650, 600});
  frame2->setSize({450, 300});

  wnd->addWidget(frame);
  wnd->addWidget(frame2);

  cam->pos      = {10, 30, 30};
  cam->dir      = {0, 0, 0};
  cam->u        = {0, -1, 0};
  cam->scale    = 1.4;
  db::Body* obj = io::impl_load_ply(fname);

  Odometry odometry("/home/test3/Downloads/iphone/odometry/outside1.txt", ' ');

  downsampled = instance::create_body();
  out         = instance::create_body();
  out2        = instance::create_body();

  preproc(obj);

#if 0
  const auto& shelfs = mod_shelf.shelf_list;
  mod_ana.sl.reserve(shelfs.size());
  if(shelfs.size() > 0)
    for(int i = 0; i < shelfs.size(); i++) mod_ana.sl.push_back(ShelfAnalyze::Shelf(i, shelfs[i]));
  mod_ana.apply(out2, nullptr);
  const auto sl = mod_ana.sl;
  for(int i = 0; i < mod_ana.sl.size(); i++) mod_ana.display_img(i);
#endif


#if 0
  {
    // write to CSV file
    const char* fileName = "planes.csv";
    std::ofstream ofs(fileName);
    if(!ofs) {
      std::cout << "ファイルが開けませんでした。" << std::endl;
      return 0;
    }

    for(const auto& p : mod_plane.planes) {
      ofs << p.r.x.min << "," << p.r.x.max << "," << p.r.y.min << "," << p.r.y.max << "," << p.r.z.min << "," << p.r.z.max << "," << p.nPoints << "," << p.dir << std::endl;
    }
    std::cout << fileName << "に書き込みました。" << std::endl;
  }
#endif

  while(!wnd->should_close()) {
    r->clear_not_default_mesh();

    if(draw_cfg.should_run_preproc){
      preproc(obj);
      draw_cfg.should_run_preproc = false;
    }

    {
      mod_box.set_plane(mod_plane.planes);
      mod_box.compute();
    }

    {
      table.setCols(4);
      table.setRows(mod_box.boxes.size() + 1);
      table(0, 0) = "name";
      table(1, 0) = "x";
      table(2, 0) = "y";
      table(2, 0) = "z";
      for(size_t i = 1; i < mod_box.boxes.size(); i++) {
        table(0, i)  = std::to_string(i);
        const auto b = mod_box.boxes[i];
        table(1, i)  = std::to_string(b.x.min) + " - " + std::to_string(b.x.max);
        table(2, i)  = std::to_string(b.y.min) + " - " + std::to_string(b.y.max);
        table(3, i)  = std::to_string(b.z.min) + " - " + std::to_string(b.z.max);
      }
    }

    wnd->draw_ui();

    {
      fps_list[99] = mu::instance::get_io()->fps;
      for(int i = 0; i < 99; i++) fps_list[i] = fps_list[i + 1];

      box_num_list[99] = mod_box.boxes.size();
      for(int i = 0; i < 99; i++) box_num_list[i] = box_num_list[i + 1];
      plot.plotLine(fps_list, 100, {{"col", "F00"}, {"label", "fps"}});
      plot.plotLine(box_num_list, 100, {{"col", "00F"}, {"label", "boxes"}});
      plot.plotOther();
    }

    if(draw_cfg.draw_clip) r->cube(mod_clip.r.center<double>(), mod_clip.r.size<double>(), colors::red, 0.05);

    if(draw_cfg.draw_shelfs) {
      int i = 0;
      for(const auto& s : mod_shelf.shelf_list) {
        const auto c    = s.center();
        const auto size = s.size();
        const auto c3d  = Vec3(c[0], c[1], 0);
        r->cube(c3d, {size[0], size[1], 0.1}, colors::red, 0.05);
        r->baloon("Shelf" + std::to_string(i), c3d, 1.0);
        i++;
      }
    }

    if(draw_cfg.draw_pc && out2->meshs.size() > 0) {
      const auto m = reinterpret_cast<MeshCol*>(out2->meshs[0]);
      r->add_mesh(m);
    }

    if(draw_cfg.draw_planes_rect) {
      for(size_t i = 0; i < mod_plane.planes.size(); i++) {
        const auto rect = mod_plane.planes[i].r.margin(mod_box.margin);
        const auto dir  = mod_plane.planes[i].dir;
        if(dir == NORMAL_NOT_DEFINED) continue;
        const auto c   = rect.center<double>();
        const auto col = mu::colors::colormap[dir % mu::colors::colormap.size()];
        r->cube(c, rect.size<double>(), col, draw_cfg.line_width);
      }
    }

    if(draw_cfg.draw_planes) {
      for(size_t i = 0; i < mod_plane.planes.size(); i++) {
        const auto rect = mod_plane.planes[i].r;
        const auto dir  = mod_plane.planes[i].dir;
        if(dir == NORMAL_NOT_DEFINED) continue;
        const auto c = rect.center<double>();
        auto size    = rect.size<double>();
        if(dir == NORMAL_X) size[0] = 0.0001;
        if(dir == NORMAL_Y) size[1] = 0.0001;
        if(dir == NORMAL_Z) size[2] = 0.0001;
        const auto col = mu::colors::colormap[dir % mu::colors::colormap.size()];
        if(draw_cfg.line_width <= 0)
          r->cube(c, size, col);
        else
          r->cube(c, size, col, draw_cfg.line_width);
      }
    }

    if(draw_cfg.draw_box) {
      for(size_t i = 0; i < mod_box.boxes.size(); i++) {
        const auto rect = mod_box.boxes[i];
        const auto c    = rect.center<double>();
        const auto col  = mu::colors::colormap[i % mu::colors::colormap.size()];
        r->cube(c, rect.size<double>(), col, std::max(0.02f, draw_cfg.line_width));
        if(draw_cfg.draw_baloon) r->baloon("b" + std::to_string(i), c, 1.0);
      }
    }

    {
      const std::string str = "shelf = " + std::to_string(mod_shelf.shelf_list.size()) + " box = " + std::to_string(mod_box.boxes.size());
      const auto ws         = wnd->getWindowSize();
      const Vec2d pos(ws[0] - 300, 20);
      r->put_text(str, pos, 1.0, colors::white);
    }


#if 0
      for(size_t i = 0; i < odometry.els.size() - 1; i++) {
        const auto& o  = odometry.els[i];
        const auto& o2 = odometry.els[i + 1];
        assert(o.cnt > 0);
        const auto p1 = Vec3(o.value[3], o.value[7], o.value[10]);
        const auto p2 = Vec3(o2.value[3], o2.value[7], o.value[10]);
        r->line(p1, p2, colors::yellow, draw_cfg.line_width);
      }
#endif

    if(draw_cfg.draw_coord) r->coord({0, 0, 0}, {0, 0, 3}, 0.1);
    mu::ui::update(true);
  }
  wnd->terminate();
  glfwTerminate();
  return 0;
}
