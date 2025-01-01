#include <movutl/instance/instance.hpp>
#include <movutl/ui/widget/plot.hpp>
#include <vector>

namespace mu::ui {

uiPlot::uiPlot() {
  flags.EnableAutoExpandX = 1;
  flags.EnableAutoExpandY = 1;
}

uiPlot::uiPlot(const std::string& title_) {
  title                   = title_;
  flags.EnableAutoExpandX = 1;
  flags.EnableAutoExpandY = 1;
}

template <typename T> Vec2 uiPlot::getRange(const T* v, size_t n) const {
  assert(n > 0);
  Vec2 range{(double)v[0], (double)v[0]};
  for(size_t i = 0; i < n; i++) {
    range[0] = std::min<double>(v[i], range[0]);
    range[1] = std::max<double>(v[i], range[1]);
  }
  return range;
}

template <typename T> void uiPlot::setPlotRange(const T* v, size_t n, const bool isXrange) {
  const auto r = getRange(v, n);
  if(isXrange)
    prange.xlim = r;
  else
    prange.ylim = r;
}

template <typename T> void uiPlot::setPlotRange(const T* x, const T* y, size_t n) {
  prange.xlim = getRange(x, n);
  prange.ylim = getRange(y, n);
  prange.set  = true;
}

template <typename T> void uiPlot::setPlotRange(const _Vec<T, 2>* v, size_t n) {
  assert(n > 0);
  Vec2 rangeX{v[0][0], v[0][0]};
  Vec2 rangeY{v[0][1], v[0][1]};
  for(size_t i = 0; i < n; i++) {
    rangeX[0] = std::min<double>(v[i][0], rangeX[0]);
    rangeX[1] = std::max<double>(v[i][0], rangeX[1]);
    rangeY[0] = std::min<double>(v[i][1], rangeY[0]);
    rangeY[1] = std::max<double>(v[i][1], rangeY[1]);
  }
  prange.xlim = rangeX;
  prange.ylim = rangeY;
  prange.set  = true;
}

template <typename T> void uiPlot::plotLine(const T* v, const size_t n, const uiPlotParams& param) {
  auto r = drawing_wnd()->get_renderer();
  if(!flags.Active) {
    return;
  }
  if(!prange.set) {
    setPlotRangeY(v, n);
    prange.xlim = {0, (double)n};
    prange.set  = true;
  }
  for(size_t i = 0; i < n - 1; i++) {
    const Vec2d p0 = {cvt_pos_x(i), cvt_pos_y(v[i])};
    const Vec2d p1 = {cvt_pos_x(i + 1), cvt_pos_y(v[i + 1])};
    r->line(p0, p1, param.col_line, 2);
  }
  if(param.label != "") {
    label_list.push_back({param.col_line, param.label});
  }
}
template <typename T> void uiPlot::plotLine(const T* x, const T* y, size_t n, const uiPlotParams& param) {
  auto r = drawing_wnd()->get_renderer();
  if(!flags.Active) {
    return;
  }
  if(!prange.set) {
    setPlotRangeX(x, n);
    setPlotRangeY(y, n);
    prange.set = true;
  }
  for(size_t i = 0; i < n - 1; i++) {
    const auto p0 = cvt_pos(x[i], y[i]);
    const auto p1 = cvt_pos(x[i + 1], y[i + 1]);
    r->line(p0, p1, param.col_line, 2);
  }
  if(param.label != "") {
    label_list.push_back({param.col_line, param.label});
  }
}

template <typename T> void uiPlot::plotScatter(const _Vec<T, 2>* v, size_t n, const uiPlotParams& param) {
  auto r = drawing_wnd()->get_renderer();
  if(!flags.Active) {
    return;
  }
  if(!prange.set) {
    setPlotRange(v, n);
  }
  switch(param.marker) {
    case uiPlotMarker::Circle:
      for(size_t i = 0; i < n; i++) r->circle(cvt_pos(v[i]), param.size, param.col_line);
      break;
    case uiPlotMarker::Cross:
      for(size_t i = 0; i < n; i++) r->cross(cvt_pos(v[i]), param.size, param.col_line);
      break;
    case uiPlotMarker::Plus:
      for(size_t i = 0; i < n; i++) r->plus(cvt_pos(v[i]), param.size, param.col_line);
      break;
    case uiPlotMarker::Diamond:
      for(size_t i = 0; i < n; i++) r->diamond(cvt_pos(v[i]), param.size, param.col_line);
      break;
    case uiPlotMarker::None:
    default:
      for(size_t i = 0; i < n; i++) r->circle(cvt_pos(v[i]), param.size, param.col_line);
      break;
  }
  if(param.label != "") {
    label_list.push_back({param.col_line, param.label});
  }
}

template void uiPlot::plotLine<int>(const int*, const size_t, const uiPlotParams& param);
template void uiPlot::plotLine<double>(const double*, const size_t, const uiPlotParams& param);
template void uiPlot::plotLine<float>(const float*, const size_t , const uiPlotParams& param);
template void uiPlot::plotLine<double>(const double*, const double*, const size_t , const uiPlotParams& param);
template void uiPlot::plotLine<int>(const int*, const int*, const size_t, const uiPlotParams& param);
template void uiPlot::plotScatter<double>(const _Vec<double, 2>* v, size_t n, const uiPlotParams& param);

void uiPlot::render() {
  if(!flags.Active) {
    needRendering(false);
    return;
  }
  if(!flags.needRendering) {
    return;
  }
  label_list.clear();
  auto r           = drawing_wnd()->get_renderer();
  const auto style = instance::get_style();
  r->rectPS(pos, size, {0, 0, 0});
  r->rectPS(pos, size, style->col_WidgetLine, 1);
  prange.set = false;
  if(title != "") r->put_text(title, pos, 1);
  needRendering(true);
  impl_needCalcAlinment_parent(); // TODO: 常に呼ばなくてもいいようにすること！
}

void uiPlot::plotOther() {
  auto r           = drawing_wnd()->get_renderer();
  if(!flags.Active) {
    needRendering(false);
    return;
  }
  // plot x values
  size_t nx = size[0] / 80;
  for(double x = prange.xmin(); x < prange.xmax(); x += prange.xsize() / nx) {
    const int p           = cvt_pos_x(x);
    const std::string str = std::to_string(x);
    r->put_text(str, {p, pos[1] + size[1] - 15}, 1);
    r->line(Vec2d(p, pos[1] + g_padding[1]), {p, int(pos[1] + size[1] - g_padding[1])}, {100, 100, 120}, 1);
  }

  // plot y values
  size_t ny = size[0] / 80;
  for(double y = prange.ymin(); y < prange.ymax(); y += prange.ysize() / ny) {
    const int p           = cvt_pos_y(y);
    const std::string str = std::to_string(y);
    r->put_text(str, {pos[0], p}, 1);
    r->line(Vec2d(pos[0] + g_padding[0], p), {int(pos[0] + size[0]), p}, Vec3b(100, 100, 120), 1);
  }

  // plot labels
  for(size_t t = 0; t < label_list.size(); t++) {
    const auto col  = label_list[t].first;
    const auto name = label_list[t].second;
    const int y     = pos[1] + t * 25 + g_padding[1];
    const int x     = int(pos[0] + g_padding[0]) + 15;
    r->rectPS({x, y}, {18, 18}, col);
    r->rectPS({x, y}, {18, 18}, {200, 200, 200}, 1);
    r->put_text(name, {x + 22, y}, 1);
  }
}

bool uiPlot::CallbackFunc([[maybe_unused]] uiCallbackFlags flag, [[maybe_unused]] Vec2d vec2_1, [[maybe_unused]] int num_1, [[maybe_unused]] int num_2, [[maybe_unused]] const char** strings) { return false; }


} // namespace mu::ui
