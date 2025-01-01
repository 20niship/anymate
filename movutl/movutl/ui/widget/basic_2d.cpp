#include <algorithm>
#include <functional>
#include <locale>
#include <string>
#include <type_traits>
#include <vector>

#include <movutl/core/logger.hpp>
#include <movutl/ui/all_widgets.hpp>
#include <movutl/ui/ui.hpp>


namespace mu::ui {

uiRootWidget2D::uiRootWidget2D() {}
void uiRootWidget2D::render() {
  for(auto w : widgets) {
    if(w->isActive())w->render();
  }
}
void uiRootWidget2D::calcHoveringWidget(int x, int y) {
  uiWidget* LastHovering = HoveringWidget;
  if(!LastHovering && LastHovering->contains(x, y) && LastHovering->isActive() && LastHovering != this) return;
  uiWidget* w{this};
  bool search_all = false;
  while(w->widgets.size() > 0 && !search_all) {
    const auto w_start = w;
    for(int i = w->widgets.size() - 1; i >= 0; i--) {
      const auto w2 = w->widgets[i];
      if(w2->contains(x, y) && w2->isActive()) {
        w = w2;
        break;
      }
    }
    if(w_start == w) {
      break;
    }
  }
  if(LastHovering != w) {
    LastHovering->setHoveringFlag(false);
    w->setHoveringFlag(true);
    HoveringWidget = w;
  }
}

bool uiRootWidget2D::CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, [[maybe_unused]] int num_1, [[maybe_unused]] int num_2, [[maybe_unused]] const char** strings) {
  double xpos, ypos;
  glfwGetCursorPos(drawing_wnd()->getGLFWwindow(), &xpos, &ypos);
  // 一次的に機能停止
  // if(CallbackResizer(flag, {xpos, ypos}, num_1, num_2, strings)) return true;

  switch(flag) {
    case uiCallbackFlags::OnHover: break;
    case uiCallbackFlags::OffHover: break;
    case uiCallbackFlags::ValueChange: break;
    case uiCallbackFlags::CharInput: break;
    case uiCallbackFlags::ShouldClose: break;

    case uiCallbackFlags::ResizeTo:
      size = vec2_1;
      // needRendering(true);
      // needApplyAlignment();
      // needCalcInnerSize();
      break;

    case uiCallbackFlags::MouseMove:
      calcHoveringWidget(xpos, ypos);
      if(HoveringWidget != static_cast<uiWidget*>(this)) {
        HoveringWidget->CallbackFunc(uiCallbackFlags::MouseMove, {(int)xpos, (int)ypos}, HoveringWidget == FocusedWidget, num_2, nullptr);
      }
      if(FocusedWidget != static_cast<uiWidget*>(this) && HoveringWidget != FocusedWidget) {
        FocusedWidget->CallbackFunc(uiCallbackFlags::MouseMove, {(int)xpos, (int)ypos}, false, num_2, nullptr);
      }
      break;

    case uiCallbackFlags::LMouseDown:
    case uiCallbackFlags::RMouseDown:
    case uiCallbackFlags::CMouseDown:
      calcHoveringWidget(xpos, ypos);
      if(FocusedWidget != HoveringWidget) {
        FocusedWidget->setFocusedFlag(false);
        FocusedWidget = HoveringWidget;
        FocusedWidget->setFocusedFlag(true);
      }
      if(FocusedWidget != this) {
        FocusedWidget->CallbackFunc(flag, {(int)xpos, (int)ypos}, num_1, num_2, strings);
      }
      break;

    case uiCallbackFlags::LMouseUP:
    case uiCallbackFlags::RMouseUP:
    case uiCallbackFlags::CMouseUP:
      calcHoveringWidget(xpos, ypos);
      if(FocusedWidget != this) FocusedWidget->CallbackFunc(flag, {(int)xpos, (int)ypos}, num_1, num_2, strings);
      break;

    case uiCallbackFlags::MouseScroll:
      calcHoveringWidget(xpos, ypos);
      if(HoveringWidget != this) {
        const auto resolved = HoveringWidget->CallbackFunc(uiCallbackFlags::MouseScroll, {(int)xpos, (int)ypos}, num_1, num_2, strings);
        if(resolved && FocusedWidget != HoveringWidget) {
          FocusedWidget->setFocusedFlag(false);
          FocusedWidget = HoveringWidget;
          FocusedWidget->setFocusedFlag(true);
        }
      }
      if(FocusedWidget != this) FocusedWidget->CallbackFunc(uiCallbackFlags::MouseScroll, {(int)xpos, (int)ypos}, num_1, num_2, strings);
      break;

      // case uiCallbackFlags::DragDrop:
      //     calcHoveringWidget(xpos, ypos);
      //     FocusedWidget->setFocusedFlag(false);
      //     FocusedWidget = HoveringWidget;
      //     FocusedWidget->setFocusedFlag(true);
      //     if(FocusedWidget != this) FocusedWidget->CallbackFunc(uiCallbackFlags::DragDrop, Vec2d(xpos, ypos),num_1, num_2, strings);
      //     printf("drop %d\n", num_1);
      //     for (int i = 0; i < num_1; i++) printf("%s\n", strings[i]);
      //     break;

    case uiCallbackFlags::Keyboard:
      calcHoveringWidget(xpos, ypos);
      FocusedWidget->setFocusedFlag(false);
      FocusedWidget->setFocusedFlag(true);
      if(FocusedWidget != this) FocusedWidget->CallbackFunc(uiCallbackFlags::Keyboard, {(int)xpos, (int)ypos}, num_1, num_2, strings);
      break;


      // switch(num_1){
      //     case GLFW_KEY_F11:
      //         if(num_2 == GLFW_PRESS) setFullScreen(!wndStyle.isFullScreen);
      //         break;
      //     case GLFW_KEY_ESCAPE:
      //         if(wndStyle.isFullScreen && num_2 == GLFW_PRESS) setFullScreen(false);
      //         break;
      // }

      break;
    default: LOGE << "Not set flag!!\n"; break;
  }

  return true;
}

