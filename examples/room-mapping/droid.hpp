#pragma once
#include <movutl/experimental/CSVreader.hpp>
#include <cwchar>
#include <movutl/db/body.hpp>
#include <movutl/mu.hpp>
#include <movutl/plugins/plugin_base.hpp>
#include <movutl/tools/mesh/meshtools.hpp>
#include <movutl/ui/colors.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>
#include <tuple>
#include <vector>

using namespace mu::db;
using namespace mu::render;
using namespace mu::instance;

using mu::plugin::PluginBase;
using mu::plugin::PluginInfo;
using mu::plugin::PluginParams;

// inline void down_sample(int n, Body* in, Body* out) {}

class DownsampleMod : public PluginBase {
public:
  int n = 15;

  DownsampleMod() = default;
  virtual PluginInfo get_plugin_info() { return PluginInfo(); }
  virtual void set_param(const PluginParams&) {}
  virtual PluginParams get_param() const { return PluginParams(); }
  virtual void registered() {}
  virtual void unregistered() {}

  virtual void apply(Body* in, Body* out) { mu::tools::downsample_all_mesh(in, out, n); }
};

class TransformModifier : public PluginBase {
public:
  Vec3 pos;
  Vec3 rot    = {0, 0, 0.9};
  float scale = 1;

  TransformModifier() = default;

  virtual PluginInfo get_plugin_info() { return PluginInfo(); }
  virtual void set_param(const PluginParams&) {}
  virtual PluginParams get_param() const { return PluginParams(); }
  virtual void registered() {}
  virtual void unregistered() {}

  virtual void apply(Body* in, Body* out) {
    MU_ASSERT(in != nullptr);
    MU_ASSERT(out != nullptr);
    MU_ASSERT(in->meshs.size() == 1);
    MU_ASSERT(in->meshs[0]->get_mesh_type() == _MeshBase::MeshType::Col);

    if(out->meshs.size() == 0) {
      const auto m = mu::instance::create_mesh_col();
      m->primitive_type(_MeshBase::PrimitiveType::POINTS);
      out->meshs.push_back(m);
    }

    MeshCol* mi = reinterpret_cast<MeshCol*>(in->meshs[0]);
    MeshCol* mo = reinterpret_cast<MeshCol*>(out->meshs[0]);

    if(mi->get_vertices_size() == 0) return;
    const auto centre = mi->get_center_of_geom();

    MeshCol::Vertex* ptr       = mi->vertex.begin();
    const MeshCol::Vertex* end = mi->vertex.end();

    const float c = std::cos(rot[2]);
    const float s = std::sin(rot[2]);
    mo->vertex.clear();

    for(; ptr <= end; ptr++) {
      const auto v   = Vec3(ptr->pos[0], ptr->pos[1], ptr->pos[2]) - centre;
      const double x = (v[0] * c - v[1] * s) * scale;
      const double y = (v[1] * c + v[0] * s) * scale;
      const double z = (v[2]) * scale;
      const Vec3b col(ptr->col[0], ptr->col[1], ptr->col[2]);
      const Vec2 uv(ptr->uv[0], ptr->uv[1]);
      mo->vertex.push_back(mu::db::MeshCol::Vertex(Vec3(x, y, z), col, uv));
    }
  }
};

class ClipModifier : public PluginBase {
public:
  Rect3D r;
  ClipModifier() {
    r.x.min = r.y.min = -15;
    r.x.max = r.y.max = 15;
    r.z.min           = 0;
    r.z.max           = 2;
  }
  virtual PluginInfo get_plugin_info() { return PluginInfo(); }
  virtual void set_param(const PluginParams&) {}
  virtual PluginParams get_param() const { return PluginParams(); }
  virtual void registered() {}
  virtual void unregistered() {}

  virtual void apply(Body* in, Body* out) { mu::tools::clip_all_mesh(in, out, r); }
};

using Odometry = CSVreader<float, 12>;

struct PCinfo {
  float xmin, xmax, ymin, ymax, zmin, zmax;
  int n;

  PCinfo() { reset(); }
  void reset() {
    xmin = ymin = zmin = 99999999.0;
    xmax = ymax = zmax = 0.0;
  }

  inline void add(float x, float y, float z) {
    xmin = std::min(xmin, x);
    ymin = std::min(ymin, y);
    zmin = std::min(zmin, z);

    xmax = std::max(xmax, x);
    ymax = std::max(ymax, y);
    zmax = std::max(zmax, z);

    n++;
  }

  void report() const {
    std::cout << "----  point cloud information -----" << std::endl;
    std::cout << "x = ( " << xmin << "\t" << xmax << ")" << std::endl;
    std::cout << "y = ( " << ymin << "\t" << ymax << ")" << std::endl;
    std::cout << "z = ( " << zmin << "\t" << zmax << ")" << std::endl;
    std::cout << "----  point cloud information -----" << std::endl;
  }
};


