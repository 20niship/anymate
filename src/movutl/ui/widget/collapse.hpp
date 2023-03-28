#include <movutl/ui/widget.hpp>

namespace mu::ui {

class uiCollapse : public uiWidget {
  std::string title;

public:
  uiCollapse()                             = delete;
  uiCollapse(const uiCollapse&)            = delete;
  uiCollapse& operator=(const uiCollapse&) = delete;

  uiCollapse(const std::string title_);
  void render() override;
  bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings) override;
};

inline uiWidget* collapse(const std::string& text_) { return new uiCollapse(text_); }
inline uiWidget* collapse(const std::string& text_, const std::initializer_list<uiWidget*>& w) {
  auto c = new uiCollapse(text_);
  for(auto&& w_ : w) c->AddWidget(w_);
  return c;
}

} // namespace mu::ui
