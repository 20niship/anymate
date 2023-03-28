#include <movutl/ui/ui.hpp>
#include <movutl/ui/widget.hpp>
#include <movutl/ui/widget/range2.hpp>
#include <sstream>

namespace mu::ui {
using namespace core;

template <typename T> uiRange2<T>::uiRange2(std::string text_, Range<T>* value_, Vec2 range_, const double delta_) {
  text       = text_;
  value      = value_;
  range      = range_;
  delta      = delta_;
  last_value = *value_;

  flags.EnableAutoExpandX = true;
  flags.EnableAutoShrinkX = true;
}

template <typename T> void uiRange2<T>::render() {
  if(!flags.Active) {
    needRendering(false);
    std::cout << "Not visivle or not active widget\n";
    return;
  }

  auto wnd         = drawing_wnd();
  auto r           = wnd->get_renderer();
  const auto style = getStyle();
  if((*value != last_value) || (flags.needRendering)) {
    const auto ss = r->put_text(text, pos + Vec2d{0, style->WidgetPadding[0]}, 1);
    const Vec2d pos2(pos[0] + ss[0] + style->WidgetMargin[0], pos[1]);
    const Vec2d each_size((size[0] - pos2[0] + pos[0]) / 2 - 1, size[1]);
    _num_display_pos = pos2;
    if(each_size[0] < 0) return;
    Vec2d new_outer_size = ss;
    for(int i = 0; i < 2; i++) {
      const Vec2d pos3 = pos2 + Vec2d{each_size[0] * i, 0};
      const auto col   = (i == hovering_idx) ? style->col_WidgetBg : style->col_WidgetBgHover;
      if(i == hovering_idx) {
      }
      r->rectPS(pos3, each_size, col);
      r->rectPS(pos3, each_size, style->col_WidgetLine, 1);
      std::string tt;
      if(range[1] - range[0] > 100) {
        tt = std::to_string((int)(i == 0 ? value->min : value->max));
      } else {
        std::ostringstream out;
        out.precision(2);
        out << std::fixed << (float)(i == 0 ? value->min : value->max);
        tt = out.str();
      }
      const auto s      = r->put_text(tt, pos3, 1, {255, 255, 255}) + style->WidgetPadding;
      new_outer_size[0] = new_outer_size[0] + s[0];
      new_outer_size[1] = std::max(s[1], new_outer_size[1]);
      last_value        = *value;
    }
    new_outer_size += style->WidgetPadding * 2;
    new_outer_size[0] = std::max<int>(new_outer_size[0], 20);
    new_outer_size[1] = std::max<int>(new_outer_size[1], 10);
    if(new_outer_size != outerSize) {
      impl_needCalcAlinment_parent();
      outerSize = new_outer_size;
    }
    needRendering(false);
  }
}
template <typename T> bool uiRange2<T>::CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, [[maybe_unused]] int num_1, [[maybe_unused]] int num_2, [[maybe_unused]] const char** strings) {
  const int each_size = (size[0] - _num_display_pos[0] + pos[0]) / 3;
  int tmp_idx         = (vec2_1[0] - _num_display_pos[0]) / each_size;
  if(tmp_idx > 2 || vec2_1[1] < pos[1] || pos[1] + size[1] < vec2_1[1] || vec2_1[0] < _num_display_pos[0]) tmp_idx = -1;
  if(flag == uiCallbackFlags::LMouseDown || flag == uiCallbackFlags::MouseScroll) {
    selecting_idx = tmp_idx;
  } else if(flag == uiCallbackFlags::MouseMove) {
    hovering_idx = tmp_idx;
  }

  if(selecting_idx < 0) {
    return false;
  }
  if(focused_widget() != this) return false;

  if(flag == uiCallbackFlags::MouseScroll) {
    if(selecting_idx == 0)
      value->min += num_2 > 0 ? 1 : -1;
    else
      value->max += num_2 > 0 ? 1 : -1;

    return true;
  } else if(flag == uiCallbackFlags::LMouseDown) {
    start_mouse_pos_x = vec2_1[0];
    start_mouse_val   = *value;
  } else if(flag == uiCallbackFlags::MouseMove && (num_2 & 0b01)) {
    if(selecting_idx == 0)
      value->min = start_mouse_val.min + delta * (vec2_1[0] - start_mouse_pos_x);
    else
      value->max = start_mouse_val.max + delta * (vec2_1[0] - start_mouse_pos_x);
  }

  value->min = std::clamp<float>(value->min, range[0], range[1]);
  value->max = std::clamp<float>(value->max, range[0], range[1]);
  return true;
}

// the explicit instanciation
template uiRange2<double>::uiRange2(std::string, Range<double>*, Vec2, const double);
template uiRange2<float>::uiRange2(std::string, Range<float>*, Vec2, const double);
template uiRange2<int>::uiRange2(std::string, Range<int>*, Vec2, const double);
template uiRange2<unsigned int>::uiRange2(std::string, Range<unsigned int>*, Vec2, const double);

} // namespace mu::ui
