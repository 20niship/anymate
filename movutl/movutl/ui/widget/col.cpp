#include <movutl/ui/ui.hpp>
#include <movutl/ui/widget/col.hpp>
#include <movutl/core/color.hpp>

namespace mu::ui {

uiCol::uiCol(const std::string& text_, Vec3b* value_) {
  text                    = text_;
  value                   = value_;
  last_value              = *value_;
  flags.EnableAutoExpandX = true;
  flags.EnableAutoShrinkX = true;
}
void uiCol::render() {
  if(!flags.Active) {
    needRendering(false);
    return;
  }
  auto wnd         = drawing_wnd();
  const auto style = getStyle();
  if(*value == last_value && (!flags.needRendering)) return;
  auto r = wnd->get_renderer();

  const auto ss = r->put_text(text, pos + Vec2d{0, style->WidgetPadding[0]}, 1);
  const Vec2d pos2_{pos[0] + ss[0] + style->WidgetMargin[0], pos[1]};
  r->rectPS(pos2_, {size[1], size[1]}, *value);
  r->rectPS(pos2_, {size[1], size[1]}, style->col_WidgetLine, 1);
  const auto pos2 = pos2_ + Vec2d(size[1] + 1, 0);
  const Vec2d each_size{(size[0] - pos2[0] + pos[0]) / 3 - 1, size[1]};
  _num_display_pos = pos2;
  if(each_size[0] < 0) return;
  Vec2d new_outer_size = ss;
  for(int i = 0; i < 3; i++) {
    const Vec2d pos3 = pos2 + Vec2d{each_size[0] * i, 0};
    const auto col   = (i == selecting_idx) ? style->col_WidgetBg : style->col_WidgetBgHover;
    r->rectPS(pos3, each_size, col);
    r->rectPS(pos3, each_size, style->col_WidgetLine, 1);
    const auto s      = r->put_text(std::to_string((*value)[i]), pos3, 1) + style->WidgetPadding;
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

bool uiCol::CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, [[maybe_unused]] int num_1, [[maybe_unused]] int num_2, [[maybe_unused]] const char** strings) {
  constexpr int delta = 1;
  Vec3 tmp_v{(double)(*value)[0], (double)(*value)[1], (double)(*value)[2]};
  const int each_size = (size[0] - _num_display_pos[0] + pos[0]) / 3;
  int tmp_idx         = (vec2_1[0] - _num_display_pos[0]) / each_size;
  if(tmp_idx > 2 || vec2_1[1] < pos[1] || pos[1] + size[1] < vec2_1[1] || vec2_1[0] < _num_display_pos[0]) tmp_idx = -1;
  if(flag == uiCallbackFlags::LMouseDown || flag == uiCallbackFlags::MouseScroll) {
    selecting_idx = tmp_idx;
  }
  if(selecting_idx < 0) {
    return false;
  }
  if(focused_widget() != this) return false;

  if(flag == uiCallbackFlags::MouseScroll) {
    std::cout << "MOUSESCROLL" << vec2_1[0] << vec2_1[1] << std::endl;
    tmp_v[selecting_idx] += num_2 > 0 ? delta : -delta;
    return true;
  } else if(flag == uiCallbackFlags::LMouseDown) {
    start_mouse_pos_x = vec2_1[0];
    start_mouse_val   = tmp_v;
  } else if(flag == uiCallbackFlags::MouseMove && (num_2 & 0b01)) {
    tmp_v[selecting_idx] = start_mouse_val[selecting_idx] + delta * (vec2_1[0] - start_mouse_pos_x);
  }

  (*value)[0] = std::clamp<int>(tmp_v[0], 0, 255);
  (*value)[1] = std::clamp<int>(tmp_v[1], 0, 255);
  (*value)[2] = std::clamp<int>(tmp_v[2], 0, 255);
  return true;
}


uiCol2::uiCol2(const std::string& text_, Vec3b* value_) {
  text                    = text_;
  value                   = value_;
  last_value              = *value_;
  flags.EnableAutoExpandX = true;
  flags.EnableAutoShrinkX = true;
}
void uiCol2::render() {
  if(!flags.Active) {
    needRendering(false);
    return;
  }
  auto wnd = drawing_wnd();
  auto r   = wnd->get_renderer();

  const auto style = getStyle();
  if(*value == last_value && (!flags.needRendering)) return;
  hsv = core::RGB2HSV(*value);

  constexpr int bar_color = 15;
  const int w             = std::min<int>(size[0], 300);

  const Vec2d col1_top = pos + style->WidgetPadding;
  const Vec2d col1_size(w - bar_color - style->WidgetPadding[0] * 2, w);

  const Vec2d col2_top = pos + Vec2d(w - bar_color, 0);
  const Vec2d col2_size(bar_color, w);

  r->rectPS(col1_top, col1_size, style->col_WidgetLine, 2);
  r->rectPS(col2_top, col2_size, style->col_WidgetLine, 2);

  constexpr int sp = 30;

  const int id_e = hsv[0] * w / 360;
  for(int i = 0; i < sp; i++) {
    const Vec3 hsv1((double)i * 360 / sp, 100, 100);
    const Vec3 hsv2((double)(i + 1) * 360 / sp, 100, 100);
    const auto c1 = core::HSVtoRGB(hsv1);
    const auto c2 = core::HSVtoRGB(hsv2);

    const Vec2d r1 = col2_top + Vec2d(0, w * i / sp);
    const Vec2d ds(col2_size[0], col2_size[1] / sp + 1);

    r->rectPS(r1, ds, c1, c2, c2, c1);
  }

  {
    const Vec2d r1_ = col2_top + Vec2d(0, id_e);
    const Vec2d r2_ = col2_top + Vec2d(bar_color, id_e);
    r->line(r1_, r2_, {0, 0, 0}, 2);
    r->arrow_left(r2_ - Vec2d(8, 5), 10, {0, 0, 0});
    r->arrow_right2(r1_ + Vec2d(0, -5), 10, {0, 0, 0});
  }

  {
    const Vec3b c1 = core::HSVtoRGB<double>({(float)hsv[0], 100.0, 0.0});
    const Vec3b c2 = core::HSVtoRGB<double>({(float)hsv[0], 100.0, 100.0});
    const Vec3b c3 = core::HSVtoRGB<double>({(float)hsv[0], 0.0, 100.0});
    r->rectPS(col1_top, col1_size, c3, c1, c1, c2);

    const int x = (double)col1_size[0] * hsv[1] / 100;
    const int y = (double)col1_size[0] * (100 - hsv[2]) / 100;
    r->circle(pos + Vec2d{x, y}, 5, {255, 255, 255}, 2);
  }

  outerSize = {w, w};
  needRendering(false);
}

bool uiCol2::CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, [[maybe_unused]] int num_1, [[maybe_unused]] int num_2, [[maybe_unused]] const char** strings) {
  if(focused_widget() != this) return false;

  constexpr int bar_color = 15;
  const int w             = std::min<int>(size[0], 300);

  const auto style     = getStyle();
  const Vec2d col1_top = pos + style->WidgetPadding;
  const Vec2d col1_size(w - bar_color - style->WidgetPadding[0] * 2, w);

  [[maybe_unused]]const Vec2d col2_top = pos + Vec2d(w - bar_color, 0);
  const Vec2d col2_size(bar_color, w);

  if(flag != uiCallbackFlags::MouseMove) return false;
  if(num_2 != 0b01) return false;

  if(vec2_1[0] > pos[0] + w - bar_color) {
    hsv[0] = 360.0f * (vec2_1[1] - pos[1]) / w;
    *value = core::HSVtoRGB(hsv);
    return true;
  } else {
    hsv[1] = 100.0f * (vec2_1[0] - col1_top[0]) / col1_size[0];
    hsv[2] = 100.0f - 100.0 * (vec2_1[1] - col1_top[1]) / col1_size[1];
    *value = core::HSVtoRGB(hsv);
    return true;
  }
}

} // namespace mu::ui
