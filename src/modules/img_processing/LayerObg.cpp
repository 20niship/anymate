#include "LayerObj_info.h"
#define SET_VALUE_RANGE(value, min, max) MAX(min, MIN(value, max))

void RenderingObj::setRenderSize(int width, int height) {
	const_width = width;
	const_height = height;

	const_center_x = float(const_width) / 2.0f;
	const_center_y = float(const_height) / 2.0f;
}


void RenderingObj::setPlayStartPos(int pos) {
	// 

}


void RenderingObj::getFrame(int) {


}


/////////////////////////////////////////////////////////////////////////
//        reference : https://en.wikipedia.org/wiki/Blend_modes
/////////////////////////////////////////////////////////////////////////
#define IMAGE_BLEND_PROCESSOR_ADD(a, b) (MIN((a)+(b), 255))

// f(a, b) = 255 - (255 - a)*(255-b)  , where a is the base layer value and b is the top layer value. 
#define IMAGE_BLEND_PROCESSOR_SCREEN(a, b) ((65025 - (255 - (a))* (255 - (b))) / 255)
//#define IMAGE_BLEND_PROCESSOR_SCREEN(a, b) __int8(255.0f * (1.0f - (1.0f - float(a)/255.0f)* (1.0f - float(b)/255.0f))) <- Wikipediaにある正式なやつ 

#define IMAGE_BLEND_PROCESSOR_MULTIPLY(a, b) ((a)* (b) / 255)

#define IMAGE_BLEND_PROCESOR_DARKEN_ONLY(a, b) (MIN((a), (b)))
#define IMAGE_BLEND_PROCESOR_LIGHTEN_ONLY(a, b) (MAX((a), (b)))

//f(a, b) = {2ab + a^2 (1 - 2b) (when b < 0.5) ,  2a(1-b) + sqrt(a)(2b-1) (otherwise)}
#define IMAGE_BLEND_PROCESOR_SOFT_LIGHT(a, b) (((b)<122) ? (0.00888f * float(a)* float(b) + (a)*(a) *(1-2*(b))):(2*(a)*(1-(b)) + sqrt(a) * (2*(b) - 1))

//f(a, b) = {2ab (when a < 0.5) , 1-2(1-a)(1-b) (otherwise)}
#define IMAGE_BLEND_PROCESSOR_OVERLAY(a, b) (((a)<122) ? __int8(0.00888f * float(a)* float(b)) :  ((65025 - 2 * (255 - (a))* (255 - (b))) / 255))


//新しくプロジェクトを作るときに呼び出される
RenderingObj::RenderingObj(int _width, int _height) {
	const_width = _width;
	const_height = _height;

	Cur_img = cv::Mat::zeros(_height, _width, CV_8UC4); //500×500ピクセルの黒色のMat


	const_center_x = float(_width) / 2.0f;
	const_center_y = float(_height) / 2.0f;
}


RenderingObj::~RenderingObj() {
	//TODO: 全てのメモリーをリリースする

}

void RenderingObj::_overlay_normal(layerObj *upper_layer) {
	int x_min = const_center_x + (*upper_layer).move_x - (*upper_layer).anchor_x;
	int x_max = const_center_x + (*upper_layer).move_x - (*upper_layer).anchor_x + (*upper_layer).width;
	if ((const_width < x_min) || (x_max < 0))return;


	int y_min = const_center_y + (*upper_layer).move_y - (*upper_layer).anchor_y;
	int y_max = const_center_y + (*upper_layer).move_y - (*upper_layer).anchor_y + (*upper_layer).height;

	if ((const_height < y_min) || (y_max < 0)) return;

	int x_min2 = SET_VALUE_RANGE(x_min, 0, const_width);
	int x_max2 = SET_VALUE_RANGE(x_max, x_min, const_width);

	int y_min2 = SET_VALUE_RANGE(y_min, 0, const_height);
	int y_max2 = SET_VALUE_RANGE(y_max, y_min, const_height);

	if ((*upper_layer).enable_alpha) {
		for (int dy = y_min2; dy < y_max2; dy++) {
			cv::Vec4b* ptr_rendered_img = Cur_img.ptr<cv::Vec4b>(dy);
			cv::Vec4b* ptr_layer_img = (*upper_layer).img.ptr<cv::Vec4b>(dy - y_min);

			for (int dx = x_min2; dx < x_max2; dx++) {
				//float alpha = float((*upper_layer).img_alpha[dx - x_min, dy - y_min]) / 225;
				float alpha = float(ptr_layer_img[dx - x_min][3]) / 255.0f;
				//std::cout << "alpha" << alpha << "\n";
				ptr_rendered_img[dx] = ptr_rendered_img[dx] * (1.0f - alpha) + ptr_layer_img[dx - x_min] * alpha;
				ptr_rendered_img[dx][3] = MAX(int(alpha * 225), (ptr_rendered_img[dx][3]));
			}
		}
	}
	else {
		for (int dy = y_min2; dy < y_max2; dy++) {
			cv::Vec4b* ptr_rendered_img = Cur_img.ptr<cv::Vec4b>(dy);
			cv::Vec4b* ptr_layer_img = (*upper_layer).img.ptr<cv::Vec4b>(dy - y_min);
			for (int dx = x_min2; dx < x_max2; dx++) {
				ptr_rendered_img[dx] = ptr_layer_img[dx - x_min];
			}
		}
	}
}


