#if 0

#include "CSVreader.hpp"
#include <mu.hpp>
#include <opencv2/opencv.hpp>
#include <tuple>
#include <vector>

using namespace mu::db;
using namespace mu;
using namespace mu::instance;



class RoomBuilder {
private:
  float get_shelf_height(const Rect3D& shelf) {
    int n   = 0;
    float h = 0.0;

    const auto [xmin, xmax] = deproject(shelf.x.min, shelf.x.max);
    const auto [ymin, ymax] = deproject(shelf.y.min, shelf.y.max);
    char* data              = (char*)height_map.data;

    for(int y = ymin; y < ymax; y++) {
      for(int x = xmin; x < xmax; x++) {
        n++;
        if(x < 0 || x >= mat_width || y < 0 || y >= mat_height) continue;
        h += (float)data[y * mat_width + x] / 50.0;
      }
    }
    return h / n;
  }

  inline std::tuple<float, float> deproject(float x, float y) {
    const float mat_scale_x = (float)mat_width / (info.xmax - info.xmin);
    const float mat_scale_y = (float)mat_height / (info.ymax - info.ymin);
    const float x_          = x / mat_scale_x;
    const float y_          = y / mat_scale_y;
    return std::make_tuple(x_, y_);
  }

  inline std::tuple<float, float> project(float x, float y) {
    const float cos         = std::cos(rot);
    const float sin         = std::sin(rot);
    const float mat_scale_x = (float)mat_width / (info.xmax - info.xmin);
    const float mat_scale_y = (float)mat_height / (info.ymax - info.ymin);
    const int x_            = mat_scale_x * (cos * x - sin * y - info.xmin);
    const int y_            = mat_scale_y * (sin * x + cos * y - info.ymin);
    return std::make_tuple(x_, y_);
  }


public:
  float floor_height;
  int z_max;
  float scale;
  float rot;
  float minarea;

  int mat_width, mat_height;
  PCinfo info;

  std::vector<Rect3D> shelf_list;

  cv::Mat result;
  cv::Mat height_map;

  Body* b;

  void set_body(Body* b_) {
    MU_ASSERT(b != nullptr);
    MU_ASSERT(b->meshs.size() > 0);
    b = b_;
  }

  void construct() {
    const float sin = std::sin(rot);
    const Rect3D bbox = b->get_bbox();

    info.reset();
    std::cout << "getting information,,,,,,,," << std::endl;
    for(size_t i = 0; i < vx.size(); i++) {
      assert(i < vy.size());
      assert(i < vz.size());
      const float x = cos * vx[i] - sin * vy[i];
      const float y = sin * vx[i] + cos * vy[i];
      info.add(x, y, vz[i]);
    }
    info.report();

    height_map = cv::Mat(mat_width, mat_height, CV_8UC1);
    result     = cv::Mat(mat_width, mat_height, CV_8UC3, cv::Scalar(0, 0, 0));

    char* img_ptr    = (char*)height_map.data;
    char* result_ptr = (char*)result.data;

    const float mat_scale_x = (float)mat_width / (info.xmax - info.xmin);
    const float mat_scale_y = (float)mat_height / (info.ymax - info.ymin);

    for(size_t i = 0; i < vx.size(); i++) {
      if(vz[i] < floor_height) continue;
      if(vz[i] > z_max) continue;

      const float x_ = vx[i];
      const float y_ = vy[i];
      const int x    = mat_scale_x * (cos * x_ - sin * y_ - info.xmin);
      const int y    = mat_scale_y * (sin * x_ + cos * y_ - info.ymin);

      if(x < 0 || x >= mat_width) continue;
      if(y < 0 || y >= mat_height) continue;

      char* px                       = result_ptr + (y * mat_width + x) * 3;
      *px                            = vr[i];
      *(px + 1)                      = vg[i];
      *(px + 2)                      = vb[i];
      *(img_ptr + y * mat_width + x) = vz[i] * 10;
    }
  }

