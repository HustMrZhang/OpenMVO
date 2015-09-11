/*************************************************************************
 * �ļ����� frame
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/8/6
 *
 * ˵���� ֡
 *************************************************************************/

#include "openmvo/mvo/frame.h"
#include "openmvo/utils/image_utils.h"

namespace mvo
{
	int Frame::frame_counter_ = 0;

	Frame::Frame(AbstractCamera* cam, const cv::Mat& img, double timestamp) :
		id_(frame_counter_++),
		timestamp_(timestamp),
		cam_(cam)
	{
		initFrame(img);
	}

	Frame::~Frame()
	{
		//std::for_each(fts_.begin(), fts_.end(), [&](Feature* i){if (i != NULL) { delete i; i = NULL; } });
	}

	void Frame::initFrame(const cv::Mat& img)
	{
		// ���ͼ�񣬱�֤ͼ���С�����ģ�ʹ�Сһ�£��Լ�ͼ��Ϊ�Ҷ�ͼ��
		if (img.empty() || img.type() != CV_8UC1 || img.cols != cam_->width() || img.rows != cam_->height())
			throw std::runtime_error("Frame: provided image has not the same size as the camera model or image is not grayscale");

		// ����ͼ���������Ĭ�Ͻ������ĵȼ�Ϊ5
		createImgPyramid(img, 5, img_pyr_);
	}

	/// ��֡���������
	void Frame::addFeature(Feature* ftr)
	{
		fts_.push_back(ftr);
	}

	// ����ͼ�������
	void Frame::createImgPyramid(const cv::Mat& img_level_0, int n_levels, ImgPyr& pyr)
	{
		pyr.resize(n_levels);// ����������ͼ�񣬹�5��
		pyr[0] = img_level_0;
		for (int i = 1; i < n_levels; ++i)
		{
			pyr[i] = cv::Mat(pyr[i - 1].rows / 2, pyr[i - 1].cols / 2, CV_8U);
			halfSample(pyr[i - 1], pyr[i]);
		}
	}

	
}