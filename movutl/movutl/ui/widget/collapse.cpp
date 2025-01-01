#include <movutl/ui/ui.hpp>
#include <movutl/ui/widget/collapse.hpp>
#include <movutl/instance/instance.hpp>

namespace mu::ui {
uiCollapse::uiCollapse(std::string text_) {
  title = text_;
  needRendering(true);
  setAlignType(uiWidgetAlignTypes::VertialListl);
  flags.EnableAutoExpandX = 1;
  flags.EnableTitlebar    = 1;
  flags.EnableAutoExpandY = 0;
  flags.EnableAutoShrinkX = 1;
  flags.EnableAutoShrinkY = 0;
  flags.EnableUserResize  = 0;
  // flags.EnableScroll_X = 1;
  flags.EnableScroll_Y = 0;
}

void uiCollapse::render() {
  if(!flags.Active) {
    needRendering(false);
    std::cout << "Not visivle or not active widget\n";
    return;
  }
  auto wnd         = drawing_wnd();
  auto r           = wnd->get_renderer();
  const auto style = getStyle();

  Vec2d whole_size{0, 0};
  // if(lastScrollPos != scrollPos){ needRendering(true); }
  const uint16_t title_bar = style->TitlebarHeight;
  if(!flags.needRendering) return;

  // wnd->setClippingRect(std::move(vkUI::Rect(pos+Vec2d{0, title_bar}, size - Vec2d{0, title_bar})));
  // if(title != "" ) flags.EnableTitlebar = 1;
  r->rectPS(pos, {size[0], title_bar}, {50, 50, 90});
  if(flags.CollapsingTitlebar) {
    r->arrow_right2(pos + 2, title_bar - 5, style->col_Text);
  } else {
    r->arrow_down(pos + title_bar / 3.0f, title_bar * 0.8, style->col_Text);
  }
  const auto s = r->put_text(title, pos + Vec2d{title_bar + 3, 2}, 1, style->col_Text);
  whole_size   = Vec2d{s[0] + title_bar, s[1]} + style->WidgetMargin;
  if(!flags.CollapsingTitlebar) {
    for(uint16_t i = 0; i < widgets.size(); i++) {
      widgets[i]->render();
      const auto sw = widgets[i]->getSize();
      whole_size    = {std::max<int>(whole_size[0], sw[0] + style->WidgetMargin[0]), whole_size[1] + sw[1] + style->WidgetMargin[1] * 2};
    }
  }
  if(whole_size != outerSize) {
    outerSize = whole_size;
    impl_needCalcAlinment_parent();
  }
  needRendering(false);
}

bool uiCollapse::CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, [[maybe_unused]] int num_1, [[maybe_unused]] int num_2, [[maybe_unused]] const char** strings) {

  if(!flags.Active) return false;
  if(flag != uiCallbackFlags::LMouseDown) return false;
  /* if(!flags.EnableTitlebar ){ return false; } */
  const int th = getStyle()->TitlebarHeight;
  const auto r = Rect(pos, {size[0] - th - 6, th});
  if(!r.contains(vec2_1)) return false;

  CollapseTitlebar(!flags.CollapsingTitlebar);
  impl_needCalcAlignment_all(); // TODO:Frame in Frameの設計がなされている時はParent->parentにする必要があるような....
  return false;
}
} // namespace mu::ui
