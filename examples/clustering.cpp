#include <movutl/mu.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>
#include <movutl/tools/mesh/segmentation/cluster.hpp>
#include <movutl/tools/colormap.hpp>

using namespace mu;
using namespace mu::Magi;
using namespace mu::db;
using namespace mu::core;
using namespace mu::render;

int main(int argc, char* argv[]) {
  instance::init();
  auto wnd    = mu::ui::create_window("hello", 1080, 720);
  auto cam    = wnd->getCameraPtr();
  auto r      = wnd->get_renderer();

  // clang-format off
  auto frame = mu::ui::frame("main", {
      mu::ui::collapse("camera", {
        mu::ui::uivec3("pos", &cam->pos, {-999, 999}, 1),
        mu::ui::uivec3("dir", &cam->dir, {-999, 999}, 1),
        mu::ui::slider("scale", &cam->scale, {0.1, 1}, 1),
        mu::ui::slider("point", &r->gl_point_size, {1, 10}, 1),
      }),
  });
  // clang-format on

  wnd->addWidget(frame);
  cam->pos   = {30, 30, 30};
  cam->dir   = {0, 0, 0};
  cam->scale = 0.6;

  const std::string path = (argc > 1) ? argv[1] : std::string(MOVUTL_DATA_DIR) + "/residential.pcd";
  auto cloud = io::impl_load_pcd(path.c_str());
  cloud->set_geom_to_origin();
  cloud->apply_all_transform();
  const auto bbox = cloud->get_bbox();
  auto m = (MeshCol *)cloud->meshs[0];

  ClusterManagerNormal seg;
  seg.set_cloud(m);
  seg.run();
  seg.get_colored_output(m);

  r->add_body(cloud);
  r->summary();

  while(!wnd->should_close()) {
    wnd->draw_ui();
    r->coord({0, 0, 0}, {0, 0, 6});
    r->cube(bbox, colors::yellow, 0.1);
    mu::ui::update(true);
  }
  wnd->terminate();
  glfwTerminate();
  return 0;
}
