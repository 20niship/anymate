#pragma once

# include <opencv2/opencv.hpp>
# include <opencv2/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h> 


// Layer type

#define LAYER_TYPE_IMG         1000
#define LAYER_TYPE_MOVIE       1001
#define LAYER_TYPE_AUDIO       1002
#define LAYER_TYPE_POLYGON     1003
#define LAYER_TYPE_CUSTOM_OBJ  1004

#define LAYER_TYPE_SCENE        2001
#define LAYER_TYPE_SCENE_AUDIO  2002

#define LAYER_TYPE_GROUP        3001
#define LAYER_TYPE_TIME         3002
#define LAYER_TYPE_CAMERA       3003


#define LAYER_TYPE_FRAME_BUFFER  4001
#define LAYER_TYPE_BEFORE_OBJ    4002
#define LAYER_TYPE_LAYER_COPY    4003

#define LAYER_TYPE_EFFECT        5004


// Overlay type
#define OVERLAY_TYPE_NORMAL   10000
#define OVERLAY_TYPE_ADD     10001
#define OVERLAY_TYPE_MULTIPLICATION 10002
#define OVERLAY_TYPE_SCREEN    10003
#define OVERLAY_TYPE_OVERLAY   10004
#define OVERLAY_TYPE_COMPARE_1  100006
#define OVERLAY_TYPE_COMPARE_2  100007

#define OVERLAY_TYPE_CUSTOM    10008


struct layerObj{
	cv::Mat img = cv::Mat::zeros(10, 10, CV_8UC4);;
	cv::VideoCapture cap;
	std::vector<uint8_t> img_alpha;

	bool enable_alpha = false; //true -> アルファを使って合成　false -> そのまま（アルファを考慮しない）
	
	//Activeかどうか
	bool isSolo = false;
	bool isMute = false;
	bool isActive = isSolo || !isMute;

	//各種設定
	int start = -1;     //スタート位置
	int end = -1;       //最終位置
	int layer_num = 0;  //レイヤー番号
	int width = 0;
	int height = 0;
	float anchor_x = 0; //基準点のX座標　（デフォルトでは width / 2）
	float anchor_y = 0;

	//TODO: x, yを変化させるときはどうするの？
	float move_x = 0; //基準点のX座標の移動距離（デフォルトでは0）
	float move_y = 0;

	int type = LAYER_TYPE_IMG;
	int overlay_mode = OVERLAY_TYPE_NORMAL;

	cv::Vec4b getColor(int x, int y) {
		x = MAX(MIN((img).cols, x), 1);
		y = MAX(MIN((img).rows, x), 1);

		cv::Vec4b *ptr = img.ptr<cv::Vec4b>(y);
		cv::Vec4b color = ptr[x];
		return color;
	}

	void setImage(cv::Mat src, cv::Mat alpha) {
		img = src.clone();
		img_alpha = alpha;
		anchor_x = float(img.cols) / 2.0f; anchor_y = float(img.rows) / 2.0f;
		width = img.cols;  height = img.rows;
		move_x = 0; move_y = 0;
	}

	void setImage(cv::Mat src) {
		img = src.clone();
		if (src.type() == CV_8SC3) cv::cvtColor(img, img, cv::COLOR_RGB2RGBA);
		if (src.type() == CV_8SC1) cv::cvtColor(img, img, cv::COLOR_GRAY2BGRA);
		
		anchor_x = float(img.cols) / 2.0f; anchor_y = float(img.rows) / 2.0f;
		width = img.cols;  height = img.rows;
		move_x = 0; move_y = 0;
	}

	void setCap(cv::VideoCapture cap_) {
		cap = cap_;
		anchor_x = float(img.cols) / 2.0f; anchor_y = float(img.rows) / 2.0f;
		//TODO: capのwidthとheightを設定
		move_x = 0; move_y = 0;
	}

};


class RenderingObj {
public:
	RenderingObj(int, int);
	~RenderingObj();

	cv::Mat Cur_img;

	bool enable_alpha = true; //true -> アルファを使って合成　false -> そのまま（アルファを考慮しない）
	
	void setRenderSize(int, int);
	void setPlayStartPos(int);
	void getFrame(int);
	void clearImage();
	
	void RenderAndAddOneLayer(layerObj *);
	void _overlay_normal(layerObj *);
	void _overlay_add(layerObj *);
	void _overlay_multiply(layerObj *);
	void _overlay_screen(layerObj *);
	void _overlay_overlay(layerObj *);
	void _overlay_compare_1(layerObj *);
	void _overlay_compare_2(layerObj *);


private:
	//各種設定
	int start = -1;     //スタート位置
	int end = -1;       //最終位置
	int CFN = 0;    //current frame number
	int rendering_layer = 0;  //現在レンダリングを行っているレイヤー番号
	int const_width = 0;    //出力ファイルの大きさ
	int const_height = 0;
	float const_center_x = 0;
	float const_center_y = 0;
};

struct SceneObj {
	//TODO: make SceneObj
};

