#pragma once
#include <movutl/ui/ui.hpp>
#include <movutl/ui/widget.hpp>
namespace mu::ui {
class uiTable : public uiWidget {
private:
  std::vector<std::string> columns;
  size_t cols;
  Vec3b col{255, 255, 255};

public:
  uiTable()                          = delete;
  uiTable(const uiTable&)            = delete;
  uiTable& operator=(const uiTable&) = delete;
  uiTable(const std::string& text_); // \t for next column, \r for next row
  uiTable(const std::vector<std::string>& text_, const size_t cols);
  void setTextALl(const std::string&); // \t for next column, \r for next row
  void setCols(const size_t n);
  void setRows(const size_t n);
  void setSize(const size_t x, const size_t y) {
    setCols(x);
    setRows(y);
  }
  int getCols() const { return cols; }
  int getRows() const { return columns.size() / cols; }
  const std::string& operator()(int x, int y) const;
  std::string& operator()(int x, int y);
  void render() override;
  bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char** strings) override;
};

inline uiWidget* table(const std::string& text_) { return (new uiTable(text_)); }
inline uiWidget* table(const std::vector<std::string> text_, const int n) { return (new uiTable(text_, n)); }
} // namespace mu::ui
