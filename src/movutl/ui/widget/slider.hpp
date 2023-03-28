#pragma once

#include <movutl/ui/ui.hpp>
#include <movutl/ui/widget.hpp>
namespace mu::ui {
template <typename T> class uiSliderH : public uiWidget {
private:
  T* value;
  T last_value;
  Vec2 range;
  std::string text;
  float delta{1.0f};
  // float last_value, min_value, max_value, delta_value;
public:
  uiSliderH(const std::string& text_, T* value_, const Vec2& range_, const double delta_ = 1.0);
  void render() override;
  bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings) override;
};

template <typename T> inline uiWidget* slider(const std::string text_, T* v, const Vec2& r) { return (new uiSliderH<T>(text_, v, r)); }
template <typename T> inline uiWidget* slider(const std::string text_, T* v, const Vec2& r, const double delta) { return (new uiSliderH<T>(text_, v, r, delta)); }

} // namespace mu::ui
