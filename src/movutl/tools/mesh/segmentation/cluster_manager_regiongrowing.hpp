#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include <movutl/tools/magi/cluster.hpp>
#include <movutl/core/vector.hpp>
/* #includmovutl/e "region_growing.hpp" */

// https://github.com/imLogM/multi_seed_region_grow

#define MAX_CLUSTER_NUM 150

#define MAGI_REGION_NOTDEFINED 254
#define MAGI_REGION_INVALID 255
#define MAGI_REGION_BACKGROUND 253

namespace mu::Magi{

// cv::Matを使わないパターン
class ClusterManagerRegionGrowing{
public:
    int threshold, threshold_2ndtry;
    int max_region_num, region_n;
    double min_region_area_factor;
    core::Vec2d PointShift2D[8];
    int rgb_sum[3];
    Magi::Cluster<core::Vec3> *clusters[MAX_CLUSTER_NUM];
    // std::vector<Magi::Cluster<core::Vec3> > clusters;

    int width, height, zoom_ratio;
    db::CamInfo cam;
    uint8_t *src, *result;

    ClusterManagerRegionGrowing();
    void setDefaultClusters();
    void setSrc(uint8_t *_src, const int w, const int h, const int z);
    void setDefaultMask();
    void setMask(uint8_t *mask);
    void setIgnoreAt(int x, int y);
    void setThreshould(int t){ threshold = t; }
    void setThreshould2(int t){ threshold_2ndtry = t; }
    void setMaxRegionNum(uint8_t n){ max_region_num= n; }
    void setMinRegionAreaFactor(double t){ min_region_area_factor= t; }
    uint8_t  getRegionNum(){ return region_n; }

    // void grow(cv::Mat& src, cv::Mat& mask, cv::Point seed, int threshold) {
    void grow(const int x, const int y) ;
    void apply();
    cv::Mat ColorMap(cv::Mat &result_image);
    void setCamInfo(const db::CamInfo &c);
    void ground_esimate();
};

} // namespace Magi::2D