void RenderingObj::_overlay_add(layerObj *upper_layer) {
	int x_min = const_center_x + (*upper_layer).move_x - (*upper_layer).anchor_x;
	int x_max = const_center_x + (*upper_layer).move_x - (*upper_layer).anchor_x + (*upper_layer).width;
	if ((const_width < x_min) || (x_max < 0))return;


	int y_min = const_center_y + (*upper_layer).move_y - (*upper_layer).anchor_y;
	int y_max = const_center_y + (*upper_layer).move_y - (*upper_layer).anchor_y + (*upper_layer).height;

	if ((const_height < y_min) || (y_max < 0)) return;

	int x_min2 = SET_VALUE_RANGE(x_min, 0, const_width);
	int x_max2 = SET_VALUE_RANGE(x_max, x_min, const_width);

	int y_min2 = SET_VALUE_RANGE(y_min, 0, const_height);
	int y_max2 = SET_VALUE_RANGE(y_max, y_min, const_height);

	if ((*upper_layer).enable_alpha) {
		for (int dy = y_min2; dy < y_max2; dy++) {
			cv::Vec4b* ptr_rendered_img = Cur_img.ptr<cv::Vec4b>(dy);
			cv::Vec4b* ptr_layer_img = (*upper_layer).img.ptr<cv::Vec4b>(dy - y_min);

			for (int dx = x_min2; dx < x_max2; dx++) {
				//float alpha = float((*upper_layer).img_alpha[dx - x_min, dy - y_min]) / 225;
				float alpha = float(ptr_layer_img[dx - x_min][3]) / 255.0f;
				//std::cout << "alpha" << alpha << "\n";
				for (int i = 0; i < 3; i++){
					ptr_rendered_img[dx][i] = IMAGE_BLEND_PROCESSOR_ADD(ptr_rendered_img[dx][i], ptr_layer_img[dx - x_min][i] * alpha);
				}
				ptr_rendered_img[dx][3] = MAX(int(alpha * 225), (ptr_rendered_img[dx][3]));
			}
		}
	}
	else {
		for (int dy = y_min2; dy < y_max2; dy++) {
			cv::Vec4b* ptr_rendered_img = Cur_img.ptr<cv::Vec4b>(dy);
			cv::Vec4b* ptr_layer_img = (*upper_layer).img.ptr<cv::Vec4b>(dy - y_min);
			for (int dx = x_min2; dx < x_max2; dx++) {
				for (int i = 0; i < 3; i++) {
					ptr_rendered_img[dx][i] = IMAGE_BLEND_PROCESSOR_ADD(ptr_rendered_img[dx][i], ptr_layer_img[dx - x_min][i]);
				}
			}
		}
	}
}


void RenderingObj::_overlay_multiply(layerObj *upper_layer){
}

void RenderingObj::_overlay_screen(layerObj *upper_layer){
	int x_min = const_center_x + (*upper_layer).move_x - (*upper_layer).anchor_x;
	int x_max = const_center_x + (*upper_layer).move_x - (*upper_layer).anchor_x + (*upper_layer).width;
	if ((const_width < x_min) || (x_max < 0))return;


	int y_min = const_center_y + (*upper_layer).move_y - (*upper_layer).anchor_y;
	int y_max = const_center_y + (*upper_layer).move_y - (*upper_layer).anchor_y + (*upper_layer).height;

	if ((const_height < y_min) || (y_max < 0)) return;

	int x_min2 = SET_VALUE_RANGE(x_min, 0, const_width);
	int x_max2 = SET_VALUE_RANGE(x_max, x_min, const_width);

	int y_min2 = SET_VALUE_RANGE(y_min, 0, const_height);
	int y_max2 = SET_VALUE_RANGE(y_max, y_min, const_height);

	if ((*upper_layer).enable_alpha) {
		for (int dy = y_min2; dy < y_max2; dy++) {
			cv::Vec4b* ptr_rendered_img = Cur_img.ptr<cv::Vec4b>(dy);
			cv::Vec4b* ptr_layer_img = (*upper_layer).img.ptr<cv::Vec4b>(dy - y_min);

			for (int dx = x_min2; dx < x_max2; dx++) {
				//float alpha = float((*upper_layer).img_alpha[dx - x_min, dy - y_min]) / 225;
				float alpha = float(ptr_layer_img[dx - x_min][3]) / 255.0f;
				//std::cout << "alpha" << alpha << "\n";
				for (int i = 0; i < 3; i++) {
					ptr_rendered_img[dx][i] = IMAGE_BLEND_PROCESSOR_SCREEN(ptr_rendered_img[dx][i], ptr_layer_img[dx - x_min][i] * alpha);
						255 - (255 - ptr_rendered_img[dx][i])*(255 - ptr_layer_img[dx - x_min][i] * alpha);
				}
				ptr_rendered_img[dx][3] = MAX(int(alpha * 225), (ptr_rendered_img[dx][3]));
			}
		}
	}else {
		for (int dy = y_min2; dy < y_max2; dy++) {
			cv::Vec4b* ptr_rendered_img = Cur_img.ptr<cv::Vec4b>(dy);
			cv::Vec4b* ptr_layer_img = (*upper_layer).img.ptr<cv::Vec4b>(dy - y_min);
			for (int dx = x_min2; dx < x_max2; dx++) {
				for (int i = 0; i < 3; i++) {
					ptr_rendered_img[dx][i] = IMAGE_BLEND_PROCESSOR_SCREEN(ptr_rendered_img[dx][i], ptr_layer_img[dx - x_min][i]);
				}
			}
		}
	}
}

