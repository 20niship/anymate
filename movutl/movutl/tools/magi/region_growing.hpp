#pragma once

#include <iostream>
#include <stack>
#include <cassert>
#include <opencv2/opencv.hpp>

// https://github.com/imLogM/multi_seed_region_grow

namespace mu::Magi{

class RegionGrowingRGB{
public:
    cv::Mat src, dest, mask;
    int threshold, threshold_2ndtry;
    uint8_t max_region_num, region_n;
    double min_region_area_factor;
    cv::Point PointShift2D[8];

    RegionGrowingRGB(){
        threshold = 20;
        max_region_num = 100;
        min_region_area_factor = 0.01;
        PointShift2D[0] = cv::Point(1, 0);
        PointShift2D[1] = cv::Point(1, -1);
        PointShift2D[2] = cv::Point(0, -1);
        PointShift2D[3] = cv::Point(-1, -1);
        PointShift2D[4] = cv::Point(-1, 0);
        PointShift2D[5] = cv::Point(-1, 1);
        PointShift2D[6] = cv::Point(0, 1);
        PointShift2D[7] = cv::Point(1, 1);
    }

    void setSrc(cv::Mat& _src){ 
        src = _src; 
        region_n = 0;
        dest = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
        mask = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
    }

    void setMask(cv::Mat &img){
        assert(img.cols == mask.cols);
        assert(img.rows == mask.rows);
        mask = img;
    }

    void setIgnoreAt(int x, int y){
        assert(mask.cols >= x && x >= 0 && y >= 0 && mask.rows >= y);
        mask.at<uint8_t>(cv::Point(x, y)) = 255;
    }

    void setThreshould(int t){ threshold = t; }
    void setThreshould2(int t){ threshold_2ndtry = t; }
    void setMaxRegionNum(uint8_t n){ max_region_num= n; }
    void setMinRegionAreaFactor(double t){ min_region_area_factor= t; }
    uint8_t  getRegionNum(){ return region_n; }

    void grow(cv::Mat& src, cv::Mat& dest, cv::Mat& mask, cv::Point seed, int threshold) {
        /* apply "seed grow" in a given seed
        * Params:
        *   src: source image
        *   dest: a matrix records which pixels are determined/undtermined/ignored
        *   mask: a matrix records the region found in current "seed grow"
        */
        std::stack<cv::Point> point_stack;
        point_stack.push(seed);

        while(!point_stack.empty()) {
            cv::Point center = point_stack.top();
            mask.at<uint8_t>(center) = 1;
            point_stack.pop();

            for (int i=0; i<8; ++i) {
                cv::Point estimating_point = center + PointShift2D[i];
                if (estimating_point.x < 0
                    || estimating_point.x > src.cols-1
                    || estimating_point.y < 0
                    || estimating_point.y > src.rows-1) {
                    // estimating_point should not out of the range in image
                    continue;
                } else {
    //                uint8_t delta = (uint8_t)abs(src.at<uint8_t>(center) - src.at<uint8_t>(estimating_point));
                    // delta = (R-R')^2 + (G-G')^2 + (B-B')^2
                    int delta = int(pow(src.at<cv::Vec3b>(center)[0] - src.at<cv::Vec3b>(estimating_point)[0], 2)
                                    + pow(src.at<cv::Vec3b>(center)[1] - src.at<cv::Vec3b>(estimating_point)[1], 2)
                                    + pow(src.at<cv::Vec3b>(center)[2] - src.at<cv::Vec3b>(estimating_point)[2], 2));
                    if (dest.at<uint8_t>(estimating_point) == 0
                        && mask.at<uint8_t>(estimating_point) == 0
                        && delta < threshold) {
                        mask.at<uint8_t>(estimating_point) = 1;
                        point_stack.push(estimating_point);
                    }
                }
            }
        }
    }

    void apply(){
        int min_region_area = int(min_region_area_factor * src.cols * src.rows);  // small region will be ignored
        // 0 - undetermined, 255 - ignored, other number - determined
        uint8_t padding = 1;  // use which number to pad in "dest"

        // 4. traversal the whole image, apply "seed grow" in undetermined pixels
        for (int x=0; x<src.cols; ++x) {
            for (int y=0; y<src.rows; ++y) {
                if (dest.at<uint8_t>(cv::Point(x, y)) == 0) {
                    grow(src, dest, mask, cv::Point(x, y), threshold);
                    int mask_area = (int)cv::sum(mask).val[0];  // calculate area of the region that we get in "seed grow"
                    if (mask_area > min_region_area) {
                        dest = dest + mask * padding;   // record new region to "dest"
                        // cv::imshow("mask", mask*255);
                        // cv::waitKey();
                        if(++padding > max_region_num) { 
                            printf("run out of max_region_num..."); 
                            region_n = max_region_num;
                            return ; 
                        }
                    } else {
                        dest = dest + mask * 255;   // record as "ignored"
                    }
                    mask = mask - mask;     // zero mask, ready for next "seed grow"
                }
            }
        }
        // check again(わからなかった場所を埋める)
        region_n = padding-1;
    }

    cv::Mat ColorMap(cv::Mat &result_image){
        const auto default_color_map = Magi::get_default_color_map();
        result_image = cv::Mat::zeros(src.rows, src.cols, CV_8UC3);
        for (int x=0; x<dest.cols; ++x) {
            for (int y = 0; y < dest.rows; ++y) {
                const int idx = dest.at<uint8_t>(cv::Point(x, y));
                if(idx > 0 && idx<255){
                    result_image.at<cv::Vec3b>(cv::Point(x, y)) = default_color_map[(idx+1) % default_color_map.size()];
                } else if (dest.at<uint8_t>(cv::Point(x, y)) == 255) {
                    result_image.at<cv::Vec3b>(cv::Point(x, y))[0] = 255;
                    result_image.at<cv::Vec3b>(cv::Point(x, y))[1] = 255;
                    result_image.at<cv::Vec3b>(cv::Point(x, y))[2] = 255;
                }
            }
        }
        return result_image;
    }
};

} // namespace Magi::2D