  void get_rotation() {
    cv::Mat bin;
    cv::threshold(height_map, bin, 0, 255, cv::THRESH_BINARY);
    cv::dilate(bin, bin, cv::noArray(), cv::Point(-1, -1), 3);
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(bin, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat img;
    cv::cvtColor(height_map, img, cv::COLOR_GRAY2BGR);

    float angle_sum = 0;
    int n           = 0;

    for(auto&& c : contours) {
      const auto minRect = minAreaRect(c);
      const auto angle   = minRect.angle;
      const auto area    = minRect.size.area();
      const auto ratio   = minRect.size.width / minRect.size.height;
      if(area < minarea) continue;
      if(0.3 < ratio && ratio < 3) continue;
      const int angle2 = int(angle * 180.0 / 3.141592) % 90;
      n++;
      angle_sum += angle2;

      cv::Point2f vertices[4];
      minRect.points(vertices);
      for(int i = 0; i < 4; i++) cv::line(img, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0), 2);
    }

    cv::imshow("hoge", img);
    rot = -(angle_sum / n) * 3.141592 / 180.0;
    DISP(rot);
  }


  void find_shelf() {
    cv::Mat bin;
    cv::threshold(height_map, bin, 0, 255, cv::THRESH_BINARY);
    cv::dilate(bin, bin, cv::noArray(), cv::Point(-1, -1), 3);
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(bin, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    const float mat_scale_x = (float)mat_width / (info.xmax - info.xmin);
    const float mat_scale_y = (float)mat_height / (info.ymax - info.ymin);

    for(size_t i = 0; i < contours.size(); i++) {
      std::vector<cv::Point> contours_poly;
      cv::approxPolyDP(contours[i], contours_poly, 3, true);
      const auto boundRect = cv::boundingRect(contours_poly);
      const auto area      = boundRect.area();

      if(area < minarea) continue;

      const float x = boundRect.x / mat_scale_x;
      const float y = boundRect.y / mat_scale_y;
      const float w = boundRect.width / mat_scale_x;
      const float h = boundRect.height / mat_scale_y;
      const float z = 0;
      shelf_list.push_back(Rect3D(x, y, w, h, z));
    }

    // --- merge  shelfs-----:
    std::cout << "found " << shelf_list.size() << " shelfs \n merging........" << std::endl;
    bool ok = false;
    while(!ok && shelf_list.size() > 0) {
      ok = true;
      for(size_t i = 0; i < shelf_list.size() - 1; i++) {
        for(size_t j = i + 1; j < shelf_list.size(); j++) {
          if(!shelf_list[i].contains(shelf_list[j])) continue;
          ok               = false;
          const auto merge = shelf_list[i].merge(shelf_list[j]);
          shelf_list[i]    = merge;
          shelf_list.erase(shelf_list.begin() + j);
        }
      }
    }
    std::cout << "result --> " << shelf_list.size() << " shelfs \n " << std::endl;

    std::cout << "mapping height ........." << std::endl;
    for(size_t i = 0; i < shelf_list.size(); i++) {
      shelf_list[i].height = get_shelf_height(shelf_list[i]);
    }
    std::cout << "Done!" << std::endl;
  }

  void draw_shelf(cv::Scalar col = cv::Scalar(255, 100, 0)) {
    const float mat_scale_x = (float)mat_width / (info.xmax - info.xmin);
    const float mat_scale_y = (float)mat_height / (info.ymax - info.ymin);

    int idx = 0;
    for(auto&& s : shelf_list) {
      const int x    = s.x.min * mat_scale_x;
      const int y    = s.y.min * mat_scale_y;
      const int xmax = s.x.max * mat_scale_x;
      const int ymax = s.y.max * mat_scale_y;
      cv::rectangle(result, cv::Point(x, y), cv::Point(xmax, ymax), col, 1);

      const std::string text = "S" + std::to_string(idx) + " a=" + std::to_string(int(s.area() * 10));
      idx++;
      cv::rectangle(result, cv::Point(x, y), cv::Point(x + 100, y + 20), col, -1);
      cv::putText(result, text.c_str(), cv::Point(x, y + 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255));
    }
  }

  void draw_grid(int n = 10, cv::Scalar col = cv::Scalar(100, 100, 100)) {
    for(int i = 0; i < 10; i++) {
      int k = mat_height * i / 10;
      int j = mat_width * i / 10;
      cv::line(result, cv::Point(0, k), cv::Point(mat_width, k), col, 1);
      cv::line(result, cv::Point(j, 0), cv::Point(j, mat_height), col, 1);
    }
  }
};

#endif