void RenderingObj::_overlay_overlay(layerObj *upper_layer){
	int x_min = const_center_x + (*upper_layer).move_x - (*upper_layer).anchor_x;
	int x_max = const_center_x + (*upper_layer).move_x - (*upper_layer).anchor_x + (*upper_layer).width;
	if ((const_width < x_min) || (x_max < 0))return;


	int y_min = const_center_y + (*upper_layer).move_y - (*upper_layer).anchor_y;
	int y_max = const_center_y + (*upper_layer).move_y - (*upper_layer).anchor_y + (*upper_layer).height;

	if ((const_height < y_min) || (y_max < 0)) return;

	int x_min2 = SET_VALUE_RANGE(x_min, 0, const_width);
	int x_max2 = SET_VALUE_RANGE(x_max, x_min, const_width);

	int y_min2 = SET_VALUE_RANGE(y_min, 0, const_height);
	int y_max2 = SET_VALUE_RANGE(y_max, y_min, const_height);

	if ((*upper_layer).enable_alpha) {
		for (int dy = y_min2; dy < y_max2; dy++) {
			cv::Vec4b* ptr_rendered_img = Cur_img.ptr<cv::Vec4b>(dy);
			cv::Vec4b* ptr_layer_img = (*upper_layer).img.ptr<cv::Vec4b>(dy - y_min);

			for (int dx = x_min2; dx < x_max2; dx++) {
				//float alpha = float((*upper_layer).img_alpha[dx - x_min, dy - y_min]) / 225;
				float alpha = float(ptr_layer_img[dx - x_min][3]) / 255.0f;
				//std::cout << "alpha" << alpha << "\n";
				for (int i = 0; i < 3; i++) {
					ptr_rendered_img[dx][i] = IMAGE_BLEND_PROCESSOR_OVERLAY(ptr_rendered_img[dx][i], ptr_layer_img[dx - x_min][i] * alpha);
					255 - (255 - ptr_rendered_img[dx][i])*(255 - ptr_layer_img[dx - x_min][i] * alpha);
				}
				ptr_rendered_img[dx][3] = MAX(int(alpha * 225), (ptr_rendered_img[dx][3]));
			}
		}
	}
	else {
		for (int dy = y_min2; dy < y_max2; dy++) {
			cv::Vec4b* ptr_rendered_img = Cur_img.ptr<cv::Vec4b>(dy);
			cv::Vec4b* ptr_layer_img = (*upper_layer).img.ptr<cv::Vec4b>(dy - y_min);
			for (int dx = x_min2; dx < x_max2; dx++) {
				for (int i = 0; i < 3; i++) {
					ptr_rendered_img[dx][i] = IMAGE_BLEND_PROCESSOR_OVERLAY(ptr_rendered_img[dx][i], ptr_layer_img[dx - x_min][i]);
				}
			}
		}
	}
}

void RenderingObj::_overlay_compare_1(layerObj *upper_layer){}
void RenderingObj::_overlay_compare_2(layerObj *upper_layer){}


void RenderingObj::RenderAndAddOneLayer(layerObj *upper_layer) {

	switch ((*upper_layer).overlay_mode)
	{
	case OVERLAY_TYPE_NORMAL:
		_overlay_normal(upper_layer);
		break;

	case OVERLAY_TYPE_SCREEN:
		_overlay_screen(upper_layer);
		break;

	case OVERLAY_TYPE_ADD:
		_overlay_add(upper_layer);
		break;

	case OVERLAY_TYPE_OVERLAY:
		_overlay_overlay(upper_layer);
		break;
	default:
		break;
	}
}

void RenderingObj::clearImage() {
	Cur_img = cv::Mat::zeros(const_height, const_width, CV_8UC4); //500×500ピクセルの黒色のMat
}
