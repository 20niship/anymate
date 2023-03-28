#pragma once

#include <movutl/ui/ui.hpp>
#include <movutl/ui/widget.hpp>
namespace mu::ui {

class uiFrame : public uiWidget {
  std::string title;
  int last_not_collapse_size{-1};
  bool last_collapsing{false};

public:
  uiFrame()                          = delete;
  uiFrame(const uiFrame&)            = delete;
  uiFrame& operator=(const uiFrame&) = delete;

  uiFrame(const std::string title_);
  void render() override;
  bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings) override;
};

inline uiWidget* frame(const std::string text_) { return (new uiFrame(text_)); }
inline uiWidget* frame(const std::string text_, std::initializer_list<uiWidget *> w) { 
  auto f = new uiFrame(text_);
  f->AddWidget(w);
  return f;
}
} // namespace mu::ui
