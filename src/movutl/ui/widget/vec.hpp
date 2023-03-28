#pragma once
#include <movutl/core/vector.hpp>
#include <movutl/ui/widget.hpp>

namespace mu::ui {

template <typename T> class uiVec2 : public uiWidget {
private:
  _Vec<T, 2>* value;
  _Vec<T, 2> last_value;
  Vec2 range;
  std::string text;
  float delta{1.0f};
  int selecting_idx{-1};
  int hovering_idx{-1};
  int start_mouse_pos_x;
  Vec2d _num_display_pos{0, 0};
  _Vec<T, 2> start_mouse_val;
  // float last_value, min_value, max_value, delta_value;
public:
  uiVec2(const std::string &text_, _Vec<T, 2>* value_, const Vec2 &range_, const double delta = 1.0);
  void render() override;
  bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings) override;
};

template <typename T> class uiVec3 : public uiWidget {
private:
  _Vec<T, 3>* value;
  _Vec<T, 3> last_value;
  Vec2 range;
  std::string text;
  float delta{1.0f};
  int selecting_idx{-1};
  int hovering_idx{-1};
  int start_mouse_pos_x;
  Vec2d _num_display_pos{0, 0};
  _Vec<T, 3> start_mouse_val;
  // float last_value, min_value, max_value, delta_value;
public:
  uiVec3(std::string text_, _Vec<T, 3>* value_, Vec2 range_, const double delta = 1.0);
  void render() override;
  bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings) override;
};

template<typename T>
inline uiWidget *uivec2(const std::string text_, _Vec<T, 2>*value_, const Vec2 &range_, const double delta=1.0){
  return new uiVec2<T>(text_, value_, range_, delta);
}

template<typename T>
inline uiWidget *uivec3(const std::string text_, _Vec<T, 3>*value_, const Vec2 &range_, const double delta=1.0){
  return new uiVec3<T>(text_, value_, range_, delta);
}


} // namespace mu::ui
