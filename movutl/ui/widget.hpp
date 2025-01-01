#pragma once

#include <movutl/ui/io.hpp>
#include <cmath>
#include <codecvt>
#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace mu::ui{
using namespace core;

class uiWidget {
protected:
  struct uiWidgetFlags_ {
    // unsigned short Visible               : 1 = 1;
    unsigned int Active : 1 = 1;
    unsigned int EnableScroll_X : 1 = 1;       // Always show scrollbar (even if ContentSize[1] < Size[1])
    unsigned int EnableScroll_Y : 1 = 1;       // Always show scrollbar (even if ContentSize[1] < Size[1])
    unsigned int EnableMouseInputs : 1 = 1;    // Enable mouse callback(mouse Click or mouse Move)
    unsigned int EnableKeyboardInputs : 1 = 1; // Enable keyboard callback
    // unsigned int EnableChildWidget     : 1 = 1;   // Enable child window and show child windows
    unsigned int EnableDragDropInputs : 1 = 0; // Enable child window and show child windows
    unsigned int EnableUserResize : 1 = 0;
    // unsigned int is3D                  : 1 = 0;

    // [internal] Please access via getter/setter functions
    unsigned int EnableAutoExpandX : 1 = 0; // Widgetの大きさが小さく親Widgetに余白が余っている場合、ギリギリまで拡大する
    unsigned int EnableAutoExpandY : 1 = 0; // Widgetの大きさが小さく親Widgetに余白が余っている場合、ギリギリまで拡大する
    unsigned int EnableAutoShrinkX : 1 = 0; // setSize()で style->minSizeまで縮小できる
    unsigned int EnableAutoShrinkY : 1 = 0; // setSize()で style->minSizeまで縮小できる
    unsigned int EnableTitlebar : 1 = 0;    // Titlebar = style->FontSize + style->WidgetInnerSize[0]
    unsigned int CollapsingTitlebar : 1 = 0;
    unsigned int isHovering : 1 = 0;
    unsigned int isFocused : 1 = 0;
    // unsigned int EnableUserResize       : 1 = 0;   // UserがsetSizeでリサイズすることを許可する
    unsigned int needRendering : 2 = 3;     // 強制的にレンダリングを行う（VertexArrayとかに描画データを格納）
    unsigned int needCalcAlignment : 1 = 1; // 自身と子のWidgetのサイズを計算し直す必要がある。
    unsigned int needCalcInnerSize : 1 = 1; // Alignmentを計算する時にInnerSizeをレンダリング関数で計算し直す。
    unsigned int needApplyAlignment : 1 = 1;
    // unsigned int setupFinished         : 1 = 0;   // initAlignment()関数が呼ばれたかどうか
  } flags;

  // protected:
  uiWidget* parent{nullptr};
  Vec2d size{-1, -1};
  Vec2d pos{-1, -1};
  uiWidgetAlignTypes align_type{uiWidgetAlignTypes::Absolute};
  // スクロール関連
  Vec2d scrollPos{0, 0};
  Vec2d lastScrollPos{-1, -1};
  // Vec2d innerSize{1, 1}; // このWidgetの中に子Widgetが入るときに参照するサイズ. renderScrollbarsでinnerSizeで除算するところがあるので1以上
  Vec2d outerSize{1, 1}; //
  bool isSelectingScrollX{false}, isSelectingScrollY{false};
  bool isSelectingResizer{false}, isSelectingEdge_left{false}, isSelectingEdge_top{false}, isSelectingEdge_bottom{false}, isSelectingEdge_right{false};

  void calcAlignHorizontal();
  void calcAlignVertical();

  void renderScrollbars();
  void updateScrollPos();
  bool CallbackScrollbars(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings);
  void CallbackTitlebar(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings);
  bool CallbackResizer(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings);
  void CallbackAlignment(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings);

public:
  std::vector<uiWidget*> widgets; // child widgets

