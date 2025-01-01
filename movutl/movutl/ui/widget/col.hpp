#pragma once
#include <movutl/ui/widget.hpp>

namespace mu::ui {

class uiCol : public uiWidget {
private:
  Vec3b* value;
  Vec3b last_value;
  std::string text;
  int selecting_idx{-1};
  int start_mouse_pos_x;
  Vec2d _num_display_pos{0, 0};
  Vec3 start_mouse_val;
  // float last_value, min_value, max_value, delta_value;
public:
  uiCol(const std::string& text_, Vec3b* value_);
  void render() override;
  bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings) override;
};

class uiCol2 : public uiWidget {
private:
  Vec3b* value;
  Vec3 hsv;
  Vec3b last_value;
  std::string text;
  //int selecting_idx{-1};
  Vec2d _num_display_pos{0, 0};
  Vec3 start_mouse_val;
  // float last_value, min_value, max_value, delta_value;
public:
  uiCol2(const std::string& text_, Vec3b* value_);
  void render() override;
  bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings) override;
};


inline uiWidget* Col(const std::string& text_, Vec3b* c) { return (new uiCol(text_, c)); }
inline uiWidget* Col2(const std::string& text_, Vec3b* c) { return (new uiCol2(text_, c)); }

} // namespace mu::ui
