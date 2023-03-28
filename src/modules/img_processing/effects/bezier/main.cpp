# include <opencv2/opencv.hpp>
# include <opencv2/highgui.hpp>
# include <opencv2/imgproc/imgproc_c.h> //CV_AA用

#include <iostream>

#define ESCAPE_KEY 27
#define SQUARE(x) ((x)*(x))

float mX1 = 0.25f;
float mX2 = 0.1f;
float mY1 = 0.25f;
float mY2 = 1.0f;


float Bezier_fA(float aA1, float aA2) { return 1.0f - 3.0f * aA2 + 3.0f * aA1; }
float Bezier_fB(float aA1, float aA2) { return 3.0f * aA2 - 6.0 * aA1; }
float Bezier_fC(float aA1) { return 3.0 * aA1; }

float CalcBezier(float aT, float aA1, float aA2) { return  ((Bezier_fA(aA1, aA2)*aT + Bezier_fB(aA1, aA2))*aT + Bezier_fC(aA1))*aT; }
float GetSlope(float aT, float aA1, float aA2) { return 3.0 * Bezier_fA(aA1, aA2)*aT*aT + 2.0 * Bezier_fB(aA1, aA2) * aT + Bezier_fC(aA1); }

float getForX(float aX) {
	float currentSlope;
	float aGuessT = aX;
	for (__int8 i = 0; i < 4; i++) {
		currentSlope = GetSlope(aGuessT, mX1, mX2);
		if (currentSlope == 0.0) return aGuessT;
		float currentX = CalcBezier(aGuessT, mX1, mX2) - aX;
		aGuessT -= currentX / currentSlope;
	}
	return aGuessT;
}


float getBezier(float aX){
	if (mX1 == mY1 && mX2 == mY2) return aX; // linear
	return CalcBezier(getForX(aX), mY1, mY2);
}


#define WINDOW_SIZE 500.0f
#define WINDOW_MARGIN 25


cv::Mat img = cv::Mat::zeros(WINDOW_SIZE + WINDOW_MARGIN * 2, WINDOW_SIZE + WINDOW_MARGIN * 2, CV_64FC3);
bool isBtnDown = false;

void mouse_callback(int event, int x, int y, int flags, void *userdata){
	if (event == cv::EVENT_LBUTTONDOWN) {
		isBtnDown = true;
		return;
	}else if (event == cv::EVENT_LBUTTONUP) {
		isBtnDown = false;
		return;
	}

	float posX = float(x - WINDOW_MARGIN) / WINDOW_SIZE;
	float posY = 1.0f - float(y - WINDOW_MARGIN) / WINDOW_SIZE;

	float dis_1 = SQUARE(posX - mX1) + SQUARE(posY - mY1);
	float dis_2 = SQUARE(posX - (1.0f - mX2)) + SQUARE(posY - mY2);

	if (event == cv::EVENT_MOUSEMOVE && isBtnDown == true) {
		if (dis_1 < dis_2 && dis_1 < 10) {
			mX1 = posX; 
			mY1 = posY;
		}
		if (dis_1 > dis_2 && dis_2 < 10) {
			mX2 = 1.0f - posX;
			mY2 = posY;
		}

		return;
	}

}


void main() {
	cv::namedWindow("test", cv::WINDOW_FREERATIO);
	cv::setMouseCallback("test", mouse_callback);

	while (1) {
		cv::rectangle(img, cv::Point(0, 0), cv::Point(WINDOW_SIZE + WINDOW_MARGIN*2, WINDOW_SIZE + WINDOW_MARGIN*2), cv::Scalar(0,0,0), -1, 8, 0);
		cv::rectangle(img, cv::Point(WINDOW_MARGIN, WINDOW_MARGIN), cv::Point(WINDOW_SIZE + WINDOW_MARGIN, WINDOW_SIZE + WINDOW_MARGIN), cv::Scalar(225, 225, 225), 1, 8, 0);

		cv::Point point_1 = cv::Point(WINDOW_SIZE * mX1 + WINDOW_MARGIN, WINDOW_SIZE * (1.0f - mY1) + WINDOW_MARGIN);
		cv::Point point_2 = cv::Point(WINDOW_SIZE * (1.0f - mX2) + WINDOW_MARGIN, WINDOW_SIZE * (1.0f - mY2) + WINDOW_MARGIN);

		// ベジエ曲線の描画
		int posX_old = 0;
		int posY_old = 500.0f * (1.0f - getBezier(0));

		for (int i = 0; i < 100; i++) {
			int posX = WINDOW_SIZE * float(i) / 100.0f;
			int posY = 500.0f * (1.0f - getBezier(float(i) / 100.0f));
			cv::line(img, cv::Point(posX_old + WINDOW_MARGIN, posY_old + WINDOW_MARGIN), cv::Point(posX + WINDOW_MARGIN, posY + WINDOW_MARGIN), cv::Scalar(225, 225, 225), 2, CV_AA);
			posX_old = posX; posY_old = posY;
		}


		// Blue，太さ10，アンチエイリアス
		cv::line(img, cv::Point(WINDOW_MARGIN,  WINDOW_SIZE + WINDOW_MARGIN), point_1, cv::Scalar(200, 150, 0), 2, CV_AA);
		cv::line(img, cv::Point(WINDOW_SIZE + WINDOW_MARGIN,  WINDOW_MARGIN), point_2, cv::Scalar(200, 150, 0), 2, CV_AA);

		// 画像，円の中心座標，半径，色，線太さ，種類
		cv::circle(img, point_1, 7, cv::Scalar(0, 200, 200), 3, 4);
		cv::circle(img, point_2, 7, cv::Scalar(0, 200, 200), 3, 4);

		std::string text = "(" + std::to_string(mX1) + ", " + std::to_string(mY1) + "), (" + std::to_string(mX2) + ", " + std::to_string(mY2) + ")";
		cv::putText(img, text, cv::Point(0, 100), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(100,100,100));
		cv::imshow("test", img);


		int key = cv::waitKey(1);

		std::cout  << "(" << mX1 << ", " << mY1 << "), (" << mX2 << ", " << mY2 << ")\n";

		if (key == ESCAPE_KEY) {
			// esc or enterキーで終了
			break;
		}
	}
	std::cout << "program finished";
}

