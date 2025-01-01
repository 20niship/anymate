#include <chrono>
#include <movutl/instance/instance.hpp>
#include <movutl/ui/io.hpp>

namespace mu::ui {

void ErrorCallback(int code, const char* description) {
  LOGE << "---- GLFW Error Callback --------";
  std::cout << "code = " << code << std::endl;
  std::cout << "description = \n" << description << std::endl;
}


void setup_glfw_callbacks(GLFWwindow* window) {
  // MU_ASSERT(window != nullptr);
  if(window) {
    glfwSetWindowFocusCallback(window, WindowFocusCallback);
    glfwSetCursorEnterCallback(window, CursorEnterCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCharCallback(window, CharCallback);
  }
  glfwSetMonitorCallback(MonitorCallback);
  glfwSetErrorCallback(ErrorCallback);
}


void WindowFocusCallback(GLFWwindow* window, int focused) {
  auto io = instance::get_io();
  if(window != nullptr) {
    // TODO: 特定のWindowにフォーカスさせる
  }
  io->api_add_focus_event(focused != 0);
}

uiMouseButton mouse_button_glfw2mu(int b) {
  switch(b) {
    case GLFW_MOUSE_BUTTON_1: return uiMouseButton::Left;
    case GLFW_MOUSE_BUTTON_2: return uiMouseButton::Right;
    case GLFW_MOUSE_BUTTON_3: return uiMouseButton::Middle;
    case GLFW_MOUSE_BUTTON_4: return uiMouseButton::Count;
    case GLFW_MOUSE_BUTTON_5: return uiMouseButton::Undo;
    case GLFW_MOUSE_BUTTON_6: return uiMouseButton::Redo;
  };
  return uiMouseButton::Unknown;
}

void MouseButtonCallback([[maybe_unused]] GLFWwindow* window, int button, int action, int mods) {
  auto io = instance::get_io();
  if(!(mods & GLFW_MOD_CONTROL)) io->keys[MU_KEY_RIGHT_CONTROL].down = io->keys[MU_KEY_LEFT_CONTROL].down = false;
  if(!(mods & GLFW_MOD_SHIFT)) io->keys[MU_KEY_RIGHT_SHIFT].down = io->keys[MU_KEY_LEFT_SHIFT].down = false;
  if(!(mods & GLFW_MOD_ALT)) io->keys[MU_KEY_RIGHT_ALT].down = io->keys[MU_KEY_LEFT_ALT].down = false;
  if(!(mods & GLFW_MOD_SUPER)) io->keys[MU_KEY_RIGHT_SUPER].down = io->keys[MU_KEY_LEFT_SUPER].down = false;

  constexpr int MouseBtnCount = 5;
  if(button >= 0 && button < MouseBtnCount) io->api_add_mouse_button_event(mouse_button_glfw2mu(button), action == GLFW_PRESS);
}

void ScrollCallback([[maybe_unused]] GLFWwindow* window, double xoffset, double yoffset) {
  auto io = instance::get_io();
  io->api_add_mouse_wheel_event(xoffset, yoffset);
}

void CursorPosCallback([[maybe_unused]] GLFWwindow* window, double x, double y) {
  auto io = instance::get_io();
  io->api_add_mouse_pos_event(x, y);
}

void CursorEnterCallback(GLFWwindow* window, int entered) {
  auto io = instance::get_io();
  io->api_add_focus_event(entered);
  if(entered) {
    drawing_wnd(instance::get_window(window));
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    io->api_add_mouse_pos_event(x, y);
  }
}

void CharCallback([[maybe_unused]] GLFWwindow* window, unsigned int c) {
  auto io = instance::get_io();
  io->api_add_input_char(c);
}

void MonitorCallback(GLFWmonitor*, int) { LOGE << "Monitor callback not implemented!"; }
const char* GetClipboardText(void* user_data) { return glfwGetClipboardString((GLFWwindow*)user_data); }
void SetClipboardText(void* user_data, const char* text) { glfwSetClipboardString((GLFWwindow*)user_data, text); }


void KeyCallback([[maybe_unused]] GLFWwindow* window, int keycode, int, int action, int mods) {
  if(action != GLFW_PRESS && action != GLFW_RELEASE) return;
  auto io = instance::get_io();
  if(!(mods & GLFW_MOD_CONTROL)) io->keys[MU_KEY_RIGHT_CONTROL].down = io->keys[MU_KEY_LEFT_CONTROL].down = false;
  if(!(mods & GLFW_MOD_SHIFT)) io->keys[MU_KEY_RIGHT_SHIFT].down = io->keys[MU_KEY_LEFT_SHIFT].down = false;
  if(!(mods & GLFW_MOD_ALT)) io->keys[MU_KEY_RIGHT_ALT].down = io->keys[MU_KEY_LEFT_ALT].down = false;
  if(!(mods & GLFW_MOD_SUPER)) io->keys[MU_KEY_RIGHT_SUPER].down = io->keys[MU_KEY_LEFT_SUPER].down = false;

  io->api_add_key_event((muKey)keycode, (action == GLFW_PRESS));
}

void EditorIO::api_add_input_char(const uiWchar c) {
  LOGE << "TODO!!!" << c;
  api_push_event_to_window();
}

void EditorIO::api_add_key_event(const muKey key, bool down) { api_add_analog_key_event(key, down, down ? 1.0 : 0.0); }
void EditorIO::api_add_analog_key_event(const muKey key, bool down, float v) {
  IoInputEvent e;
  e.type            = Event_Key;
  e.key.AnalogValue = v;
  e.key.Key         = key;
  e.key.Down        = down;
  InputEventsQueue.push_back(e);

  if(down) keys[key].press_start_time = last_frame_time;
  keys[key].pressure = v;
  keys[key].down     = down;
  api_push_event_to_window();
}

#define GET_CUR_TIME() (std::chrono::system_clock::now())
#define GET_DURATION_MS(END, START) (static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>((END) - (START)).count() / 1000.0))

void EditorIO::api_add_mouse_pos_event(float x, float y) {
  IoInputEvent e;
  e.type      = Event_MousePos;
  e.mouse_pos = Vec2d(x, y);
  InputEventsQueue.push_back(e);

  mouse_pos_prev = mouse_pos;
  mouse_pos      = Vec2d(x, y);
  api_push_event_to_window();
}

void EditorIO::api_add_mouse_button_event(const uiMouseButton btn, bool down) {
  if(btn  == uiMouseButton::Unknown){
    spdlog::warn("unknown mouse event occured : api_add_mouse_button_event event = {}, down = ", (int)btn, down);
return;
  } 

  IoInputEvent e;
  e.type             = Event_MouseButton;
  e.mouse_btn.Button = btn;
  e.mouse_btn.Down   = down;
  InputEventsQueue.push_back(e);

  button[(int)btn].pressing = down;

  if(!down) {
    api_push_event_to_window();
  } else {
    const auto now = GET_CUR_TIME();
    const auto dur = GET_DURATION_MS(now, button[(int)btn].start_time);

    if(button[(int)btn].clicked_count == 0 || dur > config.DoubleClickMaxTime * button[int(btn)].clicked_count) {
      button[(int)btn].start_time    = now;
      button[(int)btn].clicked_count = 1;
      button[(int)btn].start_pos     = mouse_pos;
    } else {
      const bool is_double_click = GET_DURATION_MS(button[(int)btn].start_time, now) < config.DoubleClickMaxTime;
      if(down && is_double_click) button[(int)btn].clicked_count += down;
    }
  }
  api_push_event_to_window();
}

void EditorIO::api_add_mouse_wheel_event(float wh_x, float wh_y) {
  mouse_wheel = Vec2d(wh_x, wh_y);

  IoInputEvent e;
  e.type        = Event_MouseWheel;
  e.mouse_wheel = Vec2d(wh_x, wh_y);
  InputEventsQueue.push_back(e);
  api_push_event_to_window();
}

void EditorIO::api_add_focus_event(bool v) {
  focused = v;
  IoInputEvent e;
  e.type    = Event_Focus;
  e.focused = v;
  InputEventsQueue.push_back(e);
  api_push_event_to_window();
}

EditorIO::EditorIO() {
  start_time = GET_CUR_TIME();
  // setup_glfw_callbacks();
}

double EditorIO::get_uptime_ms() const { return GET_DURATION_MS(last_frame_time, start_time); }
EditorIO::~EditorIO() {}

void EditorIO::api_update() {
  const auto now  = GET_CUR_TIME();
  double dt       = GET_DURATION_MS(now, last_frame_time);
  fps             = 1000.0f / dt;
  last_frame_time = now;
}

void EditorIO::api_push_event_to_window() const {
  auto w = drawing_wnd();
  if(w == nullptr) return;
  w->process_event();
}

EditorIO::IOConfig::IOConfig() {
  DoubleClickMaxTime = 300;
  MouseDragThreshold = 6.0;
  KeyRepeatDelay     = 0.25;
  KeyRepeatRate      = 0.02;

  key_input_enabled    = true;
  mouse_input_enabled  = true;
  drop_event_enabled   = true;
  resize_event_enabled = true;
}

bool EditorIO::is_key_pressing(const muKey key) const {
  auto w = drawing_wnd();
  return glfwGetKey(w->getGLFWwindow(), key) == GLFW_PRESS;
}

#if 0
void setupCursor() {
  GLFWerrorfun prev_error_callback             = glfwSetErrorCallback(NULL);
  bd->MouseCursors[ImGuiMouseCursor_Arrow]     = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  bd->MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
  bd->MouseCursors[ImGuiMouseCursor_ResizeNS]  = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
  bd->MouseCursors[ImGuiMouseCursor_ResizeEW]  = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
  bd->MouseCursors[ImGuiMouseCursor_Hand]      = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
#if GLFW_HAS_NEW_CURSORS
  bd->MouseCursors[ImGuiMouseCursor_ResizeAll]  = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
  bd->MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
  bd->MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
  bd->MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
#else
  bd->MouseCursors[ImGuiMouseCursor_ResizeAll]  = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  bd->MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  bd->MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  bd->MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
#endif
  glfwSetErrorCallback(prev_error_callback);

  // Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
  if(install_callbacks) InstallCallbacks(window);

  bd->ClientApi = client_api;
  return true;
}

void destroyCursor() {
  for(ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++) glfwDestroyCursor(bd->MouseCursors[cursor_n]);
}

static void UpdateMouseData() {
  Data* bd    = GetBackendData();
  ImGuiIO& io = ImGui::GetIO();

  if(glfwGetInputMode(bd->Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
    io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
    return;
  }
  const bool is_app_focused = glfwGetWindowAttrib(bd->Window, GLFW_FOCUSED) != 0;
  if(is_app_focused) {
    // (Optional) Set OS mouse position from Dear ImGui if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
    if(io.WantSetMousePos) glfwSetCursorPos(bd->Window, (double)io.MousePos.x, (double)io.MousePos.y);

    // (Optional) Fallback to provide mouse position when focused (CursorPosCallback already provides this when hovered or captured)
    if(is_app_focused && bd->MouseWindow == NULL) {
      double mouse_x, mouse_y;
      glfwGetCursorPos(bd->Window, &mouse_x, &mouse_y);
      io.AddMousePosEvent((float)mouse_x, (float)mouse_y);
      bd->LastValidMousePos = ImVec2((float)mouse_x, (float)mouse_y);
    }
  }
}

static void UpdateMouseCursor() {
  ImGuiIO& io = ImGui::GetIO();
  Data* bd    = GetBackendData();
  if((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(bd->Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) return;

  ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
  if(imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor) {
    // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
    glfwSetInputMode(bd->Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  } else {
    // Show OS mouse cursor
    // FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
    glfwSetCursor(bd->Window, bd->MouseCursors[imgui_cursor] ? bd->MouseCursors[imgui_cursor] : bd->MouseCursors[ImGuiMouseCursor_Arrow]);
    glfwSetInputMode(bd->Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

// Update gamepad inputs
static inline float Saturate(float v) { return v < 0.0f ? 0.0f : v > 1.0f ? 1.0f : v; }

void NewFrame() {
  ImGuiIO& io = ImGui::GetIO();
  Data* bd    = GetBackendData();
  IM_ASSERT(bd != NULL && "Did you call InitForXXX()?");

  // Setup display size (every frame to accommodate for window resizing)
  int w, h;
  int display_w, display_h;
  glfwGetWindowSize(bd->Window, &w, &h);
  glfwGetFramebufferSize(bd->Window, &display_w, &display_h);
  io.DisplaySize = ImVec2((float)w, (float)h);
  if(w > 0 && h > 0) io.DisplayFramebufferScale = ImVec2((float)display_w / (float)w, (float)display_h / (float)h);

  // Setup time step
  double current_time = glfwGetTime();
  io.DeltaTime        = bd->Time > 0.0 ? (float)(current_time - bd->Time) : (float)(1.0f / 60.0f);
  bd->Time            = current_time;

  UpdateMouseData();
  UpdateMouseCursor();
  UpdateGamepads();
}
#endif

} // namespace mu::ui
