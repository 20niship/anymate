#include <iostream>

#include <movutl/instance/instance.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>
#include <movutl/ui/icon.hpp>

using namespace mu::core;
using namespace mu::colors;
using namespace mu::db;

double fps_list[100];

int main() {
  mu::instance::init();
  auto wnd = mu::ui::create_window("hello", 640, 480);
  bool value;
  Range range = {0, 0};
  Vec3b bg(0, 0, 0);

  auto plot = mu::ui::uiPlot("fps");

  // clang-format off
  auto frame = mu::ui::frame("testaa", {
     mu::ui::check("hoge", &value),
     mu::ui::button("this is button", &value),
     mu::ui::label("test label "),
     mu::ui::label("日本語です"),
     mu::ui::range2("test", &range, {0, 10}, 0.1),
     mu::ui::Col("background", &bg),
     &plot
  });
  // clang-format on  
  
  auto table = mu::ui::uiTable("name\tvalue\nbar");
  auto frame2 = mu::ui::frame("testaa", {&table});

  frame->setSize(200, 300);
  frame->setPos(30, 30);

  frame2->setSize(200, 300);
  frame2->setPos(250, 30);

  wnd->addWidgetUI(frame);
  wnd->addWidgetUI(frame2);

  std::cout << "window creation end!" << std::endl;

  while(!wnd->should_close()) {
    {
       table.setCols(3);
       table.setRows(8); 
       table(0,0) = "name";
       table(1,0) = "value";
       table(2,0) = "bar";
       const char *names[] = {"bob", "taro", "doraemon", "nobita", "sazae", "amuro", "shaa", "conan"};
       for(int i=1; i<8; i++){
         table(0,i) = names[i];
         table(1,i) = value ? std::to_string(i) : mu::icon::PENCIL;
         table(2,i) = std::to_string((i % 2 == 0) ? range.min :range.max);
       }
    }
    wnd->get_renderer()->background_color(bg);
    wnd->draw_ui();

    // render fps
    {
      fps_list[99] = mu::instance::get_io()->fps;
      for (int i = 0; i < 99; i++) {
        fps_list[i] = fps_list[i + 1];
      }
      plot.plotLine(fps_list, 100, {{"col", "F0F"}, {"label", "test"}});
      plot.plotOther();
    }

    auto r = wnd->get_renderer();
    /* r->summary(); */
    wnd->drawDevelopperHelps();
    mu::ui::update(true);
  }

  wnd->terminate();
  glfwTerminate();
  return 0;
}
