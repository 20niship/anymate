#pragma once

#include <chrono>

#ifdef WITH_OPENGL // CmakeListsをみてね
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#else
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <movutl/core/defines.hpp>
#include <movutl/ui/enum.hpp>
#include <movutl/ui/key.hpp>

namespace mu::ui {
class uiWindow;
enum class uiCallbackFlags : uint16_t {
  OnHover     = 0,  // Widget上にマウスがHoverされたとき
  OffHover    = 1,  // Widget上のRectからマウスが離れた時（
  ValueChange = 2,  // 値が変更されたとき（Userによって？）
  ResizeTo    = 3,  //
  MouseMove   = 4,  // マウスが移動した時（HoverしているWidgetとFocusされたWidgetに送信される）
  LMouseDown  = 5,  // Left mouse down
  RMouseDown  = 6,  // Right mouse down
  CMouseDown  = 7,  // Center mouse down
  LMouseUP    = 8,  // Left mouse up
  RMouseUP    = 9,  // Right mouse up
  CMouseUP    = 10, // Center mouse up
  MouseScroll = 11, // マウスがスクロールされた（スクロール量はnumに入っている）
  DragDrop    = 12, // ファイルがD＆Dされた（numにファイル数、const char **stringsにファイル名のリスト）
  ShouldClose = 13, // ウィンドウが閉じられる直前 glfwShouldWindowCloseがtrueになった時
  Keyboard    = 14, // Keyboard input
  CharInput   = 15  // char input (keyboard?)
};

using InputTextFlags = uint64_t;
using InputTextFlags = uint64_t;
using uiWchar        = unsigned short;

enum class uiMouseButton: uint8_t { Unknown = 0xFF, Left = 0, Right, Middle, Undo, Redo, Count };


struct JoypadData {
  bool select : 1;      //!< セレクトボタン。
  bool l3 : 1;          //!< ハンドルスイッチ左。アナログもーどの時のみ有効。それ以外の時は常に1。
  bool r3 : 1;          //!< ハンドルスイッチ右。アナログモードの時のみ有効。それ以外の時は常に1。
  bool start : 1;       //!< スタートボタン。
  bool up : 1;          //!< 十字キー上。
  bool right : 1;       //!< 十字キー右。
  bool down : 1;        //!< 十字キー下。
  bool left : 1;        //!< 十字キー左。
  bool l2 : 1;          //!< L2。
  bool r2 : 1;          //!< R2。
  bool l1 : 1;          //!< L1。
  bool r1 : 1;          //!< R1。
  bool Y : 1;           //!< さんかくボタン。
  bool B : 1;           //!< まるボタン。
  bool A : 1;           //!< ばつボタン。
  bool X : 1;           //!< しかくボタン。
  std::int8_t rstick_y; //!< 右ハンドル左右方向。アナログモード時のみ有効。倒さない状況で0x80付近、上に倒しきると0x00、下に倒しきると0xFFとなる。
  std::int8_t rstick_x; //!< 右ハンドル上下方向。アナログモード時のみ有効。倒さない状況で0x80付近、左に倒しきると0x00、右に倒しきると0xFFとなる。
  std::int8_t lstick_y; //!< 左ハンドル左右方向。
  std::int8_t lstick_x; //!< 左ハンドル上下方向。
};

#define MU_KEYDATA_SIZE 400

class EditorIO {
public:
  GLFWwindow* glfw_wnd;
  uiWindow* wnd;

  struct IOConfig {
    float IniSavingRate;      // = 5.0f               // Maximum time between saving positions/sizes to .ini file, in seconds.
    float DoubleClickMaxDist; // = 6.0f               // Distance threshold to stay in to validate a double-click, in pixels.
    float DoubleClickMaxTime; ///  = 0.30f              // Time for a double-click, in seconds.
    float MouseDragThreshold; // = 6.0f               // Distance threshold before considering we are dragging

    float FontGlobalScale;     // = 1.0f               // Global scale all fonts
    bool FontAllowUserScaling; // = false              // Allow user scaling text of individual window with CTRL+Wheel.

    float KeyRepeatDelay; // = 0.250f             // When holding a key/button, time before it starts repeating, in seconds (for buttons in Repeat mode, etc.).
    float KeyRepeatRate;  // = 0.020f             // When holding a key/button, rate at which it repeats, in seconds.

    bool key_input_enabled;
    bool mouse_input_enabled;
    bool drop_event_enabled;
    bool resize_event_enabled;

    IOConfig();
  };
  IOConfig config;

  // --------   user access queues-------
  enum EventType { Event_None = 0, Event_MousePos, Event_MouseWheel, Event_MouseButton, Event_Key, Event_Text, Event_Focus, Event_COUNT, Event_User };
  struct IoInputEvent {
    struct InputEvtMouseBtn {
      uiMouseButton Button;
      bool Down;
    };
    struct InputEvtKey {
      muKey Key;
      bool Down;
      float AnalogValue;
    };

