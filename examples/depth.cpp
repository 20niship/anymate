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
  cam->pos   = {20, -16, -16};
  cam->dir   = {0, 0, 0};
  cam->scale = 0.4;

  auto r = wnd->get_renderer();
  r->add_mesh(reinterpret_cast<db::Mesh *>(obj->meshs[0]));

  int i=0;
  while(!wnd->should_close()) {
    r->camera.pos   = {
      std::sin(i*0.1)*10,
      std::cos(i*0.1)*10,
      -16
    };
    i++;
    mu::ui::update(true);

    auto depth = r->get_depth_image();
    auto rgb = r->get_color_image();

double min, max;
cv::minMaxLoc(depth, &min, &max);
DISP(min);
DISP(max);

    depth.convertTo(depth, CV_8UC1, 255);
    cv::applyColorMap(depth, depth, cv::COLORMAP_JET);
    cv::imshow  ("depth", depth);
    /* cv::imshow("color", rgb); */
    cv::waitKey(1);

  }
  wnd->terminate();
  glfwTerminate();
  return 0;
}
