
#pragma once
#include <movutl/core/rect.hpp>
#include <movutl/core/vector.hpp>
#include <movutl/ui/widget.hpp>

namespace mu::ui {

template <typename T> class uiRange2 : public uiWidget {
private:
  core::Range<T>* value;
  core::Range<T> last_value;
  Vec2 range;
  std::string text;
  float delta{1.0f};
  int selecting_idx{-1};
  int hovering_idx{-1};
  int start_mouse_pos_x;
  Vec2d _num_display_pos{0, 0};
  Range<T> start_mouse_val;
  // float last_value, min_value, max_value, delta_value;
public:
  uiRange2(std::string text_, Range<T>* value_, Vec2 range_, const double delta = 1.0);
  void render() override;
  bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings) override;
};


template <typename T> inline uiWidget* range2(const std::string text_, core::Range<T>* value_, const Vec2& range_, const double delta = 1.0) { return new uiRange2(text_, value_, range_, delta); }


} // namespace mu::ui