class ShelfBuild : public PluginBase {
public:
  float minarea = 100;
  std::vector<Rect> shelf_list;
  ShelfBuild() = default;
  virtual PluginInfo get_plugin_info() { return PluginInfo(); }
  virtual void set_param(const PluginParams&) {}
  virtual PluginParams get_param() const { return PluginParams(); }
  virtual void registered() {}
  virtual void unregistered() {}

  virtual void apply(Body* in, Body*) {
    MU_ASSERT(in != nullptr);
    MU_ASSERT(in->meshs.size() == 1);
    MU_ASSERT(in->meshs[0]->get_mesh_type() == _MeshBase::MeshType::Col);

    const Vec2d imgsize(200, 200);
    const Rect3D bbox   = in->get_bbox();
    const float scale_x = imgsize[0] / bbox.x.length();
    const float scale_y = imgsize[1] / bbox.y.length();
    // TODO: Windowsで↓だとエラーが出る。constexprコンストラクタを作る必要がありそう
    //  uint8_t img_ptr[imgsize[0] * imgsize[1]];
    uint8_t img_ptr[200 * 200];
    memset(img_ptr, 0, imgsize[0] * imgsize[1]);

    MeshCol* mi = reinterpret_cast<MeshCol*>(in->meshs[0]);

    if(mi->get_vertices_size() == 0) return;

    MeshCol::Vertex* ptr       = mi->vertex.begin();
    const MeshCol::Vertex* end = mi->vertex.end();
    for(; ptr <= end; ptr++) {
      const int x = scale_x * ptr->pos[0] + int(imgsize[0] / 2);
      const int y = scale_y * ptr->pos[1] + int(imgsize[1] / 2);
      if(x < 0 || x >= imgsize[0]) continue;
      if(y < 0 || y >= imgsize[1]) continue;
      /* *(img_ptr + y * imgsize[0] + x) = ptr->pos[2] * 10; */
      *(img_ptr + y * imgsize[0] + x) = 200;
    }
    cv::Mat bin(imgsize[0], imgsize[1], CV_8UC1, (void*)img_ptr);


    // ---------------    find contours   ---------------------
    /* cv::dilate(bin, bin, cv::noArray(), cv::Point(-1, -1), 3); */
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(bin, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);


    for(size_t i = 0; i < contours.size(); i++) {
      std::vector<cv::Point> contours_poly;
      cv::approxPolyDP(contours[i], contours_poly, 3, true);
      const auto boundRect = cv::boundingRect(contours_poly);
      const auto area      = boundRect.area();

      if(area < minarea) continue;
      Rect r;
      r.x.min = (boundRect.x - int(imgsize[0] / 2)) / scale_x;
      r.y.min = (boundRect.y - int(imgsize[1] / 2)) / scale_y;
      r.x.max = (boundRect.x + boundRect.width - int(imgsize[0] / 2)) / scale_x;
      r.y.max = (boundRect.y + boundRect.height - int(imgsize[1] / 2)) / scale_y;
      shelf_list.push_back(r);
    }

    // --- merge  shelfs-----:
    std::cout << "found " << shelf_list.size() << " shelfs merging........" << std::endl;
    bool ok = false;
    while(!ok && shelf_list.size() > 0) {
      ok = true;
      for(size_t i = 0; i < shelf_list.size() - 1; i++) {
        for(size_t j = i + 1; j < shelf_list.size(); j++) {
          if(!shelf_list[i].contains(shelf_list[j])) continue;
          ok = false;
          shelf_list[i].merge(shelf_list[j]);
          shelf_list.erase(shelf_list.begin() + j);
        }
      }
    }
    std::cout << "result --> " << shelf_list.size() << " shelfs " << std::endl;

    // ------  split into meshes -----------
    mi->indices.clear();
    for(ptr = mi->vertex.begin(); ptr < end; ptr++) {
      for(size_t i = 0; i < shelf_list.size(); i++) {
        if(shelf_list[i].contains(ptr->pos[0], ptr->pos[1])) {
          ptr->col[0] = mu::colors::colormap[i % 6][0];
          ptr->col[1] = mu::colors::colormap[i % 6][1];
          ptr->col[2] = mu::colors::colormap[i % 6][2];
          mi->indices.push_back(i);
          break;
        }
      }
    }
#if 0
    for(int i = 0; i < contours.size(); i++) cv::drawContours(bin, contours, i, cv::Scalar(255, 0, 0));
    for(auto&& s : shelf_list) {
      const int x    = s.x.min * scale_x + imgsize[0];
      const int y    = s.y.min * scale_y + imgsize[1];
      const int xmax = s.x.max * scale_x;
      const int ymax = s.y.max * scale_y;
      cv::rectangle(bin, cv::Point(x, y), cv::Point(xmax, ymax), cv::Scalar(0, 0, 255), 1);
    }
    cv::resize(bin, bin, cv::Size(), 2.0, 2.0);
    cv::imshow("a", bin);
    cv::waitKey(1);
#endif
  }
};



class ShelfAnalyze : public PluginBase {
public:
  float thickness = 0.2;

