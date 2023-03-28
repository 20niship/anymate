#include <movutl/mu.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>

using namespace mu;

int main() {
  instance::init();
  auto wnd      = mu::ui::create_window("teapot", 640, 480);
  auto cam      = wnd->getCameraPtr();
  auto path     = std::string(MOVUTL_DATA_DIR) + "/teapot.stl";
  db::Body* obj = io::impl_load_obj(path.c_str());

  LOGD << "window creation end";
  /* wnd->addWidget(obj); */

  // clang-format off
  auto frame = mu::ui::frame("main", {
      mu::ui::collapse("camera", {
        mu::ui::uivec3("pos", &cam->pos, {-999, 999}, 1),
        mu::ui::uivec3("dir", &cam->dir, {-999, 999}, 1),
        mu::ui::slider("scale", &cam->scale, {0.1, 100}, 1),
      })
  });
  // clang-format on
  wnd->addWidget(frame);
  cam->pos   = {20, -16, -16};
  cam->dir   = {0, 0, 0};
  cam->scale = 5.0;

  DISP(obj->meshs.size());
  DISP(obj->meshs[0]->get_vertices_size());
  auto r = wnd->get_renderer();
  r->add_mesh(reinterpret_cast<db::Mesh *>(obj->meshs[0]));

  LOGD << "Start rendering!";

  while(!wnd->should_close()) {
    wnd->draw_ui();
    r->coord({0, 0, 0}, {0, 0, 0.5}, 0.01);
    r->summary();
    const auto bbox = obj->meshs[0]->get_bbox();
    r->cube(bbox.center<double>(), bbox.size<double>(), mu::colors::red, 0.03);
    mu::ui::update(true);
  }
  wnd->terminate();
  glfwTerminate();
  return 0;
}
