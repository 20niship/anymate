# include <opencv2/opencv.hpp>
# include <opencv2/highgui.hpp>
#include <initializer_list>
#include <iostream>
#include <vector>
#include <algorithm>

#define ESCAPE_KEY 27

void do_nothing(int size, void*) {}


int mode = 2;  //select mode 0, 1, or 2
cv::Mat img;
#define BLACK_VALUE 100
#define WHITE_VALUE -600
#define BRIGHTNESSVALUE 30

int blackValue = -10000000;
int brigthnessValue = 30;
int whiteValue = -6000000;

int row = 0;
int column = 0;

int width = 0;
int height = 0;


//void sortRow() {
//	int x = 0;
//	int y = row;
//	int xend = 0;
//
//	while (xend < width - 1) {
//		switch (mode) {
//		case 0:
//			x = getFirstNotBlackX(x, y);
//			xend = getNextBlackX(x, y);
//			break;
//		case 1:
//			x = getFirstBrightX(x, y);
//			xend = getNextDarkX(x, y);
//			break;
//		case 2:
//			x = getFirstNotWhiteX(x, y);
//			xend = getNextWhiteX(x, y);
//			break;
//		default:
//			break;
//		}
//
//		if (x < 0) break;
//
//		int sortLength = xend - x;
//
//		color[] unsorted = new color[sortLength];
//		color[] sorted = new color[sortLength];
//
//		for (int i = 0; i < sortLength; i++) {
//			unsorted[i] = img.pixels[x + i + y * img.cols];
//		}
//
//		sorted = sort(unsorted);
//
//		for (int i = 0; i < sortLength; i++) {
//			img.pixels[x + i + y * img.cols] = sorted[i];
//		}
//
//		x = xend + 1;
//	}
//}
//

//
//int rgb_to_int(cv::Vec3b color) {
//	return color[0] * 65025 + color[1] * 255 + color[2];
//}
//
//cv::Vec3b int_to_rgb(int color) {
//	int r = color / 65025;
//	int g = (color - r * 65025) / 255;
//	int b = color % 255;
//	return cv::Vec3b(r, g, b);
//}
//
//
//cv::Vec3b rgb_to_hsv(int  r, int  g, int  b) {
//	int maxc = std::max({ r, g, b });
//	int minc = std::min({ r, g, b });
//	if (minc == maxc) return 0.0, 0.0, maxc;
//
//	int s = (maxc - minc) / maxc;
//	int rc = (maxc - r) / (maxc - minc);
//	int gc = (maxc - g) / (maxc - minc);
//	int bc = (maxc - b) / (maxc - minc);
//	int h;
//
//	if (r == maxc) int h = bc - gc;
//	else if (g == maxc) h = 2.0 + rc - bc;
//	else h = 4.0 + gc - rc;
//
//	h = (h / 6.0) / 1.0;  //TODO: 	h = (h / 6.0) % 1.0;だった。
//	return cv::Vec3b(h, s, maxc);
//}
//
////BLACK
//int getFirstNotBlackX(int _x, int _y) {
//	int x = _x;
//	int y = _y;
//
//	cv::Vec3b *ptr = img.ptr<cv::Vec3b>(y * img.cols);
//	cv::Vec3b color = ptr[x];
//	int v = std::max({ color[0], color[1], color[2]});
//
//	while (v < BLACK_VALUE) {
//		x++;
//		if (x >= width) return -1;
//		color = ptr[x];
//		v = std::max({ color[0], color[1], color[2] });
//	}
//	return x;
//}
//
//int getNextBlackX(int _x, int _y) {
//	int x = _x + 1;
//	int y = _y;
//
//	cv::Vec3b *ptr = img.ptr<cv::Vec3b>(y * img.cols);
//	cv::Vec3b color = ptr[x];
//	int v = std::max({ color[0], color[1], color[2] });
//
//	while(v > BLACK_VALUE) {
//		x++;
//		if (x >= width) return width - 1;
//		color = ptr[x];
//		v = std::max({ color[0], color[1], color[2] });
//	}
//
//	return x - 1;
//}


