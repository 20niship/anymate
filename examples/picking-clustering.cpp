#include <movutl/experimental/picking_object.hpp>
#include <movutl/mu.hpp>
#include <movutl/tools/colormap.hpp>
#include <movutl/tools/mesh/meshtools.hpp>
#include <movutl/tools/mesh/segmentation/cluster.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>

using namespace mu;
using namespace mu::db;
using namespace mu::experimental;
using namespace mu::Magi;
using namespace mu::core;
using namespace mu::render;

template <typename T> void remove_duplicates(std::vector<T>& list) {
  std::sort(list.begin(), list.end());
  list.erase(std::unique(list.begin(), list.end()), list.end());
}

int main(int argc, char* argv[]) {
  spdlog::set_level(spdlog::level::critical);
  instance::init();
  auto wnd = mu::ui::create_window("hello", 1080, 720);
  auto cam = wnd->getCameraPtr();
  auto r   = wnd->get_renderer();

  std::vector<size_t> indices, indices_tmp;

  struct Config {
    float radius         = 5;
    bool picking_cluster = false;
    enum SelectType { Circle, Rect };
    SelectType type = Circle;
  } config;


  // clang-format off
  auto frame = mu::ui::frame("main", {
      mu::ui::collapse("camera", {
        mu::ui::uivec3("pos", &cam->pos, {-999, 999}, 1),
        mu::ui::uivec3("dir", &cam->dir, {-999, 999}, 1),
        mu::ui::slider("scale", &cam->scale, {0.1, 3}, 1),
        mu::ui::slider("point", &r->gl_point_size, {1, 10}, 1),
      }),
      mu::ui::collapse("picking", {
        mu::ui::check("cluster", &config.picking_cluster),
        mu::ui::slider("r", &config.radius, {0,20}),
        mu::ui::button("circle", [&]{config.type = Config::Circle;}),
        mu::ui::button("rect", [&]{config.type = Config::Rect;}),
        mu::ui::button("clear", [&]{
            indices.clear();
            }),
      }),
  });
  // clang-format on

  frame->setSize(240, 500);
  wnd->addWidget(frame);
  cam->pos   = {30, 30, 30};
  cam->dir   = {0, 0, 0};
  cam->scale = 0.6;

  PointCloudPicker p;

  const std::string path = (argc > 1) ? argv[1] : std::string(MOVUTL_DATA_DIR) + "/residential.pcd";
  auto cloud             = io::impl_load_pcd(path.c_str());
  cloud->set_geom_to_origin();
  cloud->apply_all_transform();
  const auto bbox = cloud->get_bbox();
  auto m          = (MeshCol*)cloud->meshs[0];
  auto m2         = instance::create_mesh_col();

  ClusterManagerRGB seg;
  seg.set_cloud(m);
  seg.run();
  seg.get_colored_output(m2);

  p.setup();
  p.set_mesh(m);
  p.set_cluster_manager(&seg);

  r->summary();

  while(!wnd->should_close()) {
    p.set_use_cluter(config.picking_cluster);
    p.update(r->camera);

    const auto io = instance::get_io();
    {
      const auto pos = io->mouse_pos;
      switch(int(config.type)) {
        case Config::Circle:
          if(io->is_mouse_left_pressing()) {
            const auto index = p.get_point_circle(pos, config.radius);
            for(const auto i : index) indices.push_back(i);
          }
          break;
        case Config::Rect:
          if(io->is_mouse_left_pressing()) {
            indices_tmp.clear();
            const auto startpos = io->button[(int)ui::uiMouseButton::Left].start_pos;
            const auto index    = p.get_point_rect(startpos, pos);
            r->rectPS(startpos, pos - startpos, colors::blue, 1);
            for(const auto i : index) indices_tmp.push_back(i);
          } else {
            for(const auto i : indices_tmp) indices.push_back(i);
            indices_tmp.clear();
          }
      }
    }

    r->clear_not_default_mesh();
    if(io->is_key_pressing(mu::ui::MU_KEY_TAB))
      r->add_mesh(m2);
    else
      r->add_mesh(m);
    wnd->draw_ui();
    r->coord({0, 0, 0}, {0, 0, 6});
    r->cube(bbox, colors::yellow, 0.1);
    for(const auto i : indices) {
      m->vertex[i].col[0] = 255;
      m->vertex[i].col[1] = 0;
      m->vertex[i].col[2] = 0;
    }
    for(const auto i : indices_tmp) {
      m->vertex[i].col[2] = 255;
      m->vertex[i].col[1] = 0;
      m->vertex[i].col[0] = 0;
    }
    mu::ui::update(true);
  }
  wnd->terminate();
  glfwTerminate();
  return 0;
}
