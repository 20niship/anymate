#pragma once
#include <db/image.hpp>

namespace mu::tool {

#if 0

inline void calcNormal(const db::Image& pos, db::Image& normals) {
  // method 1 「Real-Time Plane Detection Based on Depth Map from Kinect」の場合
  //上下左右の4点から頑張る
  assert(pos.width == normals.width);
  assert(pos.height == normals.height);

  /* #pragma omp parallel for */
  for(int x = 0; x < pos.width(); x++) {
    for(int y = 0; y < pos.height(); y++) {
      const auto up    = (y == 0) ? pos(x, 0) : pos(x, y - 1);
      const auto down  = (y == pos.height()) ? pos(x, y) : pos(x, y + 1);
      const auto left  = (x == 0) ? pos(x, y) : pos(x - 1, y);
      const auto right = (x == pos.width()) ? pos(x, y) : pos(x + 1, y);

      const auto dx = right - left;
      const auto dy = up - down;
      normals(x, y) = dx.cross(dy).normalize();
    }
  }

  // TODO: 最小二乗法を使ったNormal Estimation
  // TODO: depth=0をのぞいたNormalEstimation
}

inline void calcNormal(const uint16_t* depth_buf, db::Image& normals, const db::CamInfo& cam) {
  // method 2
  // reference : https://stackoverflow.com/questions/34644101/calculate-surface-normals-from-depth-image-using-neighboring-pixels-cross-produc
  assert(depth_buf != nullptr);

  /* #pragma omp parallel for */
  for(int x = 1; x < cam.width - 1; x++) {
    for(int y = 1; y < cam.height - 1; y++) {
      const float dx   = depth_buf[x + 1 + y * cam.width] - depth_buf[x - 1 + y * cam.width];
      const float dy   = depth_buf[x + (y + 1) * cam.width] - depth_buf[x + (y - 1) * cam.width];
      const float mag  = sqrt(dx * dx + dy * dy + 1);
      normals(x, y)[0] = dx / mag;
      normals(x, y)[1] = dy / mag;
      normals(x, y)[2] = 1.0 / mag;
    }
  }
}

inline void calcNormal(const uint16_t* depth_buf, cv::Mat& normals, db::CamInfo cam) {
  // method 2
  // reference : https://stackoverflow.com/questions/34644101/calculate-surface-normals-from-depth-image-using-neighboring-pixels-cross-produc
  assert(depth_buf != nullptr);
  assert(normals.cols == cam.width);
  assert(normals.rows == cam.height);

  /* #pragma omp parallel for */
  for(int y = 1; y < cam.height - 1; y++) {
    cv::Vec3b* ptr = normals.ptr<cv::Vec3b>(y);
    for(int x = 1; x < cam.width - 1; x++) {
      const float dx  = depth_buf[x + 1 + y * cam.width] - depth_buf[x - 1 + y * cam.width];
      const float dy  = depth_buf[x + (y + 1) * cam.width] - depth_buf[x + (y - 1) * cam.width];
      const float mag = sqrt(dx * dx + dy * dy + 1);
      ptr[x][0]       = (dx / mag + 1.0) * 128.0f;
      ptr[x][1]       = (dy / mag + 1.0) * 128.0f;
      ptr[x][2]       = (1.0 / mag + 1.0) * 128.0f;
    }
  }
}

inline void getNormalRGB(const db::Image& normals) {
  assert(normals.width > 0);
  assert(normals.height > 0);
  cv::Mat img(cv::Size(normals.height, normals.width), CV_8UC3);
  for(int y = 0; y < normals.height(); y++) {
    cv::Vec3b* ptr = img.ptr<cv::Vec3b>(y);
    for(int x = 0; x < normals.width(); x++) {
      // const auto n = normals(x, y);
      ptr[x][0] = 0;   //(n[0] + 1.0)*128.0f;
      ptr[x][1] = 10;  //(n[1] + 1.0)*128.0f;
      ptr[x][2] = 100; // (n[2] + 1.0)*128.0f;
    }
  }
}

inline void absdiff(db::Image img1, db::Image img2, db::Image diff) {}


#endif 

} // namespace mu::tool