    EventType type;
    union {
      Vec2d mouse_pos;            // if Type == ImGuiInputEventType_MousePos
      Vec2d mouse_wheel;          // if Type == ImGuiInputEventType_MouseWheel
      InputEvtMouseBtn mouse_btn; // if Type == ImGuiInputEventType_MouseButton
      InputEvtKey key;            // if Type == ImGuiInputEventType_Key
      unsigned int text;          // if Type == ImGuiInputEventType_Text
      bool focused;               // if Type == ImGuiInputEventType_Focus
    };
    bool IgnoredAsSame;
    bool AddedByTestEngine;
    IoInputEvent() { /*memset(this, 0, sizeof(*this));*/
    }
    /* IoInputEvent(const InputEvtKey& key) { */
    /*   type = Type::Key; */
    /*   Key  = key; */
    /* } */
    /* IoInputEvent(const InputEvtMouseBtn& b) { */
    /*   type        = Type::MouseButton; */
    /*   MouseButton = b; */
    /* } */
  };
  core::Vec<IoInputEvent> InputEventsQueue; // Input events which will be tricked/written into IO structure.
  core::Vec<IoInputEvent> InputEventsTrail; // Past input events processed in NewFrame(). This is to allow domain-specific application to access e.g mouse/pen trail.

  // --------   window formats -------
  using TimeStamp = std::chrono::system_clock::time_point;
  TimeStamp start_time;      /// time on system startup
  TimeStamp last_frame_time; /// time on last api_update() called
  float fps;                 // Framerate estimation, in frame per second. Rolling average estimation based on IO.DeltaTime over 120 frames

  // --------   mouse -------
  struct uiMouseButtonData {
    bool pressing;     /// true if currently clicked
    int clicked_count; /// 0=クリックされていない、1=クリックされた、2＝ダブルクリックされた
    TimeStamp start_time;
    Vec2d start_pos; /// クリックが開始された時に押した場所（ドラッグする時などに使用）
  };
  uiMouseButtonData button[10];

  Vec2d mouse_wheel;
  Vec2d mouse_pos, last_clicked_pos, mouse_pos_prev;
  JoypadData joypad;
  float PenPressure; /// touchpen pressure 0.0 to 1.0

  // --------   keyboard -------
  struct uiKeyData {
    bool down; /// key is pressed
    TimeStamp press_start_time;
    uint8_t pressure; /// joypad press pressure
  };
  uiKeyData keys[MU_KEYDATA_SIZE]; /// Key state for all keys. Use IsKeyXXX() functions to access this.

  // --------   Clipboard -------
  const char* (*GetClipboardTextFn)(void* user_data);
  void (*SetClipboardTextFn)(void* user_data, const char* text);
  void* ClipboardUserData;

  bool focused;                  // Only modify via AddFocusEvent()
  uiWchar InputCharacters[1024]; // List of characters input (translated by user from keypress+keyboard state). Fill using AddInputCharacter() helper.


  EditorIO();
  ~EditorIO();
  void set_window(GLFWwindow* w);
  [[deprecated]] void enable_keyinput(bool);
  [[deprecated]] void enable_mouse_input(bool);
  [[deprecated]] void enable_resize_input(bool);
  [[deprecated]] void enable_drop_input(bool);

  void api_add_input_char(uiWchar c);
  void api_add_input_char_utf8(const char* utf8);
  void api_add_input_char_utf16(uiWchar* c);
  void api_clear_input_char() { InputCharacters[0] = 0; }
  void api_add_key_event(const muKey key, bool down);
  void api_add_analog_key_event(const muKey key, bool down, float v);
  void api_add_mouse_pos_event(float x, float y);
  void api_add_mouse_button_event(const uiMouseButton button, bool down);
  void api_add_mouse_wheel_event(float wh_x, float wh_y);
  void api_add_focus_event(bool focused);

  void api_push_event_to_window()const;

  void api_update_mod_keys(); /// check alt, ctrl, super, tag keys are pressed

  void api_update(); // new frame (update uptime, fps, etc.....)
  double get_uptime_ms() const;


  bool is_mouse_left_pressing()const{
    return button[(uint8_t)uiMouseButton::Left].pressing;
  }
  bool is_mouse_right_pressing()const{
    return button[(uint8_t)uiMouseButton::Right].pressing;
  }
  bool is_mouse_middle_pressing()const{
    return button[(uint8_t)uiMouseButton::Middle].pressing;
  }
  bool is_key_pressing(const muKey)const;
};

MU_API void setup_glfw_callbacks(GLFWwindow* wnd);

MU_API void WindowFocusCallback(GLFWwindow* window, int focused);
MU_API void CursorEnterCallback(GLFWwindow* window, int entered);
MU_API void CursorPosCallback(GLFWwindow* window, double x, double y);
MU_API void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
MU_API void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
MU_API void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
MU_API void CharCallback(GLFWwindow* window, unsigned int c);
MU_API void MonitorCallback(GLFWmonitor* monitor, int event);

} // namespace mu::ui
