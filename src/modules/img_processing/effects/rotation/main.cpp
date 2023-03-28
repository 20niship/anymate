# include <opencv2/opencv.hpp>
# include <opencv2/highgui.hpp>
#include "opencv2/opencv.hpp"


cv::Mat rotate(cv::Mat *img_before, float angle = 0.0) {

	// get rotation matrix for rotating the image around its center in pixel coordinates
	cv::Point2f center(((*img_before).cols - 1) / 2.0, ((*img_before).rows - 1) / 2.0);
	cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);

	// determine bounding rectangle, center not relevant
	cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), (*img_before).size(), angle).boundingRect2f();

	// adjust transformation matrix
	rot.at<double>(0, 2) += bbox.width / 2.0 - (*img_before).cols / 2.0;
	rot.at<double>(1, 2) += bbox.height / 2.0 - (*img_before).rows / 2.0;

	cv::Mat dst;
	cv::warpAffine((*img_before), dst, rot, bbox.size());
	return dst;
}


int main() {
	cv::namedWindow("test", cv::WINDOW_NORMAL);

	cv::Mat img = cv::imread("C:/Users/Owner/Desktop/AUL_project/Effects/sample_images/0.jpg");
	cv::Mat rotated = rotate(&img, 90);
	std::cout << rotated.cols << rotated.rows << "\n";
	cv::imshow("test", rotated);
	cv::waitKey(0);
	return 0;
}

