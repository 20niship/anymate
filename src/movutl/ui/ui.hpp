#pragma once

#ifdef WITH_OPENGL // CmakeListsをみてね
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#else
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <movutl/db/camera.hpp>
#include <movutl/render/engine.hpp>
#include <movutl/render/render.hpp>
#include <movutl/ui/io.hpp>
#include <movutl/ui/widget.hpp>
#include <movutl/ui/window.hpp>


namespace mu::ui {

using uihWnd    = uiWindow*;
using uihWidget = uiWidget*;

// keyboardCBで呼ばれる関数
// keycoard, scancode, action, mods, position

struct Editor {
  Vec<uihWnd> windows;
  uiWindow *drawingWnd, *hoveringWnd, *focusedWnd;
  uiStyle style;
  void init();
  bool update(const bool verbose);
  void terminate(const bool verbose = false);
};

#define MU_API

MU_API Editor* get_editor_ptr();
inline MU_API auto drawing_wnd(uiWindow* wnd) { return get_editor_ptr()->drawingWnd = wnd; }
inline MU_API auto drawing_wnd() { return get_editor_ptr()->drawingWnd; }
inline MU_API auto focused_wnd(uiWindow* wnd) { return get_editor_ptr()->focusedWnd = wnd; }
inline MU_API auto focused_wnd() { return get_editor_ptr()->focusedWnd; }
inline MU_API auto hover_wnd(uiWindow* wnd) { return get_editor_ptr()->hoveringWnd = wnd; }
inline MU_API auto hover_wnd() { return get_editor_ptr()->hoveringWnd; }

// inline MU_API auto setFocusedWidget(uiWidget *w){return get_editor_ptr()->focusedWidget = w;}
inline MU_API uiWidget* focused_widget() { return drawing_wnd()->root_widget_ui.getFocusedWidget(); }
inline MU_API uiWidget* hover_widget() { return drawing_wnd()->root_widget_ui.getHoveringWidget(); }

inline MU_API void initFinish() {
  assert(get_editor_ptr()->windows.size() > 0);
  /* render::engine.renderer.init(); */
}
inline MU_API uiWindow* create_window(std::string name, int w, int h) {
  auto wnd = new uiWindow(name, w, h);
  get_editor_ptr()->windows.push_back(std::move(wnd));
  return wnd;
}
// styles
inline MU_API const uiStyle* getStyle() { return &get_editor_ptr()->style; }
inline MU_API void setStyle(const uiStyle& s) { get_editor_ptr()->style = s; }
inline MU_API auto getAllWindows() { return &get_editor_ptr()->windows; }
inline MU_API bool update(bool verbose = false) { return get_editor_ptr()->update(verbose); }

} // namespace mu::ui
