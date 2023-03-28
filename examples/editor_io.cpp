#include <movutl/instance/instance.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/io.hpp>

using namespace mu::core;
using namespace mu::db;

using mu::ui::uiMouseButton::Left;
using mu::ui::uiMouseButton::Middle;
using mu::ui::uiMouseButton::Redo;
using mu::ui::uiMouseButton::Right;
using mu::ui::uiMouseButton::Undo;

using mu::ui::muKey;

int main() {
  mu::instance::init();
  LOGD << "window creation start";
  auto wnd = mu::ui::create_window("hello", 640, 480);
  LOGD << "window creation end";

  const auto* io = mu::instance::get_io();


  bool value;
  Range range{0, 0};
  // clang-format off  
  auto frame = mu::ui::frame("testaa", {
     mu::ui::check("hoge", &value),
     mu::ui::button("this is button", &value),
     mu::ui::label("test label "),
     mu::ui::label("日本語です"),
     mu::ui::range2("test", &range, {0, 10}, 0.1),
   });
  // clang-format on  
  
  frame->setAlignType(mu::ui::uiWidgetAlignTypes::VertialListl);
  wnd->addWidgetUI(frame);
  frame->setSize(300, 300);
  frame->setPos(30, 30);

  while(!wnd->should_close()) {
    wnd->draw_ui();
    wnd->drawDevelopperHelps();
    std::stringstream s;
    s << "---button---\n";
    s << "left   = " << io->button[(int)Left].pressing << " " << io->button[(int)Left].clicked_count << "\n";
    s << "right  = " << io->button[(int)Right].pressing << " " << io->button[(int)Right].clicked_count << "\n";
    s << "middle = " << io->button[(int)Middle].pressing << " " << io->button[(int)Middle].clicked_count << "\n";
    s << "undo   = " << io->button[(int)Undo].pressing << " " << io->button[(int)Undo].clicked_count << "\n";
    s << "redo   = " << io->button[(int)Redo].pressing << " " << io->button[(int)Redo].clicked_count << "\n";

    s << "---mouse---\n";
    s << "enter = " << io->keys[(int)muKey::MU_KEY_ENTER].down << "\n";
    s << "alt   = " << io->keys[(int)muKey::MU_KEY_LEFT_ALT].down << ", " << io->keys[(int)muKey::MU_KEY_RIGHT_ALT].down << "\n ";
    s << "shift = " << io->keys[(int)muKey::MU_KEY_LEFT_SHIFT].down << ", " << io->keys[(int)muKey::MU_KEY_RIGHT_SHIFT].down << "\n ";
    s << "tab   = " << io->keys[(int)muKey::MU_KEY_TAB].down << "\n";

    s << "---time---\n";
    s << "uptime  = " << io->get_uptime_ms() << "ms \n";
    s << "fps     = " << io->fps << "\n";

    const std::string str = s.str();
    /* std::cout << str << std::endl; */
    auto r = wnd->get_renderer();
    r->put_text(str, {300, 10}, 1.0);
    mu::ui::update(true);
  }

  wnd->terminate();
  glfwTerminate();
  return 0;
}
