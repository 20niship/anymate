
struct layerObj{
	cv::Mat img = cv::Mat::zeros(10, 10, CV_8UC4);;
	cv::VideoCapture cap;
	std::vector<uint8_t> img_alpha;

	bool enable_alpha = false; //true -> �A���t�@���g���č����@false -> ���̂܂܁i�A���t�@���l�����Ȃ��j
	
	//Active���ǂ���
	bool isSolo = false;
	bool isMute = false;
	bool isActive = isSolo || !isMute;

	//�e��ݒ�
	int start = -1;     //�X�^�[�g�ʒu
	int end = -1;       //�ŏI�ʒu
	int layer_num = 0;  //���C���[�ԍ�
	int width = 0;
	int height = 0;
	float anchor_x = 0; //��_��X���W�@�i�f�t�H���g�ł� width / 2�j
	float anchor_y = 0;

	//TODO: x, y��ω�������Ƃ��͂ǂ�����́H
	float move_x = 0; //��_��X���W�̈ړ������i�f�t�H���g�ł�0�j
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
		//TODO: cap��width��height��ݒ�
		move_x = 0; move_y = 0;
	}

};