// -----------------------------------------------------
//   [SECTION]   Widget Text
// -----------------------------------------------------
uiLabel::uiLabel(const std::string text_) {
  text = text_;
  col  = {255, 255, 255};
}
uiLabel::uiLabel(const std::string text_, const Vec3b& col_) {
  text = text_;
  col  = col_;
}
void uiLabel::render() {
  auto wnd = drawing_wnd();
  auto r   = wnd->get_renderer();
  if(!flags.Active) {
    needRendering(false);
    std::cout << "Not visivle or not active widget\n";
    return;
  }
  if(flags.needRendering) {
    const auto s = r->put_text(text, pos, 1, col);
    if(s != outerSize) {
      outerSize = s;
      impl_needCalcAlinment_parent();
    }
    needRendering(false);
  }
}

bool uiLabel::CallbackFunc([[maybe_unused]] uiCallbackFlags flag, [[maybe_unused]] Vec2d vec2_1, [[maybe_unused]] int num_1, [[maybe_unused]] int num_2, [[maybe_unused]] const char** strings) { return true; }


uiButton::uiButton(std::string text_, bool* value_) {
  value      = value_;
  last_value = !(*value);
  text       = text_;
}
void uiButton::render() {
  if(!flags.Active) {
    needRendering(false);
    std::cout << "Not visivle or not active widget\n";
    return;
  }

  if(*value != last_value) {
    needRendering(true);
  }
  if(flags.needRendering) {
    const auto style = getStyle();
    auto r           = drawing_wnd()->get_renderer();
    if(*value) {
      r->rectPS(pos, size, style->col_WidgetBg);
    } else {
      r->rectPS(pos, size, flags.isHovering ? (style->col_WidgetBgHover) : (style->col_WidgetBgSelected));
    }
    r->rectPS(pos, size, style->col_WidgetLine, 1);
    const auto s = r->put_text(text, pos + style->WidgetPadding, 1) + style->WidgetPadding * 2;
    if(s != outerSize) {
      impl_needCalcAlinment_parent();
      outerSize = s;
    }
    last_value = *value;
    needRendering(false);
  }
}

bool uiButton::CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, [[maybe_unused]] int num_1, [[maybe_unused]] int num_2, [[maybe_unused]] const char** strings) {
  if(flag == uiCallbackFlags::LMouseDown && contains(vec2_1[0], vec2_1[1])) {
    // if ( flag == uiCallbackFlags::LMouseDown){
    *value = !(*value);
    needRendering(true);
  }
  return true;
}

uiButtonFunc::uiButtonFunc(const std::string text_, std::function<void(void)> func_) {
  text = text_;
  func = func_;
  needRendering(true);
}
void uiButtonFunc::render() {
  if(!flags.Active) {
    needRendering(false);
    return;
  }
  if(flags.needRendering) {
    auto wnd         = drawing_wnd();
    const auto style = getStyle();
    auto r           = wnd->get_renderer();
    r->rectPS(pos, size, style->col_WidgetBg);
    r->rectPS(pos, size, style->col_WidgetLine, 1);
    const auto s = r->put_text(text, pos + style->WidgetPadding, 1) + style->WidgetPadding * 2;
    if(s != outerSize) {
      impl_needCalcAlinment_parent();
      outerSize = s;
    }
    needRendering(false);
  }
}

bool uiButtonFunc::CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, [[maybe_unused]] int num_1, [[maybe_unused]] int num_2, [[maybe_unused]] const char** strings) {
  if(flag == uiCallbackFlags::LMouseDown && contains(vec2_1[0], vec2_1[1])) {
    func();
    needRendering(true);
  }
  return true;
}


uiFrame::uiFrame(std::string text_) {
  title = text_;
  needRendering(true);
  setAlignType(uiWidgetAlignTypes::VertialListl);
  flags.EnableAutoExpandX = 1;
  flags.EnableAutoExpandY = 1;
  flags.EnableAutoShrinkX = 1;
  flags.EnableAutoShrinkY = 1;
  flags.EnableUserResize  = 1;
  // flags.EnableScroll_X = 1;
  flags.EnableScroll_Y = 1;
  setSize({200, 200});
}

