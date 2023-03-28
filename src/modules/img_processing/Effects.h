#pragma once

# include <opencv2/opencv.hpp>
# include <opencv2/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h> 


///////////////////////////////////////////////////
//                  幾何学的変換　　　　　　　　 //
///////////////////////////////////////////////////


bool rotate(cv::Mat *, float);
bool Zoom(cv::Mat *, float, float,float,float);
bool Zoom_simple(cv::Mat *, float, float);
bool Clipping(cv::Mat *, float, float, float, float);
bool ColorTragger(cv::Mat, int, int, int, int);
bool ColorTragger_easy(cv::Mat *, int, int);
bool Flip(cv::Mat *, int, int);



///////////////////////////////////////////////////
//                 各種フィルタ 　　　　　　　　 //
///////////////////////////////////////////////////

bool ToneCorrection(cv::Mat *, float, float);
bool ColorCorrection(cv::Mat *, int, int, int);
bool ColorCorrectionRGB(cv::Mat *, int, int, int, float, float, float);
bool blur(cv::Mat *, int, int);
bool blur_g(cv::Mat *, int, int, int);
bool Binarize(cv::Mat *, int);
bool MultiSlicer(cv::Mat *, float);
bool Radiation_blur(cv::Mat *, int, int, int, int);
