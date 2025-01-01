#pragma once
#include <initializer_list>
#include <unordered_map>
#include <algorithm>

#include <movutl/ui/ui.hpp>
#include <movutl/ui/widget.hpp>

namespace mu::ui {

enum class uiPlotMarker{None, Circle, Square, Diamond, Up, Down, Left, Right, Cross, Plus, Asterisk, Count };

class uiPlotParams{
private:
  std::string tolower(std::string s)const {
    std::transform(s.begin(), s.end(), s.begin(), [](const unsigned char c){ return std::tolower(c); });
    return s;
  }
  
  uiPlotMarker get_marker_type(const std::string &s_) const {
    std::string s = s_;
    const std::string lower = tolower(s);
    if(lower == "none") return uiPlotMarker::None;
    if(lower == "circle") return uiPlotMarker::Circle;
    if(lower == "diamond") return uiPlotMarker::Diamond;
    if(lower == "up") return uiPlotMarker::Up;
    if(lower == "down") return uiPlotMarker::Down;
    if(lower == "left") return uiPlotMarker::Left;
    if(lower == "right") return uiPlotMarker::Right;
    if(lower == "cross") return uiPlotMarker::Cross;
    if(lower == "plus") return uiPlotMarker::Plus;
    if(lower == "asterisk") return uiPlotMarker::Asterisk;
    if(lower == "count") return uiPlotMarker::Count;
    return uiPlotMarker::None;
  }

  Vec3b get_col(const std::string &s) const {
   const std::unordered_map<std::string, int> cvt = {
     {"0",0},{"1",1},{"2",2},{"3",3},{"4",4},{"5",5},{"6",6},{"7",7},{"8",8},{"9",9},
     {"a",10},{"b",11},{"c",12},{"d",13},{"e",14},{"f",15},
     {"A",10},{"B",11},{"C",12},{"D",13},{"E",14},{"F",15}
   };
   const std::unordered_map<std::string, Vec3b> known = {
     {"white",{255,255,255}},{"black",{0,0,0}},{"red",{255,0,0}},{"blue",{0,0,255}}, {"green",{0,255,0}}, 
     {"gray",{127,127,127}}, {"yellow",{255,0,255}}
   };

   if(s.length() == 3){
     const uint8_t r = cvt.contains(s.substr(0,1)) ? cvt.at(s.substr(0,1)) * 16 : 0;
     const uint8_t g = cvt.contains(s.substr(1,1)) ? cvt.at(s.substr(1,1)) * 16 : 0;
     const uint8_t b = cvt.contains(s.substr(2,1)) ? cvt.at(s.substr(2,1)) * 16 : 0;
     return {r, g, b};
   }else if(s.length() == 6){
     const uint8_t r = cvt.contains(s.substr(0,1)) && cvt.contains(s.substr(1,1)) ? 
        cvt.at(s.substr(0,1)) * 16 + cvt.at(s.substr(1,1)) : 0;
     const uint8_t g = cvt.contains(s.substr(2,1)) && cvt.contains(s.substr(3,1)) ? 
        cvt.at(s.substr(2,1)) * 16 + cvt.at(s.substr(3,1)) : 0;
     const uint8_t b = cvt.contains(s.substr(4,1)) && cvt.contains(s.substr(5,1)) ? 
        cvt.at(s.substr(4,1)) * 16 + cvt.at(s.substr(5,1)) : 0;
     return {r, g, b};
   }else{
     return {0,0,0};
   } 
  }

public:
  std::string label{""};
  Vec3b col_line{255,255,255};
  Vec3b col_fill{200,200,200};
  uiPlotMarker marker{ uiPlotMarker::None };
  int size{6}; // for marker size
  uiPlotParams(){}
  uiPlotParams(std::initializer_list<std::pair<std::string, std::string> > l){
    for(auto ll : l){
      const std::string first = ll.first;
      const std::string second = ll.second;
      if(first == "label")label = second; 
      else if(first == "marker") marker = get_marker_type(second); 
      else if(first == "col") col_line = col_fill = get_col(second);
      else if(first == "col_line") col_line = get_col(second);
      else if(first == "size") size = std::stoi(second);
    }
  }
};

struct uiPlotRange{
  Vec2 xlim, ylim;
  bool set{ false };
  uiPlotRange(){ set = false; }
  uiPlotRange(const Vec2 &xlim_, const Vec2 &ylim_){ xlim = xlim_; ylim = ylim_; set = true; }
  uiPlotRange(const Vec2 &xlim_, const Vec2 &ylim_, const double padding ){ xlim = xlim_ + Vec2(-padding, padding); ylim = ylim_+ Vec2(-padding, padding); set = true; }
  double xmin() const { return xlim[0]; }
  double xmax() const { return xlim[1]; }
  double ymin() const { return ylim[0]; }
  double ymax() const { return ylim[1]; }
  double xsize()const { return xlim[1] - xlim[0]; }
  double ysize()const { return ylim[1] - ylim[0]; }
  Vec2 min(){ return {xlim[0], ylim[0]}; }
  Vec2 max(){ return {xlim[1], ylim[1]}; }
};


class uiPlot : public uiWidget{
private:
  std::string title{""};
  uiPlotRange prange;
  std::vector<std::pair<Vec3b,std::string> > label_list;