  enum ShelfDir {
    SHELF_DIR_LEFT = 0,
    SHELF_DIR_RIGHT,
    SHELF_DIR_UP,
    SHELF_DIR_DOWN,
  };
  struct Shelf {
    int shelf_id;
    Rect3D r;
    ShelfDir d;
    cv::Mat img[4];
    const Vec2d imgsize = {200, 100};

    Shelf(int shelf_idx, Rect r2d) {
      shelf_id = shelf_idx;
      r.x      = r2d.x;
      r.y      = r2d.y;
      r.z.min  = 0;
      r.z.max  = 2;
      for(int i = 0; i < 4; i++) img[i] = cv::Mat(imgsize[1], imgsize[0], CV_8UC1, 255);
    }
  };

  std::vector<Shelf> sl;

  ShelfAnalyze() = default;
  virtual PluginInfo get_plugin_info() { return PluginInfo(); }
  virtual void set_param(const PluginParams&) {}
  virtual PluginParams get_param() const { return PluginParams(); }
  virtual void registered() {}
  virtual void unregistered() {}


  virtual void apply(Body* in, Body*) {
    MU_ASSERT(in != nullptr);
    MU_ASSERT(in->meshs[0]->get_mesh_type() == _MeshBase::MeshType::Col);
    if(in->meshs.size() == 0) return;

    MeshCol* mi = reinterpret_cast<MeshCol*>(in->meshs[0]);
    if(mi->get_vertices_size() == 0) return;
    auto ptr         = mi->vertex.begin();
    const auto end   = mi->vertex.end();
    auto indices_ptr = mi->indices.begin();
    while(ptr <= end) {
      const int shelf_idx = *indices_ptr;
      for(int dir = 0; dir < 4; dir++) process_vert(ptr, shelf_idx, (ShelfDir)dir);
      ptr++;
      indices_ptr++;
    }
  }

  void process_vert(const MeshCol::Vertex* ptr, int shelf_idx, const ShelfDir dir) {
    const auto bbox       = sl[shelf_idx].r;
    const auto imgsize    = sl[shelf_idx].imgsize;
    const bool dir_x_axis = dir == SHELF_DIR_UP || dir == SHELF_DIR_DOWN;
    const float scale_x   = imgsize[0] / (dir_x_axis ? bbox.x.length() : bbox.y.length());
    const float scale_y   = imgsize[1] / bbox.z.length();
    thickness             = (dir_x_axis ? bbox.y.length() : bbox.x.length()) / 2.0f;
    const float scale_d   = 150.0f / thickness;

    uint8_t* img_ptr = sl[shelf_idx].img[(int)dir].data;
    /* if(!is_shelf(dir, pos, bbox, thickness)) return; */
    const auto dist = get_distance(dir, ptr->pos, bbox);
    const int x     = dist[0] * scale_x;
    const int y     = dist[1] * scale_y;
    const int depth = dist[2] * scale_d;
    if(x < 0 || y < 0 || x >= imgsize[0] || y >= imgsize[1]) return;
    const auto p = img_ptr + y * imgsize[0] + x;
    *p           = std::min<uint8_t>(*p, depth);
    return;
  }

  void display_img(int shelf_idx) {
    for(int d = 0; d < 4; d++) {
      cv::imshow(get_dir_name((ShelfDir)d), sl[shelf_idx].img[d]);
      cv::moveWindow(get_dir_name((ShelfDir)d), d * (sl[shelf_idx].imgsize[0] + 10), 10);
    }
    cv::waitKey(0);
  }

  const char* get_dir_name(const ShelfDir d) const {
    switch(d) {
      case SHELF_DIR_LEFT: return "left";
      case SHELF_DIR_RIGHT: return "right";
      case SHELF_DIR_UP: return "up";
      case SHELF_DIR_DOWN: return "down";
    }
  }

  Vec3 get_distance(const ShelfDir d, const float pos[3], const Rect3D& r) const {
    const auto y = r.z.max - pos[2];
    double x, z;
    switch(d) {
      case SHELF_DIR_LEFT:
        x = r.y.max - pos[1];
        z = pos[0] - r.x.min;
        break;
      case SHELF_DIR_RIGHT:
        x = r.y.max - pos[1];
        z = r.x.max - pos[0];
        break;
      case SHELF_DIR_UP:
        x = r.x.max - pos[0];
        z = pos[1] - r.y.min;
        break;
      case SHELF_DIR_DOWN:
        x = r.x.max - pos[0];
        z = r.y.max - pos[1];
        break;
    }
    return Vec3(x, y, z);
  }

  bool is_shelf(const ShelfDir d, const float pos[3], const Rect3D& r, float thickness) const {
    if(!r.contains(pos[0], pos[1], pos[2])) return false;
    switch(d) {
      case SHELF_DIR_LEFT: return pos[0] < r.x.min + thickness;
      case SHELF_DIR_RIGHT: return pos[0] > r.x.max - thickness;
      case SHELF_DIR_UP: return pos[1] < r.y.min + thickness;
      case SHELF_DIR_DOWN: return pos[1] > r.y.max - thickness;
    }
  }
};
