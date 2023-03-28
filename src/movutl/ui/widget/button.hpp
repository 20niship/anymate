#pragma once

#include <movutl/ui/ui.hpp>
#include <movutl/ui/widget.hpp>
namespace mu::ui {


class uiButton : public uiWidget {
private:
  bool* value;
  bool last_value;
  std::string text;

public:
  uiButton()                           = delete;
  uiButton(const uiButton&)            = delete;
  uiButton& operator=(const uiButton&) = delete;

  uiButton(std::string text_, bool* value_);
  void render() override;
  bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings) override;
};

class uiButtonFunc : public uiWidget {
private:
  std::function<void(void)> func;
  //bool last_value;
  std::string text;

public:
  uiButtonFunc()                           = delete;
  uiButtonFunc(const uiButton&)            = delete;
  uiButtonFunc& operator=(const uiButton&) = delete;
  uiButtonFunc(const std::string text_, std::function<void(void)> func_);
  void render() override;
  bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings) override;
};

inline uiWidget* button(const std::string text_, bool* v) { return (new uiButton(text_, v)); }
inline uiWidget* button(const std::string text_, std::function<void(void)> f) { return (new uiButtonFunc(text_, f)); }
} // namespace mu::ui
