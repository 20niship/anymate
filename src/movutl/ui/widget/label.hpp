#pragma once

#include <movutl/ui/ui.hpp>
#include <movutl/ui/widget.hpp>
namespace mu::ui {

class uiLabel : public uiWidget {
private:
  std::string text;
  Vec3b col;

public:
  uiLabel()                          = delete;
  uiLabel(const uiLabel&)            = delete;
  uiLabel& operator=(const uiLabel&) = delete;

  uiLabel(const std::string text_);
  uiLabel(const std::string text_, const Vec3b& col);
  void render() override;
  void setText(const std::string& str) {
    text = str;
    needRendering(true);
  }
  std::string getText() const { return text; }
  bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings) override;
};

inline uiWidget* label(const std::string text_) { return (new uiLabel(text_)); }
inline uiWidget* label(const std::string text_, const Vec3b& col) { return (new uiLabel(text_, col)); }
} // namespace mu::ui