void uiFrame::render() {
  if(!flags.Active) {
    needRendering(false);
    std::cout << "Not visivle or not active widget\n";
    return;
  }
  auto wnd         = drawing_wnd();
  const auto style = getStyle();

  // if(lastScrollPos != scrollPos){ needRendering(true); }
  const uint16_t title_bar = flags.EnableTitlebar ? style->TitlebarHeight : 0;
  if(flags.needRendering > 0) {
    // if(flags.CollapsingTitlebar && flags.EnableTitlebar) size[1] = style->TitlebarHeight;
    // else CollapseTitlebar(false);
    if(flags.EnableTitlebar) {
      if(last_collapsing != flags.CollapsingTitlebar) {
        if(flags.CollapsingTitlebar) {
          last_not_collapse_size = size[1];
        } else {
          size[1] = last_not_collapse_size;
        }
        last_collapsing = flags.CollapsingTitlebar;
      }
      if(flags.CollapsingTitlebar) {
        size[1] = title_bar;
      }
    }
    wnd->setClippingRect(std::move(Rect(pos + Vec2d{0, title_bar}, size - Vec2d{0, title_bar})));
    auto r = drawing_wnd()->get_renderer();
    r->rectPS(pos, size, style->col_WidgetBg);
    r->rectPS(pos, size, style->col_WidgetLine, 2);

    // draw resozer
    constexpr int resizer_size = 10;
    r->triangle(pos + size - Vec2d{resizer_size, 0}, pos + size - Vec2d{0, 0}, pos + size - Vec2d{0, resizer_size}, Vec3b{100, 100, 200});

    if(title != "") flags.EnableTitlebar = 1;
    if(flags.EnableTitlebar) {
      r->rectPS(pos, {size[0], title_bar}, style->col_WidgetBgHover);
      r->rectPS(pos, {size[0], title_bar}, style->col_PopupBg, 1);
      if(flags.CollapsingTitlebar)
        r->arrow_right(pos + 2, title_bar - 5, style->col_Text);
      else
        r->arrow_down(pos + (int)(title_bar / 3), title_bar * 0.8, style->col_Text);
      const auto s = r->put_text(title, pos + Vec2d{title_bar + 3, 2}, 1, style->col_Text);
      r->cross_button(pos + Vec2d{size[0] - title_bar + 1, 1}, title_bar - 2, style->col_WidgetBg, style->col_WidgetLine, style->col_Text);
      size[0] = std::max(s[0] + title_bar, size[0]);
    }
  }

  /* DrawList[0].setClipRect(pos_ + Vec2d(0, title_bar), size_ - Vec2d(0, title_bar)); */
  if(!flags.CollapsingTitlebar && flags.EnableTitlebar) {
    for(uint16_t i = 0; i < widgets.size(); i++) widgets[i]->render();
    renderScrollbars();
  } else {
    // innerSize = size;
    outerSize = size;
  }
  needRendering(false);
}

bool uiFrame::CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, [[maybe_unused]] int num_1, [[maybe_unused]] int num_2, [[maybe_unused]] const char** strings) {
  if(!flags.Active) return false;
  CallbackScrollbars(flag, vec2_1, num_1, num_2, strings);
  CallbackTitlebar(flag, vec2_1, num_1, num_2, strings);
  CallbackResizer(flag, vec2_1, num_1, num_2, strings);
  updateScrollPos();
  return true;
}



uiCheckbox::uiCheckbox(std::string text_, bool* value_) {
  value      = value_;
  last_value = !(*value);
  text       = text_;
}
void uiCheckbox::render() {
  if(!flags.Active) {
    needRendering(false);
    std::cout << "Not visivle or not active widget\n";
    return;
  }
  if(*value != last_value) {
    needRendering(true);
  }
  if(!flags.needRendering) return;

  const auto style = getStyle();
  // 1. draw checkbox
  constexpr int checkbox_size = 14;
  const int ypad              = (size[1] - checkbox_size) / 2;
  auto r                      = drawing_wnd()->get_renderer();
  r->rectPS(pos + Vec2d(0, ypad), {checkbox_size, checkbox_size}, style->col_WidgetBg);
  r->rectPS(pos + Vec2d(0, ypad), {checkbox_size, checkbox_size}, style->col_WidgetLine, 1);
  if(*value) {
    r->check(pos + Vec2d(0, ypad), checkbox_size, {255, 255, 255}, 2);
  }
  const auto s = r->put_text(text, pos + style->WidgetPadding + Vec2d{checkbox_size, 0}, 1) + style->WidgetPadding * 2;
  if(s != outerSize) {
    impl_needCalcAlinment_parent();
    outerSize = s;
  }
  last_value = *value;
  needRendering(false);
}

bool uiCheckbox::CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, [[maybe_unused]] int num_1, [[maybe_unused]] int num_2, [[maybe_unused]] const char** strings) {
  if(flag == uiCallbackFlags::LMouseDown && contains(vec2_1[0], vec2_1[1])) {
    *value = !(*value);
    needRendering(true);
    return true;
  }
  return false;
}


} // namespace mu::ui
