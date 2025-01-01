#include "Effects.h"
#include "Overlay.h"
#include "LayerObj_info.h"

///////////////////////////////////////////////////
//                  MAIN TEST                    //
///////////////////////////////////////////////////

void do_nothing(int size, void*) {}


int main() {
	cv::namedWindow("test", cv::WINDOW_AUTOSIZE | cv::WINDOW_FREERATIO);
	int value1 = 150;
	int value2 = 150;

	int key = 0;

	RenderingObj renderer(900,700);


	layerObj layer1, layer2;

	cv::Mat img = cv::imread("C:/Users/Owner/Desktop/AUL_project/Effects/sample_images/gradation.png", cv::IMREAD_COLOR);
	cv::cvtColor(img, img, CV_RGB2RGBA);

	cv::Mat img2 = cv::imread("C:/Users/Owner/Desktop/AUL_project/Effects/sample_images/touka.png", cv::IMREAD_UNCHANGED);
	//cv::cvtColor(img2, img2, CV_RGB2RGBA);
	Zoom_simple(&img2, 50, 50);

	layer1.setImage(img);
	layer1.overlay_mode = OVERLAY_TYPE_SCREEN;
	layer2.enable_alpha = false;


	layer2.setImage(img2);
	layer2.overlay_mode = OVERLAY_TYPE_OVERLAY;
	layer2.enable_alpha = true;

	while (key != 27) {

		layer2.move_x = value1 - 150;
		layer2.move_y = value2 - 150;

		renderer.RenderAndAddOneLayer(&layer1);
		renderer.RenderAndAddOneLayer(&layer2);

		cv::createTrackbar("slider1", "test", &value1, 300, do_nothing);
		cv::createTrackbar("slider2", "test", &value2, 300, do_nothing);

		cv::imshow("test", renderer.Cur_img);
		renderer.clearImage();
		key = cv::waitKey(1);

	}
	return 0;
}




//int main() {
//	cv::namedWindow("test", cv::WINDOW_AUTOSIZE | cv::WINDOW_FREERATIO);
//	int value1 = 100;
//
//	int key = 0;
//
//	while (key != 27) {
//		layerObj layer1;
//
//		cv::Mat img = cv::imread("C:/Users/Owner/Desktop/AUL_project/Effects/sample_images/2.jpg", cv::IMREAD_UNCHANGED);
//		Binarize(&img, 0);
//		//rotate(&img, 100 - value1, false);
//
//		//std::cout << img.cols << ", " << img.rows << "\n";
//
//		cv::createTrackbar("slider1", "test", &value1, 200, do_nothing);
//
//		cv::imshow("test", img);
//		key = cv::waitKey(1);
//	}
//	return 0;
//}

