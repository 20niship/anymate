#include <movutl/db/db.hpp>
#include <movutl/ui/ui.hpp>
#include <movutl/db/project.hpp>
#include <iostream>
#include <movutl/instance/instance.hpp>
#include <opencv2/opencv.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/colors.hpp>

using namespace mu::core;
using namespace mu::db;
using namespace mu::render;
using namespace mu::instance;

int main() {
  // camera
  Camera camera(Vec3(0.0f, 0.0f, 3.0f));
  /* Shader ourShader("anim_model.vs", "anim_model.fs"); */
#if 0
  Body body("resources/objects/vampire/dancing_vampire.dae");

  const char *fname = "test.mu";
  Project proj;

  mu::instance::init();
  auto wnd = mu::ui::create_window("hello", 640, 480);

  bool button_value = false;
  auto frame = mu::ui::frame("test", {
      mu::ui::button("hogehoge", &button_value),
      mu::ui::check("checkbox", &button_value),
      mu::ui::label("this is label"),
  });

  mu::db::World world;
  world.add(ourModel);

  wnd->draw(world);
  wnd->add(body);

  while(!wnd.should_close()) {
    const uint16_t time = wnd.getIO().time;
    animator.UpdateAnimation(time);

    wnd.draw(world);
    wnd.drawui();
    wnd.update();
  }

#endif
  glfwTerminate();
  return 0;
}