cv::Vec3b getColor(cv::Mat *img, int x, int y) {

	cv::Vec3b *ptr = (*img).ptr<cv::Vec3b>(y);
	cv::Vec3b color = ptr[x];
	return color;
}

int getV(cv::Vec3b color) {
	return std::max({ color[0], color[1], color[2] });
}


//BLACK
int getFirstNotBlackY(int _x, int _y) {
	int x = _x;
	int y = _y;

	cv::Vec3b color = getColor(&img, x, y);
	int v = getV(color);

	if (y < height) {
		while (v < BLACK_VALUE) {
			y++;
			if (y >= height) return -1;

			cv::Vec3b color = getColor(&img, x, y);
			int v = getV(color);
		}
	}
	return y;
}

int getNextBlackY(int _x, int _y) {
	int x = _x;
	int y = _y + 1;

	cv::Vec3b color = getColor(&img, x, y);
	int v = getV(color);

	if (y < height) {

		while (v > BLACK_VALUE) {
			y++;
			if (y >= height) return height - 1;

			cv::Vec3b color = getColor(&img, x, y);
			int v = getV(color);
		}
	}
	return y - 1;
}




void sortColumn() {
	int x = column;
	int y = 0;
	int yend = 0;

	while (yend < height - 1) {
		//switch (mode) {
		//case 0:
		//	y = getFirstNotBlackY(x, y);
		//	yend = getNextBlackY(x, y);
		//	break;
		//case 1:
		//	y = getFirstBrightY(x, y);
		//	yend = getNextDarkY(x, y);
		//	break;
		//case 2:
		//	y = getFirstNotWhiteY(x, y);
		//	yend = getNextWhiteY(x, y);
		//	break;
		//default:
		//	break;
		//}

		//std::cout << "getFirstNotBlackY\n";
		y = getFirstNotBlackY(x, y);

		//std::cout << "getNextBlackY\n";
		yend = getNextBlackY(x, y);

		//std::cout << "main\n";
		if (y < 0) break;

		int sortLength = yend - y;


		std::vector<cv::Vec3b> sorting;

		for (int i = 0; i < sortLength; i++) {
			cv::Vec3b* ptr = img.ptr<cv::Vec3b>(y + i);
			sorting.push_back(ptr[x]);
		}

		//std::cout << "sorting ........ \n";

		std::sort(sorting.begin(), sorting.end(), [](const cv::Vec3b& c1, const cv::Vec3b& c2) {return (c1[0] * 65025 + c1[1] * 255 + c1[2]) < (c2[0] * 65025 + c2[1] * 255 + c2[2]); });

		for (int i = 0; i < sortLength; i++) {
			cv::Vec3b* ptr = img.ptr<cv::Vec3b>(y + i);
			ptr[x] = sorting[i];
		}

		y = yend + 1;
	}
}



void main() {
	img = cv::imread("C:/Users/Owner/Desktop/AUL_project/Effects/sample_images/0.jpg");
	width = img.cols;
	height = img.rows;

	int slider_value = 0;
	int slider_value2 = 0;

	while (1) {
		cv::Vec3b color = getColor(&img, 10, 10);

		std::cout << color[0] << ", " << color[1] << ", " << color[2] << "\n";
		std::cout << "V = " << getV(color) << "\n";

		while (column < width - 1) {
			sortColumn();
			column++;
		}


		cv::imshow("test", img);

		//cv::createTrackbar("slider1", "test", &slider_value, 100, do_nothing);
		//cv::createTrackbar("slider2", "test", &slider_value2, 100, do_nothing);

		int key = cv::waitKey(1000);

		std::cout << slider_value << "\n";

		if (key == ESCAPE_KEY) {
			// esc or enterキーで終了
			break;
		}
	}
	std::cout << "program finished";
}