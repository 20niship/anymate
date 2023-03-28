#include <chrono>
#include <functional>
#include <glm/fwd.hpp>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>


#ifdef DEFINED_STD_FORMAT
#include <format>
#else
#include <movutl/core/logger.hpp>
#include <movutl/instance/instance.hpp>
#include <movutl/ui/ui.hpp>
#include <movutl/ui/widget.hpp>
#include <movutl/ui/window.hpp>


template <typename... Args> std::string myFormat(const std::string& base_str, const Args... args) {
  std::string base_str_ = base_str;
  /* constexpr std::size_t size = sizeof...(Args); */
  auto tt = std::make_tuple(args...);
  std::apply([&](auto&&... args_) { ((base_str_.replace(base_str_.find("{}") != std::string::npos ? base_str_.find("{}") : 0, base_str_.find("{}") == std::string::npos ? 0 : 2, std::to_string(args_))), ...); }, tt);
  return base_str_;
}
#endif

namespace mu::ui {


uiWindow::uiWindow(const std::string& _name, uint16_t width, uint16_t height) {
  setup_glfw_callbacks(nullptr);
  window = glfwCreateWindow(width, height, _name.c_str(), nullptr, nullptr);
  if(!window) {
    spdlog::critical("INITIALIZER: Failed to create window!");
    glfwTerminate();
    exit(0);
    return;
  }

  render.createSurface(window);
  render.init();

  int ws[2];
  glfwGetWindowSize(window, &ws[0], &ws[1]);
  size = {ws[0], ws[1]};

  MU_ASSERT(window != nullptr);
  glfwSetWindowUserPointer(window, this);
  setup_glfw_callbacks(window);
  currentFrame = 0;
  updateUniformBuffer();
  draw_ui();
  cursors.init();
  root_widget_ui.needRendering(true);
  root_widget_ui.impl_needCalcAlignment_child();
  root_widget_ui.impl_needCalcInnerSize_parent();
}


void uiWindow::draw_ui() {
  drawing_wnd(this);
  /* renderer.updateVertexBuffer(); */
  root_widget_ui.calcInnerSize_recursize();
  root_widget_ui.applyAlignment_recursive();
  // if(root_widget_ui.getNeedRendering()){
  root_widget_ui.needRendering(true);

  /* render.clear(); */
  root_widget.render();
  root_widget_ui.render_child_widget();

#if 0
  if(dd.get_n_vertices_ui() == 0) {
    dd.add({0, 0}, {0, 0, 0}, {0, 0});
    dd.add({0, 0}, {0, 0, 0}, {0, 0});
    dd.add({0, 0}, {0, 0, 0}, {0, 0});
  }
#endif
}

void uiWindow::updateUniformBuffer() {
#if 0
  int ww, wh;
  glfwGetWindowSize(window, &ww, &wh);
  /* render.camera.getCameraProjMatrix(render.uniform_data.proj); */
    const float projectionMatrix[] = { // 左下原点
        2.0f / float(ww), 0.0f, 0.0f, 0.0f ,
        0.0f, -2.0f / float(wh), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };
  const float projectionMatrix[] = {2.0f / float(ww), 0.0f, 0.0f, 0.0f, 0.0f, 2.0f / float(wh), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f};
  for(int i = 0; i < 16; i++) {
    render.uniform_data.proj_uv[i] = projectionMatrix[i];
  }
  const auto text_renderer           = instance::get_text_renderer();
  render.uniform_data.texure_size[0] = text_renderer->TexWidth;
  render.uniform_data.texure_size[1] = text_renderer->TexHeight;
#endif
}

void uiWindow::fill(const Vec3b& col) {
  MU_ASSERT(size[0] > 0);
  MU_ASSERT(size[1] > 0);
  render.rectPS({1, 1}, {size[0], size[1]}, col);
}

void uiWindow::drawDevelopperHelps() {
  const auto nCmd       = render.drawlist.size();
  const auto wsize      = root_widget_ui.get_widget_num();
  const auto v_size     = render.get_n_vertices();
  const auto v_size_ui  = render.get_n_vertices_ui();
  const auto fps        = instance::get_io()->fps;
  const std::string str = myFormat("vert_size = (3d = {}, ui = {}), drawCmd={} nWidget={}, fps={}", v_size, v_size_ui, nCmd, wsize, (int)fps);
  const auto focused    = root_widget_ui.getFocusedWidget();
  const auto hovering   = root_widget_ui.getHoveringWidget();
  render.put_text(str, {10, 10}, 1, {255, 0, 255});
  render.rectPS(hovering->getPos(), hovering->getSize(), {0, 255, 0}, 2);
  render.rectPS(focused->getPos(), focused->getSize(), {255, 0, 0}, 1);
}
void uiWindow::drawDevFontTexture() {
  auto fw  = instance::get_text_texture()->width();
  auto fh  = instance::get_text_texture()->width();
  Vec2d p0 = {30, 30};
  Vec2d p2 = {size[0] - 30, size[1] - 30};
  Vec2d p1 = {p0[0], p2[1]};
  Vec2d p3 = {p2[0], p0[1]};

  Vec2d uv0 = {0, 0};
  Vec2d uv1 = {0, fh};
  Vec2d uv2 = {fw, fh};
  Vec2d uv3 = {fw, 0};

  render.quad(p0, p1, p2, p3, uv0, uv1, uv2, uv3, {255, 255, 255});
}

void uiWindow::drawFrame() {
  drawing_wnd(this);
  updateUniformBuffer();
  render.push();
  render.sort();
  render.draw();
  render.clear_drawlist();
  if(render.mesh_col != nullptr) render.mesh_col->clear();
  if(render.mesh_ui != nullptr) render.mesh_ui->clear();
  if(render.mesh_3d != nullptr) render.mesh_3d->clear();
  glfwSwapBuffers(window);
  currentFrame++;
}

void uiWindow::init() {
}

void uiWindow::terminate() {
  glfwDestroyWindow(window);
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void uiWindow::process_event() {
  const auto io = instance::get_io();
  if(io->InputEventsQueue.size() == 0) return;

  const auto last_evt      = io->InputEventsQueue.end();
  const auto last_evt_type = last_evt->type;

  using enum ui::EditorIO::EventType;

  switch(last_evt_type) {
    case Event_MouseWheel: {
      const double xoffset = last_evt->mouse_wheel[0];
      const double yoffset = last_evt->mouse_wheel[1];
      render.camera.go_closer(yoffset);
      root_widget_ui.CallbackFunc(uiCallbackFlags::MouseScroll, {0, 0}, xoffset, yoffset, nullptr);
      break;
    }

    case Event_MousePos: {
      static bool wnd_rotating = false;
      static double radius;
      static Vec2d rotation, start_pos_mouse;
      if(io->button[(int)uiMouseButton::Middle].pressing) {
        const auto vecc = render.camera.pos - render.camera.dir;
        if(!wnd_rotating) {
          radius          = vecc.norm();
          rotation[0]     = std::acos(vecc[2] / radius);  // theta
          rotation[1]     = std::atan2(vecc[1], vecc[0]); // phi
          start_pos_mouse = io->mouse_pos;
        }
        const auto delta   = io->mouse_pos - start_pos_mouse;
        const double theta = rotation[0] + (float)delta[1] / 250;
        const double phi   = rotation[1] + (float)delta[0] / 250;
        const Vec3 tmp     = {radius * std::sin(theta) * std::cos(phi), radius * std::sin(theta) * std::sin(phi), radius * std::cos(theta)};
        render.camera.pos  = tmp + render.camera.dir;
        wnd_rotating       = true;
      } else if(wnd_rotating) {
        wnd_rotating = false;
      }

      int state_ = 0;
      state_ |= io->button[(int)uiMouseButton::Left].pressing;
      state_ |= io->button[(int)uiMouseButton::Right].pressing << 1;
      state_ |= io->button[(int)uiMouseButton::Middle].pressing << 2;
      root_widget_ui.CallbackFunc(uiCallbackFlags::MouseMove, io->mouse_pos, 0, state_, nullptr);
      break;
    }

    case Event_COUNT: {
      break;
    }
    case Event_Focus: {
      break;
    }
    case Event_Key: {
      const auto key  = last_evt->key;
      const bool down = key.Down;
      // if (key == GLFW_KEY_W && action == GLFW_PRESS){ render.camera.pos[2]+= 0.1; }
      // if (key == GLFW_KEY_S && action == GLFW_PRESS){ render.camera.pos[2]-= 0.1; }
      // if (key == GLFW_KEY_A && action == GLFW_PRESS){ render.camera.pos[0]+= 0.1; }
      // if (key == GLFW_KEY_D && action == GLFW_PRESS){ render.camera.pos[0]-= 0.1; }
      // if (key == GLFW_KEY_Q && action == GLFW_PRESS){ render.camera.pos[1]+= 0.1; }
      // if (key == GLFW_KEY_E && action == GLFW_PRESS){ render.camera.pos[1]-= 0.1; }
      root_widget_ui.CallbackFunc(uiCallbackFlags::Keyboard, io->mouse_pos, key.Key, key.Down, nullptr);
      if(down && (key.Key == MU_KEY_ESCAPE || key.Key == MU_KEY_Q)) {
        should_close(true);
      }
      break;
    }
    case Event_Text: {
#if 0
      const uint16_t codepoint = (uint16_t)last_evt->key;
      std::cout << codepoint << std::endl;
      const double scale_ = (render.camera.pos - render.camera.dir).norm();
      if(codepoint == 'w') {
        render.camera.dir[2] += scale_ * 0.01;
      }
      if(codepoint == 's') {
        render.camera.dir[2] -= scale_ * 0.01;
      }
      if(codepoint == 'a') {
        render.camera.dir[0] += scale_ * 0.01;
      }
      if(codepoint == 'd') {
        render.camera.dir[0] -= scale_ * 0.01;
      }
      if(codepoint == 'q') {
        render.camera.dir[1] += scale_ * 0.01;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
      }
      if(codepoint == 'e') {
        render.camera.dir[1] -= scale_ * 0.01;
      }
      root_widget_ui.CallbackFunc(uiCallbackFlags::CharInput, {0, 0}, codepoint, 0, nullptr);
#endif
      break;
    }


    case Event_MouseButton: {
      const auto btn  = last_evt->mouse_btn;
      const bool down = btn.Down;

      using enum uiCallbackFlags;
      using enum uiMouseButton;
      switch(btn.Button) {
        case Left: root_widget_ui.CallbackFunc(down ? LMouseDown : LMouseUP, io->mouse_pos, 0, 0, nullptr); break;
        case Right: root_widget_ui.CallbackFunc(down ? RMouseDown : RMouseUP, io->mouse_pos, 0, 0, nullptr); break;
        case Middle: root_widget_ui.CallbackFunc(down ? CMouseDown : CMouseUP, io->mouse_pos, 0, 0, nullptr); break;
        case Count:
        case Undo:
        case Redo:
        case Unknown: LOGE << " Event_MouseButton callback Not Implemented!"; break;
      }
      break;
    }

    case Event_User:
    case Event_None: LOGE << "Not implemented Event!";
  }
}


} // namespace mu::ui
