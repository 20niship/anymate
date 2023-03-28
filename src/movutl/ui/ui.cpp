#include <movutl/ui/ui.hpp>
#include <movutl/instance/instance.hpp> 

namespace mu::ui {

Editor editor;
Editor* get_editor_ptr() { return &editor; }

void Editor::init() {

  if(!glfwInit()) {
    std::cerr << "ERROR: could not start GLFW3\n";
    return;
  }
#ifdef WITH_OPENGL
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#else
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
}

bool Editor::update(bool) {
  assert(windows.size() > 0);
  glfwPollEvents();
  for(size_t i = 0; i < windows.size(); i++) windows[i]->drawFrame();
  instance::get_io()->api_update();
  /* for(auto &w : windows) w->drawFrame(verbose); */ // TODO: これでうまくいかん時がある？
#if 0
    (*renderer.get_device_ptr())->waitIdle();
#endif
  return !windows[0]->should_close();
}

void Editor::terminate(const bool verbose) {
  if(verbose) {
    std::cout << "terminating vkui......." << std::endl;
  }
  glfwTerminate();
  if(verbose) {
    std::cout << "deleting windows ........" << std::endl;
  }
  for(auto&& w : windows) w->terminate();
  if(verbose) {
    std::cout << "deleting windows ........" << std::endl;
  }
  return;
}
} // namespace mu::ui
