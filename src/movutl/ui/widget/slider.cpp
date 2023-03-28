#include <movutl/ui/widget/slider.hpp>

namespace mu::ui {

template <typename T> uiSliderH<T>::uiSliderH(const std::string& text_, T* value_, const Vec2& range_, const double delta_) {
  text                    = text_;
  value                   = value_;
  range                   = range_;
  delta                   = delta_;
  last_value              = *value_;
  flags.EnableAutoExpandX = true;
  flags.EnableAutoShrinkX = true;
}
template <typename T> void uiSliderH<T>::render() {
  if(!flags.Active) {
    needRendering(false);
    return;
  }
  auto wnd         = drawing_wnd();
  auto r           = wnd->get_renderer();
  const auto style = getStyle();

  if((*value != last_value) || flags.needRendering) {
    int pos_x = pos[0] + size[0] * (*value - range[0]) / (range[1] - range[0]);
    r->rectPS(pos, size, style->col_WidgetBg);
    r->rectPS(pos, size, style->col_WidgetLine);
    r->rectPS({pos_x, pos[1]}, {10, size[1]}, {200, 55, 200});
    char tt[50];
    sprintf(tt, "%s : %0.5f", text.c_str(), (float)(*value)); // typename Tがintのときとかは%fではなく%dでしょ。あと桁数指定しようよ
    const auto s = r->put_text(std::string(tt), pos + style->WidgetPadding, 1) + style->WidgetPadding * 2;
    last_value   = *value;
    if(s != outerSize) {
      impl_needCalcAlinment_parent();
      outerSize = s;
    }
    needRendering(false);
  }
}
template <typename T> bool uiSliderH<T>::CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, [[maybe_unused]] int num_1, [[maybe_unused]] int num_2, [[maybe_unused]] const char** strings) {
  T tmp_value = float((vec2_1[0] - pos[0]) * (range[1] - range[0])) / float(size[0]) + range[0];
  if(flag == uiCallbackFlags::LMouseDown) {
    *value = tmp_value;
  } else if(flag == uiCallbackFlags::MouseScroll) {
    *value += num_2 > 0 ? delta : -delta;
  } else if(flag == uiCallbackFlags::MouseMove) {
    if(num_2 & 0b01) {
      *value = tmp_value;
    }
  }
  *value = std::clamp<float>(*value, range[0], range[1]);
  return true;
}

// the explicit instanciation
template uiSliderH<double>::uiSliderH(const std::string&, double*, const Vec2&, const double);
template uiSliderH<float>::uiSliderH(const std::string&, float*, const Vec2&, const double);
template uiSliderH<int>::uiSliderH(const std::string&, int*, const Vec2&, const double);
template uiSliderH<unsigned long>::uiSliderH(const std::string&, unsigned long*, const Vec2&, const double);

} // namespace mu::ui
