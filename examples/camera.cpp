#include <iostream>
#include <opencv2/opencv.hpp>

#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>
#include <movutl/instance/instance.hpp>

using namespace mu::core;
using namespace mu::colors;
using namespace mu::db;

int main() {
  mu::instance::init();
  LOGD << "window creation start";
  auto wnd = mu::ui::create_window("hello", 640, 480);
  LOGD << "window creation end";

  bool value;
  auto frame = mu::ui::frame("testaa", {
                                         mu::ui::check("hoge", &value),
                                         mu::ui::button("this is button", &value),
                                         mu::ui::label("test label "),
                                         mu::ui::label("日本語です"),
                                       });
  frame->setAlignType(mu::ui::uiWidgetAlignTypes::VertialListl);
  wnd->addWidgetUI(frame);
  frame->setSize(300, 300);
  frame->setPos(30, 30);

  while(!wnd->should_close()) {
    wnd->draw_ui();
    // auto r = wnd->get_renderer();
    /* r->put_text("textだよ！", {100, 100}, 2.0, {255, 255, 255}); */
    /* r->put_text("012AWE", {100, 300}, 2.0, {255, 255, 255}); */
    /* r->quad({20, 20}, {20, 100}, {100, 100}, {100, 20}, blue, red, green, purple); */
    wnd->drawDevelopperHelps();
    // auto m            = wnd->get_renderer()->mesh_ui;
    // int s             = m->get_vertices_size();
    // Mesh2D::Vertex* v = m->get_vertices_data();
    /* for(int i = 0; i < std::min(10, s); i++) { */
    /*   auto vv = v[i]; */
    /*   std::cout << vv.pos[0] << "," << vv.pos[1] << "\t" << (int)vv.col[0] << ", " << (int)vv.col[1] << "," << (int)vv.col[2] << "\t" << vv.uv[0] << ", " << vv.uv[1] << std::endl; */
    /* } */
    mu::ui::update(true);
  }

  wnd->terminate();
  glfwTerminate();
  return 0;
}
