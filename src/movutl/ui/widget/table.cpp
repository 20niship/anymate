#include <movutl/core/logger.hpp>
#include <movutl/ui/widget/table.hpp>
#include <movutl/instance/instance.hpp>

namespace mu::ui {

static std::vector<std::string> strsplit(const std::string& c, const std::string sep) {
  const auto separator_length = sep.length(); // 区切り文字の長さ
  auto list                   = std::vector<std::string>();
  if(separator_length == 0) {
    list.push_back(c);
  } else {
    auto offset = std::string::size_type(0);
    while(1) {
      auto pos = c.find(sep, offset);
      if(pos == std::string::npos) {
        list.push_back(c.substr(offset));
        break;
      }
      list.push_back(c.substr(offset, pos - offset));
      offset = pos + separator_length;
    }
  }
  return list;
}

uiTable::uiTable(const std::string& text_) {
  setTextALl(text_);
  flags.EnableAutoExpandX = flags.EnableAutoShrinkX = 1;
}

uiTable::uiTable(const std::vector<std::string>& text_, const size_t cols_) {
  assert(cols_ > 0);
  assert(text_.size() > 0);
  cols                    = cols_;
  columns                 = text_;
  flags.EnableAutoExpandX = flags.EnableAutoShrinkX = 1;
}

void uiTable::setTextALl(const std::string& text_) {
  if(text_.size() == 0) {
    LOGE << "uiTable str is empty!";
    columns = {"<EMPTY STRING>"};
    cols    = 1;
    return;
  }
  const auto lines = strsplit(text_, "\n");
  if(lines.size() == 0) {
    LOGE << "uiTable str is empty!";
    columns = {"<EMPTY STRING>"};
    cols    = 1;
    return;
  }
  cols     = 1;
  columns  = {};
  bool tmp = true;
  for(const std::string& l : lines) {
    const auto td = strsplit(l, "\t");
    if(tmp && td.size() > 0) {
      cols = td.size();
      tmp  = false;
    }
    for(const std::string& t : td) columns.push_back(t);
  }
}

void uiTable::setCols(const size_t n) {
  assert(n > 0);
  cols = n;
  if(columns.size() < n) {
    columns.resize(n);
  }
}

void uiTable::setRows(const size_t n) {
  assert(n > 0);
  columns.resize(cols * n);
}

const std::string& uiTable::operator()(int x, int y) const {
  assert(x >= 0 && y >= 0);
  assert(x + y * cols < columns.size());
  return columns[y * cols + x];
}
std::string& uiTable::operator()(int x, int y) {
  assert(x >= 0 && y >= 0);
  assert(x + y * cols < columns.size());
  return columns[y * cols + x];
}

void uiTable::render() {
  MU_ASSERT(cols > 0);
  MU_ASSERT(columns.size() >= cols);
  if(!flags.needRendering) return;
  // size[1] = std::max<int>(size[1], cols * 100);
  auto r             = drawing_wnd()->get_renderer();
  const int rows     = columns.size() / cols;
  const int td_width = size[0] / cols;
  const auto style   = instance::get_style();
  int curHeight      = pos[1];
  r->line(pos, {pos[0] + size[0], pos[1]}, style->col_WidgetLineDisabled, 1.0);
  for(int y = 0; y < rows; y++) {
    int dh_max = 0;
    for(size_t x = 0; x < cols; x++) {
      const size_t idx = y * cols + x;
      if(columns.size() <= idx) break;
      const std::string& t = columns[idx];
      const Vec2d strPos   = Vec2d(pos[0] + td_width * x, curHeight) + style->WidgetPadding / 2;
      const auto ws        = r->put_text(t, strPos, 1.0, col, td_width);
      dh_max               = std::max<int>(dh_max, ws[1] + style->WidgetPadding[1]);
    }
    curHeight += dh_max;
    /* r->line(Vec2d(pos[0], curHeight), {pos[0] + size[0], curHeight}, style->col_WidgetLineDisabled, 1.0); */
  }
  for(size_t x = 0; x <= cols; x++) {
    const int X = pos[0] + td_width * x;
    r->line(Vec2d(X, pos[1]), {X, pos[1] + size[1]}, style->col_WidgetLineDisabled, 1.0);
  }

  const Vec2d new_outer_size = {size[1], curHeight - pos[1]};
  if(new_outer_size != outerSize) {
    /* impl_needCalcAlinment_parent(); */
    outerSize = new_outer_size;
  }
  /* needRendering(false); */
  //TODO: ↑これなに？
}

bool uiTable::CallbackFunc(uiCallbackFlags , Vec2d , int , int , const char** ) { return false; }

} // namespace mu::ui
