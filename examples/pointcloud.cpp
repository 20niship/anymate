#include <movutl/db/mesh.hpp>
#include <movutl/mu.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>

using namespace mu;

int main(int argc, char* argv[]) {
  instance::init();
  auto wnd    = mu::ui::create_window("hello", 640, 480);
  auto cam    = wnd->getCameraPtr();
  auto r      = wnd->get_renderer();

  // clang-format off
  auto frame = mu::ui::frame("main", {
      mu::ui::collapse("camera", {
        mu::ui::uivec3("pos", &cam->pos, {-999, 999}, 1),
        mu::ui::uivec3("dir", &cam->dir, {-999, 999}, 1),
        mu::ui::slider("scale", &cam->scale, {0.1, 10}, 1),
        mu::ui::slider("point", &r->gl_point_size, {1, 10}, 1),
      }),
  });
  // clang-format on

  wnd->addWidget(frame);
  cam->pos   = {10, 10, 10};
  cam->dir   = {0, 0, 0};
  cam->scale = 0.6;

  const std::string path = (argc > 1) ? argv[1] : std::string(MOVUTL_DATA_DIR) + "/residential.pcd";
  auto cloud = io::impl_load_pcd(path.c_str());
  cloud->set_geom_to_origin();
  cloud->apply_all_transform();
  const auto bbox = cloud->get_bbox();

  r->add_mesh((db::MeshCol*)cloud->meshs[0]);
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
