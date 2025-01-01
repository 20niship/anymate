#include <movutl/instance/instance.hpp>
#include <movutl/ui/ui.hpp>
#include <movutl/ui/widget.hpp>

namespace mu::ui {

void uiWidget::renderScrollbars() {
  const auto style = getStyle();
  const auto wnd   = drawing_wnd();
  // if((lastInnerSize != innerSize) || (scrollPos != lastScrollPos)){ needRendering(true); if(widgets.size() > 0) needApplyAlignment();}
  const auto innerSize = getOuterSize() - style->WidgetPadding - Vec2d(0, -style->TitlebarHeight); // TODO: Widgetごとに異なる可能性があるのでgetInnerSize()にする？それともスクロールバーRendering実装をWidget個別にする＿

  Vec2d pos_scrollX = pos + Vec2d{0, size[1] - style->ScrollbarWidth};
  Vec2d pos_scrollY = pos + Vec2d{size[0] - style->ScrollbarWidth, 0};

  auto r = wnd->get_renderer();
  if(innerSize[1] > size[1] && flags.needRendering > 0) {
    r->rectPS(pos_scrollY, Vec2d(style->ScrollbarWidth, size[1] - style->ScrollbarWidth), style->col_wndScrollBg);
    r->rectPS(Vec2d(pos_scrollY[0] + 1, pos[1] + scrollPos[1] * size[1] / innerSize[1]), Vec2d(style->ScrollbarWidth - 1, (size[1] - style->ScrollbarWidth) * size[1] / innerSize[1]), style->col_wndScroll);
  }

  if(innerSize[0] > size[0] && flags.needRendering > 0) {
    r->rectPS(pos_scrollX, Vec2d(size[0] - style->ScrollbarWidth, style->ScrollbarWidth), style->col_wndScrollBg);
    r->rectPS(Vec2d(pos[0] + scrollPos[0] * size[0] / innerSize[0], pos_scrollX[1] + 1), Vec2d(size[0] * (size[0] - style->ScrollbarWidth) / innerSize[0], style->ScrollbarWidth - 1), style->col_wndScroll);
  }
  // lastInnerSize = innerSize;
  // lastScrollPos = scrollPos;
}

void uiWidget::CallbackTitlebar(uiCallbackFlags flag, Vec2d vec2_1, [[maybe_unused]] int num_1, [[maybe_unused]] int num_2, [[maybe_unused]] const char** strings) {
  static bool is_moving = false;
  static Vec2d moving_mouse_offset{0, 0};
  static Vec2d move_start_pos = pos;

  const int th                 = getStyle()->TitlebarHeight;
  const bool touching_closebtn = Rect(Vec2f(pos[0] + size[0] - th, pos[1]), Vec2f(th, th)).contains(vec2_1);
  const bool touching_titlebar = Rect(pos, {size[0] - th - 6, th}).contains(vec2_1);

  if(flag == uiCallbackFlags::MouseMove && is_moving && (num_2 & 0x01)) {
    setPos(vec2_1 - moving_mouse_offset);
    impl_needCalcAlinment_parent();
  } else if(flag == uiCallbackFlags::LMouseDown && touching_titlebar && flags.Active) {
    const auto parent = getParentWidget();
    if(parent == this || parent->align_type != uiWidgetAlignTypes::Absolute) {
      return;
    }
    moving_mouse_offset = vec2_1 - pos;
    move_start_pos      = pos;
    is_moving           = true;
    impl_needCalcAlinment_parent();
  } else if(flag == uiCallbackFlags::LMouseUP && (move_start_pos - pos).norm_sq() < 10) {
    is_moving = false;
    if(!flags.EnableTitlebar) {
      return;
    }
    if(touching_closebtn) {
      setActive(false);
      return;
    }

    if(touching_titlebar) {
      if(parent->align_type == uiWidgetAlignTypes::Absolute) {
        // 今操作しているWidgetを最前面に表示する
        if(!parent->isRenderedTop(this)) parent->toRenderTop(this);
      }
      CollapseTitlebar(!flags.CollapsingTitlebar);
    }

    impl_needCalcAlinment_parent(); // TODO:Frame in Frameの設計がなされている時はParent->parentにする必要があるような....
  } else {
    is_moving = false;
  }
}

void uiWidget::updateScrollPos() {
  const auto style = getStyle();
  if(!flags.EnableTitlebar) return;
  const auto innerSize   = getOuterSize() - style->WidgetPadding - Vec2d(0, -style->TitlebarHeight); // TODO: Widgetごとに異なる可能性があるのでgetInnerSize()にする？それともスクロールバーRendering実装をWidget個別にする＿
  const auto expand_size = innerSize - size;
  if(flags.EnableScroll_Y && expand_size[0] > 0) scrollPos[0] = std::clamp<int>(scrollPos[0], 0, expand_size[0]);
  if(flags.EnableScroll_Y && expand_size[1] > 0) scrollPos[1] = std::clamp<int>(scrollPos[1], 0, expand_size[1]);
}


bool uiWidget::CallbackResizer(uiCallbackFlags flag, Vec2d, int , int , const char** ) {
  if(!(flag == uiCallbackFlags::LMouseDown || flag == uiCallbackFlags::LMouseUP || flag == uiCallbackFlags::MouseMove || flag == uiCallbackFlags::OffHover)) return false;
  const auto wnd = drawing_wnd();

  constexpr int ui_inner_gap_ = 3;
  const int th                = getStyle()->TitlebarHeight + 4;
  constexpr int resizer_size  = 10;
  Rect resizeRect_ld(pos[0] + size[0] - resizer_size, pos[0] + size[0], pos[1] + size[1] - resizer_size, pos[1] + size[1]);
  Rect resizeRect_btm(pos[0], pos[0] + size[0], pos[1] + size[1] - ui_inner_gap_, pos[1] + size[1]);
  Rect resizeRect_left(pos[0], pos[0] + ui_inner_gap_,  pos[1] + th, pos[1] + size[1] - th);
  Rect resizeRect_right(pos[0] + size[0] - ui_inner_gap_, pos[0] + size[0], pos[1] + th, pos[1] + size[1]);

  const auto io        = instance::get_io();
  const auto mouse_pos = io->mouse_pos;
  const auto lbtn_down = io->is_mouse_left_pressing();

  if(flags.EnableUserResize && !flags.CollapsingTitlebar) {
    bool isMouseOn = ((flag == uiCallbackFlags::MouseMove) && lbtn_down) || flag == uiCallbackFlags::LMouseDown;
    // リサイズ開始かどうかを判断
    if(flag == uiCallbackFlags::LMouseDown) {
      if(resizeRect_ld.contains(mouse_pos)) {
        isSelectingResizer = true;
        wnd->setCrossHairCursor();
      } else if(resizeRect_btm.contains(mouse_pos)) {
        isSelectingEdge_bottom = true;
        wnd->setVResizeCursor();
      } else if(resizeRect_left.contains(mouse_pos)) {
        isSelectingEdge_left = true;
        wnd->setHResizeCursor();
      } else if(resizeRect_right.contains(mouse_pos)) {
        isSelectingEdge_right = true;
        wnd->setHResizeCursor();
      }
    }

    if(isSelectingResizer) {
      if(isMouseOn) {
        setWidth(mouse_pos[0] - pos[0]);
        setHeight(mouse_pos[1] - pos[1]);
        needRendering(true);
        impl_needCalcAlinment_parent();
        return true;
      } else if(flag == uiCallbackFlags::LMouseUP) {
        isSelectingResizer = false;
        parent->needRendering(true);
        wnd->setDefaultCursor();
        return true;
      }
    }
    if(isSelectingEdge_left) {
      if(isMouseOn) {
        //    (flag == uiCallbackFlags:::LMouseDown && ))
        setWidth(size[0] - mouse_pos[0] + pos[0]);
        setPosX(mouse_pos[0]);
        impl_needCalcAlinment_parent();
        parent->needRendering(true);
        return true;
      } else if(flag == uiCallbackFlags::LMouseUP) {
        isSelectingEdge_left = false;
        parent->needRendering(true);
        wnd->setDefaultCursor();
        return true;
      }
    }
    if(isSelectingEdge_right) {
      if(isMouseOn) {
        //    (flag == uiCallbackFlags:::LMouseDown && ))
        setWidth(mouse_pos[0] - pos[0]);
        impl_needCalcAlinment_parent();
        parent->needRendering(true);
        return true;
      } else if(flag == uiCallbackFlags::LMouseUP) {
        isSelectingEdge_right = false;
        parent->needRendering(true);
        wnd->setDefaultCursor();
        return true;
      }
    }
    if(isSelectingEdge_bottom) {
      if(isMouseOn) {
        setHeight(mouse_pos[1] - pos[1]);
        impl_needCalcAlinment_parent();
        parent->needRendering(true);
        return true;
      } else if(flag == uiCallbackFlags::LMouseUP) {
        isSelectingEdge_bottom = false;
        parent->needRendering(true);
        wnd->setDefaultCursor();
        return true;
      }
      // }else if(isSelectingEdge_top){
      //     if(isMouseOn) {
      //         setPosY(mouse_pos[1]);
      //         setHeight(mouse_pos[1] - pos[1]);
      //         parent->needRendering(true);
      //         return true;
      //     } else if(flag == uiCallbackFlags::LMouseUP) {
      //         isSelectingEdge_top = false;
      //         setHeight(mouse_pos[1] - pos[1]);
      //         setPosY(mouse_pos[1]);
      //         parent->needRendering(true);
      //         glfwSetCursor(window, NULL);
      //         return true;
      //     }
    }
  }
  // for(int i=0; i<widgets.size(); i++){
  //     if(widgets[i]->CallbackResizer(flag, mouse_pos, num_1, num_2, strings)) return true;
  // }
  return false;
}

bool uiWidget::CallbackScrollbars(uiCallbackFlags flag, Vec2d vec2_1, [[maybe_unused]] int num_1, [[maybe_unused]] int num_2, [[maybe_unused]] const char** strings) {
  auto style = getStyle();
  bool contains;
  const auto innerSize = getOuterSize() - style->WidgetPadding - Vec2d(0, -style->TitlebarHeight); // TODO: Widgetごとに異なる可能性があるのでgetInnerSize()にする？それともスクロールバーRendering実装をWidget個別にする＿
  // Vertical
  if(innerSize[1] >= size[1]) {
    contains = (pos[0] + size[0] - style->ScrollbarWidth <= vec2_1[0]) && (vec2_1[0] <= pos[0] + size[0]) && (pos[1] <= vec2_1[1]) && (vec2_1[1] < pos[1] + size[1] - 30);
    if(flag == uiCallbackFlags::LMouseUP) isSelectingScrollY = false;
    if(((flag == uiCallbackFlags::LMouseDown) && contains) || isSelectingScrollY) {
      int tmp_sy         = (float(vec2_1[1]) - float(pos[1]) - float((size[1] - style->ScrollbarWidth) * size[1]) / (innerSize[1] * 2.0)) * innerSize[1] / size[1];
      isSelectingScrollY = true;
      scrollPos[1]       = std::clamp<int>(tmp_sy, 0, innerSize[1] - size[1]);
      needRendering(true);
      impl_needCalcAlinment_parent();
      return true;
    }
    if(flag == uiCallbackFlags::MouseScroll && num_2 != 0) {
      scrollPos[1] = std::clamp<int>(scrollPos[1] - num_2 * 15, 0, innerSize[1] - size[1]);
      impl_needCalcAlinment_parent();
      needRendering(true);
      return true;
    }
  } else {
    if(flag == uiCallbackFlags::MouseScroll && num_2 != 0) {
      if(parent != this) return parent->CallbackScrollbars(flag, vec2_1, num_1, num_2, strings);
    }
  }


  if(innerSize[0] >= size[0]) {
    // Horizontal
    contains = (pos[0] <= vec2_1[0]) && (vec2_1[0] <= pos[0] + size[0]) && (pos[1] + size[1] - style->ScrollbarWidth <= vec2_1[1]) && (vec2_1[1] < pos[1] + size[1]);
    if(flag == uiCallbackFlags::LMouseUP) isSelectingScrollX = false;
    if(((flag == uiCallbackFlags::LMouseDown) && contains) || isSelectingScrollX) {
      int tmp_sy         = (float(vec2_1[0]) - float(pos[0]) - float(size[0] * (size[0] - style->ScrollbarWidth)) / (innerSize[0] * 2.0)) * innerSize[0] / size[0];
      isSelectingScrollX = true;
      scrollPos[0]       = std::clamp<int>(tmp_sy, 0, innerSize[0] - size[0]);
      needRendering(true);
      return true;
    }
  } else {
    if(flag == uiCallbackFlags::MouseScroll && num_1 != 0) {
      if(parent != this) return parent->CallbackScrollbars(flag, vec2_1, num_1, num_2, strings);
    }
  }
  return false;
}


// -----------------------------------------------------
//   [SECTION]   Calc alignment
// -----------------------------------------------------


void uiWidget::calcAlignVertical() {
  if(widgets.size() == 0) return;
  if(flags.EnableTitlebar && flags.CollapsingTitlebar) return;
  const auto style          = getStyle();
  const uint16_t titlebar_y = flags.EnableTitlebar ? style->TitlebarHeight : 0;

  uint16_t desiredHeight      = 0;
  uint16_t desiredWidth       = 0;
  uint16_t numExHeightWidgets = 0;
  uint16_t numShHeightWidgets = 0;

  bool EnScrollX = false;
  bool EnScrollY = false; // thisウィジェットのスクロールバーを表示するか（右と下にスペースを作るか
  for(auto w : widgets) {
    if(w->getEnableAutoExpandY()) numExHeightWidgets++;
    if(w->getEnableAutoShrinkY()) numShHeightWidgets++;
    EnScrollX |= !(w->getEnableAutoExpandX());
    EnScrollY |= !(w->getEnableAutoExpandY());
  }

  for(auto w : widgets) {
    const auto tmpCurS = w->getOuterSize(); // getSize();
    // WIDTH SETTINGS:
    const int this_frame_inner_size = std::max<int>(0, size[0] - style->WidgetMargin[0] * 2 - (EnScrollX ? (style->ScrollbarWidth) : 0));
    const bool x_overflow           = this_frame_inner_size >= tmpCurS[0];
    if(x_overflow) {
      w->setWidth(w->getEnableAutoExpandX() ? this_frame_inner_size : tmpCurS[0]);
    } else {
      w->setWidth(w->getEnableAutoShrinkX() ? this_frame_inner_size : tmpCurS[0]);
    }
    desiredHeight += tmpCurS[1] + style->WidgetMargin[1];
  }

  // 高さの調整（各Widgetごとに拡大縮小
  const auto height_max = size[1] - (EnScrollY ? (style->ScrollbarWidth) : 0) - titlebar_y;
  if(desiredHeight < height_max) {
    if(numExHeightWidgets > 0) {
      const int expand_each = (height_max - desiredHeight) / numExHeightWidgets; // 拡大できるサイズ
      assert(expand_each >= 0);
      for(auto w : widgets) {
        if(w->getEnableAutoExpandY()) {
          w->setHeight(std::min<int>(w->getOuterSize()[1] + expand_each, height_max));
        } else {
          w->setHeight(std::min<int>(w->getOuterSize()[1], height_max));
        }
      }
    } else {
      for(auto w : widgets) {
        w->setHeight(w->getOuterSize()[1]);
      }
    }
  } else {
    if(numShHeightWidgets > 0) {
      const int shrink_each = (desiredHeight - height_max) / numShHeightWidgets;
      assert(shrink_each > 0);
      for(auto w : widgets) {
        if(w->getEnableAutoShrinkY()) {
          w->setHeight(std::max<int>(w->getOuterSize()[1] - shrink_each, style->WidgetMinSize[1]));
        } else {
          w->setHeight(w->getOuterSize()[1]);
        }
      }
    } else {
      for(auto w : widgets) w->setHeight(w->getOuterSize()[1]);
    }
  }

  // それぞれのサイズの確認と表示位置の設定
  desiredWidth  = 0;
  desiredHeight = 0;
  for(auto w : widgets) {
    const auto tmpCurS = w->getSize();
    w->setPos(Vec2d(pos[0] + style->WidgetMargin[0] - scrollPos[0], pos[1] + titlebar_y + desiredHeight - scrollPos[1]));
    desiredWidth = std::max<int>(desiredWidth, tmpCurS[0]);
    desiredHeight += tmpCurS[1] + style->WidgetMargin[1];
  }

  // setSize(desiredWholeSize, -1)
  // innerSize[0] = desiredWidth;
  // innerSize[1] = desiredHeight + titlebar_y;
  outerSize = style->WidgetPadding + Vec2d(desiredWidth, desiredHeight + titlebar_y);

  needRendering(true);
  flags.needApplyAlignment = 0;
  flags.needCalcInnerSize  = 0;
}


void uiWidget::calcAlignHorizontal() {
  /*
if(widgets.size() == 0) { flags.needApplyAlignment = false; flags.needCalcInnerSize = false; return; }
const auto style = getStyle();

uint16_t titlebar_y = 0;
if(flags.EnableTitlebar) titlebar_y  = style->FontSize + style->WidgetInnerSpace_y*2 + 4;

  Vec2d outerSpace;
  outerSpace[0] = spacing ? (style->WidgetOuterSpace_x) : 0;
  outerSpace[1] = spacing ? (style->WidgetOuterSpace_y) : 0;

  uint16_t desiredWidth = 0;
  uint16_t desiredHeight = 0;
  uint16_t numExWidthWidgets = 0;
  uint16_t numShWidthWidgets = 0;
  Vec2d tmpMinS, tmpMaxS, tmpCurS;

  bool EnScrollX = false;
  bool EnScrollY = false; // thisウィジェットのスクロールバーを表示するか（右と下にスペースを作るか
  for(uint16_t i = 0; i<widgets.size(); i++){
      if(widgets[i]->getEnableAutoExpandX()) numExWidthWidgets ++;
      if(widgets[i]->getEnableAutoShrinkX()) numShWidthWidgets ++;

      EnScrollX |= !(widgets[i]->getEnableAutoExpandX());
      EnScrollY |= !(widgets[i]->getEnableAutoExpandY());
  }

  for(uint16_t i =0; i< widgets.size(); i++){
      tmpCurS = widgets[i]->getInnerSize(); //getSize();
      std::cout << "render size = (" << tmpCurS[0] << ", " << tmpCurS[1] << ")\n";

      //HEIGHT SETTINGS:
      if(size[1] - outerSpace[1] -  (EnScrollY ? (style->ScrollbarWidth) : 0) >= tmpCurS[1] + outerSpace[1]*2){
          if(widgets[i]->getEnableAutoExpandY()){ widgets[i]->setHeight(size[1] - outerSpace[1] - (EnScrollY ? (style->ScrollbarWidth) : 0) - titlebar_y); }else{ widgets[i]->setWidth(tmpCurS[1]); }
      }else{
          if(widgets[i]->getEnableAutoShrinkY()){ widgets[i]->setHeight(size[1] - outerSpace[1] - (EnScrollY ? (style->ScrollbarWidth) : 0) - titlebar_y); }else{ widgets[i]->setWidth(tmpCurS[1]); }
      }

      //HEIGHT SETTINGS:
      desiredWidth += tmpCurS[0] + outerSpace[1]*2;
  }

  // 高さの調整（各Widgetごとに拡大縮小
  if(desiredWidth < size[0] -  (EnScrollX ? (style->ScrollbarWidth) : 0)){
      if(numExWidthWidgets > 0){
          int expand_each = (size[0] - desiredWidth - (EnScrollX ? (style->ScrollbarWidth) : 0)) / numExWidthWidgets; //拡大できるサイズ
          for(int i=0; i<widgets.size(); i++){
              if(widgets[i]->getEnableAutoExpandY()){ widgets[i]->setWidth(std::min<int>(widgets[i]->getInnerSize()[0] + expand_each, size[0] - (EnScrollX ? (style->ScrollbarWidth) : 0) )); }
              else                                  { widgets[i]->setWidth(std::min<int>(widgets[i]->getInnerSize()[0],               size[0] - (EnScrollX ? (style->ScrollbarWidth) : 0) )); }
          }
      }else{
          for(int i=0; i<widgets.size(); i++) widgets[i]->setHeight(widgets[i]->getInnerSize()[0]);
      }
  }else{
      if(numShWidthWidgets > 0){
          int shrink_each =  (desiredWidth + (EnScrollX ? (style->ScrollbarWidth) : 0) - size[0]) / numShWidthWidgets;
          for(int i=0; i<widgets.size(); i++){
              if(widgets[i]->getEnableAutoShrinkY()){ widgets[i]->setWidth(std::max<int>(int(widgets[i]->getInnerSize()[0]) - int(shrink_each), style->WidgetMinSize_y)); }
              else                                  { widgets[i]->setWidth(widgets[i]->getInnerSize()[0]); }
          }
      }else{
          for(int i=0; i<widgets.size(); i++) widgets[i]->setWidth(widgets[i]->getInnerSize()[0]);
      }
  }

  // それぞれのサイズの確認と表示位置の設定
  desiredWidth  = 0; desiredWidth  = 0;
  for(int i = 0; i<widgets.size(); i++){
      tmpCurS = widgets[i]->getSize();
      widgets[i]->setPos(Vec2d(pos[0] + outerSpace[0] - scrollPos[0] + desiredWidth, pos[1] + titlebar_y));
      std::cout << "position : " << widgets[i]->getPosX() << ", " << widgets[i]->getPosY() << std::endl;
      desiredWidth += tmpCurS[0] + outerSpace[1]*2;
      desiredHeight = std::max<int>(desiredHeight, tmpCurS[0]);
  }

  innerSize[1] = desiredHeight + titlebar_y;
  innerSize[0] = desiredWidth;
  needRendering(true);
  flags.needApplyAlignment = 0;
  flags.needCalcInnerSize = 0;
  */
}

} // namespace mu::ui