  uiWidget(const uiWidget&) = delete;
  uiWidget& operator=(const uiWidget&) = delete;

  uiWidget() : parent(this) {
    // needApplyAlignment();
    // needCalcInnerSize();
    needRendering(true);
  }

  // uiWidget(Vec2d pos, Vector2d size, void *ptr_data){
  // 	setPos(pos);
  // 	setSize(size);
  // }

  virtual ~uiWidget() {
    for(size_t i = 0; i < widgets.size(); i++) {
      delete widgets[i];
    }
  }
  virtual void render() = 0;
  virtual bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings) = 0;
  bool setSize(const Vec2d& size_) {
    static Vec2d last_size;
    bool resize = size != last_size;
    last_size = size = size_;
    if(resize) {
      needRendering(true);
      return true;
    }
    return false;
    // TODO: 同じ階層の他のWidgetにResize Callbackを出して、OKかどうかを確認する
  }
  bool setSize(const int x, const int y) { return setSize({x, y}); }

  inline void setWidth(const int x) { size[0] = x; }
  inline void setHeight(const int y) { size[1] = y; }

  inline uint16_t getWidth() const { return size[0]; }
  inline uint16_t getHeight() const { return size[1]; }
  inline uint16_t getPosX() const { return pos[0]; }
  inline uint16_t getPosY() const { return pos[1]; }
  inline Vec2d getPos() const { return pos; }
  inline Vec2d getSize() const { return size; }
  // inline Vec2d  getInnerSize() const         { return innerSize; }
  inline Vec2d getOuterSize() const { return outerSize; }
  // inline int       getInnerWidth()              { return innerSize[0]; }
  // inline int       getInnerHeight()             { return innerSize[1]; }
  inline void setPos(const Vec2d& pos_) {
    pos[0] = pos_[0];
    pos[1] = pos_[1];
    needRendering(true);
  }
  inline void setPos(const int x, const int y) { setPos({x, y}); }
  inline void setPosX(int x_) {
    pos[0] = x_;
    needRendering(true);
  }
  inline void setPosY(int y_) {
    pos[1] = y_;
    needRendering(true);
  }
  inline bool contains(int x, int y) const { return ((x >= pos[0]) && (x <= pos[0] + size[0]) && (y >= pos[1]) && (y <= pos[1] + size[1])); }
  // inline bool     containsTitlebar(int x, int y){ if(flags.EnableTitlebar){ return false; } auto style = getStyle(); return ((x >= pos[0]) && (x <= pos[0] + size[0]) && (y >= pos[1]) && (y <=
  // pos[1] + style->TitlebarHeight)); }
  inline void needRendering(bool necessary) {
    if(necessary) {
      flags.needRendering = 3;
      for(size_t i = 0; i < widgets.size(); i++) {
        widgets[i]->needRendering(necessary);
      } /*if(parent != this && parent != nullptr){parent->needRendering(true); }*/
    } else {
      if(flags.needRendering > 0) flags.needRendering--;
    }
  }
  // inline void      needApplyAlignment ()         { impl_needCalcAlignment_child(); impl_needCalcAlinment_parent();} //
  // inline void      needCalcInnerSize()          { impl_needCalcInnerSize_parent();} //
  inline uiWidget* getParentWidget() { return parent; }
  inline uiWidget* getRootWidget() {
    if(parent != this) {
      return parent->getRootWidget();
    } else {
      return this;
    }
  }
  inline void setParentWidget(uiWidget* w) { parent = w; }

  // [internal] Do Not Use!!!
  inline void impl_needCalcAlignment_all() { getRootWidget()->impl_needCalcAlignment_child(); }
  inline void impl_needCalcInnerSize_parent() {
    flags.needCalcInnerSize = true;
    if(parent == this) return;
    parent->impl_needCalcInnerSize_parent();
  }
  inline void impl_needCalcAlinment_parent() {
    flags.needApplyAlignment = true;
    if(parent == this) return;
    parent->impl_needCalcAlinment_parent();
  }
  inline void impl_needCalcAlignment_child() {
    flags.needApplyAlignment = true;
    for(size_t i = 0; i < widgets.size(); i++) {
      widgets[i]->impl_needCalcAlignment_child();
    }
  }
  inline void impl_needCalcAlignment() {
    flags.needApplyAlignment = true;
    needRendering(true);
  }

  // flags settings
  // inline void     setVisivle         (bool  v)     { flags.Visible   = v;         needRendering(); }
  inline void setActive(bool v) {
    flags.Active = v;
    needRendering(true);
    for(size_t i = 0; i < widgets.size(); i++) {
      widgets[i]->setActive(v);
    }
  }
  inline void setEnableScroll_X(bool v) {
    flags.EnableScroll_X = v;
    needRendering(true);
  }
  inline void setEnableScroll_Y(bool v) {
    flags.EnableScroll_Y = v;
    needRendering(true);
  }
  inline void setEnableUserInput(bool v) {
    if(!v) {
      return;
    }
    flags.EnableKeyboardInputs = 1;
    flags.EnableMouseInputs = 1;
  }
  inline void setEnableUserResize(bool v) { flags.EnableUserResize = v; }
  inline void setEnablAutoResize(bool v) {
    flags.EnableAutoExpandX = v;
    flags.EnableAutoExpandY = v;
  }
  inline void CollapseTitlebar(bool c) {
    flags.CollapsingTitlebar = c;
    for(size_t i = 0; i < widgets.size(); i++) {
      widgets[i]->setActive(!c);
    }
  }
  inline void setHoveringFlag(bool v) {
    flags.isHovering = v;
    needRendering(true);
    CallbackFunc((v ? uiCallbackFlags::OnHover : uiCallbackFlags::OffHover), Vec2d(0, 0), 0, 0, nullptr);
  }
  inline void setFocusedFlag(bool v) {
    flags.isFocused = v;
    needRendering(true);
  }
  inline void setAlignType(uiWidgetAlignTypes v) {
    align_type = v;
    needRendering(true);
    flags.needCalcAlignment = 1;
  }
  // inline uiWidgetAlignTypes     getAlignType()      { return align_type;}
  inline bool getEnableAutoExpandX() const { return flags.EnableAutoExpandX > 0; }
  inline bool getEnableAutoExpandY() const { return flags.EnableAutoExpandY > 0; }
  inline bool getEnableAutoShrinkX() const { return flags.EnableAutoShrinkX > 0; }
  inline bool getEnableAutoShrinkY() const { return flags.EnableAutoShrinkY > 0; }
  inline bool isActive() const { return flags.Active; }
  inline bool isRenderedTop(const uiWidget* w) const { return widgets[widgets.size() - 1] == w; }
  inline void toRenderTop(uiWidget* w) {
    for(size_t i = 0; i < widgets.size(); i++) {
      if(widgets[i] != w) continue;
      widgets[i] = widgets[widgets.size() - 1];
      widgets[widgets.size() - 1] = w;
      return;
    }
  }
  inline bool getNeedRendering() const { return flags.needRendering; }
  inline void render_child_widget() {
    for(auto w : widgets) {
      w->render();
    }
  }
  inline int get_widget_num() const {
    int i = widgets.size();
    for(auto w : widgets) {
      i += w->get_widget_num();
    }
    return i;
  }

  inline void calcInnerSize_recursize() {
    if(!flags.needCalcInnerSize) return;
    needRendering(true);
    for(size_t i = 0; i < widgets.size(); i++) {
      widgets[i]->calcInnerSize_recursize();
      widgets[i]->render();
    }
    flags.needCalcInnerSize = false;
  }

  inline void applyAlignment_recursive() {
    if(!flags.needApplyAlignment && parent != nullptr) return;
    for(size_t i = 0; i < widgets.size(); i++) {
      widgets[i]->applyAlignment_recursive();
    }
    switch(align_type) {
      case uiWidgetAlignTypes::Absolute:
        break; // do nothing !!
      // case uiWidgetAlignTypes::Grid:
      // case uiWidgetAlignTypes::HorizontalList:
      // case uiWidgetAlignTypes::VertialListl:
      // case uiWidgetAlignTypes::Horizontel_noSpace:
      // case uiWidgetAlignTypes::Vertical_noSpace:
      default: calcAlignVertical(); break;
    }
    flags.needApplyAlignment = false;
  }

  // 子Widget全てをUserリサイズ不可にする
  // inline void     setChildEnablAutoResize (bool  v) {
  // 	if(!v) return;
  // 	for(size_t i=0; i<widgets.size(); i++){
  // 		widgets[i]->setChildEnablAutoResize(v);
  // 	}
  // }
  // inline Vec2d getMinSize() {
  // 	Vec2d tmp__ = innerSize;
  // 	// if(widgets.size() == 0){
  // 		auto style = getStyle();
  // 		if(flags.EnableAutoShrinkX){tmp__[0] = std::min<float>(tmp__[0], style->WidgetMinSize_x);}
  // 		if(flags.EnableAutoShrinkY){tmp__[1] = std::min<float>(tmp__[0], style->WidgetMinSize_y);}
  // 	// }else{
  // 	//   setAlignment(align_type);
  // 	// }
  // 	return tmp__;
  // }
  // inline Vec2d getMaxSize() {
  // 	Vec2d tmp__ = innerSize;
  // 	if(flags.EnableAutoExpandX){tmp__[0] = (uint16_t)-1;}
  // 	if(flags.EnableAutoExpandY){tmp__[1] = (uint16_t)-1;}
  // 	return tmp__;
  // }

  inline uiWidget* AddWidget(std::initializer_list<uiWidget*> widget_) {
    for(auto w : widget_) {
      AddWidget(w);
    }
    return this;
  }

  // inline bool AddWidgets(uiWidget *widget, ...){
  // 	const std::vector<uiWidget *> ws{widget...};
  // 	for(auto w : ws){ AddWidget(w);}
  // 	return true;
  // }

  inline uiWidget* AddWidget(uiWidget* widget_) {
    assert(widget_ != nullptr);
    //  if (widget_ == nullptr) return false;
    /* if(!flags.EnableChildWidget){ std::cerr << "!flags.EnableChildWidget @ AddWidget Failed!!!;" << std::endl; return false;} */
    widgets.push_back(widget_);
    widget_->setParentWidget(this);
    impl_needCalcAlignment_child();
    impl_needCalcAlinment_parent();
    impl_needCalcInnerSize_parent();
    return this;
  }
};


enum class WidgetDrawTypes { DrawAsLines, DrawAsSurface, DrawAsPoints };

class uiRootWidget : public uiWidget {
public:
  uiRootWidget() {}
  void render() override {
    for(auto w : widgets) {
      w->render();
    }
  }
  bool CallbackFunc([[maybe_unused]] uiCallbackFlags flag, [[maybe_unused]] Vec2d vec2_1, [[maybe_unused]] int num_1, [[maybe_unused]] int num_2, [[maybe_unused]] const char** strings) override {
    return true;
  };
};

class uiRootWidget2D : public uiWidget {
private:
  void calcHoveringWidget(int x, int y);
  uiWidget *HoveringWidget{static_cast<uiWidget*>(this)}, *FocusedWidget{static_cast<uiWidget*>(this)};

public:
  uiRootWidget2D();
  uiRootWidget2D(const uiRootWidget2D&) = delete;
  uiRootWidget2D& operator=(const uiRootWidget2D&) = delete;
  void render() override;
  bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings) override;
  uiWidget* getHoveringWidget() const { return HoveringWidget; }
  uiWidget* getFocusedWidget() const { return FocusedWidget; }
};

} // namespace vkUI