  template<typename T> inline int cvt_pos_x(const T v)const { assert(prange.set); return (v - prange.xmin())*size[0]/prange.xsize() + pos[0]; }
  template<typename T> inline int cvt_pos_y(const T v)const { assert(prange.set); return (v - prange.ymin())*size[1]/prange.ysize() + pos[1]; }
  template<typename T> inline Vec2d cvt_pos(const _Vec<T, 2> v)const{ assert(prange.set); return {cvt_pos_x(v[0]), cvt_pos_y(v[1])}; }
  inline Vec2d cvt_pos(const double x, const double y)const{ assert(prange.set); return {cvt_pos_x(x), cvt_pos_y(y)}; }
  template<typename T> Vec2 getRange(const T *v, size_t size) const ;
  template<typename T> void setPlotRange(const T *v, size_t size, const bool isXrange);
  template<typename T> void setPlotRange(const T *x, const T *y, size_t size);
  template<typename T> void setPlotRange(const _Vec<T,2> *v, size_t n);
  template<typename T> inline void setPlotRangeX(const T *x, size_t n){ setPlotRange(x, n, true); }
  template<typename T> inline void setPlotRangeY(const T *x, size_t n){ setPlotRange(x, n, false); }
public:
  Vec2 g_padding{15,15}; // pos, sizeの領域からg_paddingだけ内側の領域がグラフのプロット領域となる
	uiPlot();
  uiPlot(const std::string &title_);
	uiPlot(const uiPlot&) = delete;
	uiPlot& operator=(const uiPlot&) = delete;
	void render() override;
	void setTitle(const std::string &str){ title = str; needRendering(true); }
	std::string getTitle() const { return title; }

  // ---------------  plotting Functions ------------------
  void setPlotRange(const uiPlotRange &pr){ prange = pr; }
  const uiPlotRange &getPlotRange() const { return prange; }
  void plotOther();
  template<typename T> void plotLine   (const T *v, size_t n, const uiPlotParams &param={});
  template<typename T> void plotLine   (const T *x, const T *y, size_t n, const uiPlotParams &param={});
  template<typename T> void plotScatter(const _Vec<T, 2> *v, size_t n, const uiPlotParams &params={});

  // template<typename T> void plotBar (const std::string &label, const T *nums, size_t size){}
  /* template<typename T> void plotScatter(const std::string &label, const T *x, const T *y, size_t size); */
  /* void plotEnd(); */
  /* void renderTitle(); */
  /* void renderLabels(); */

	bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char **strings) override;
};



} // namespace vkUI
