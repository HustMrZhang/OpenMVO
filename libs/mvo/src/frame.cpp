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
#include "openmvo/mvo/feature.h"
#include <openmvo/utils/math_utils.h>

namespace mvo
{
	int Frame::frame_counter_ = 0;

	Frame::Frame(AbstractCamera* cam, const cv::Mat& img, double timestamp) :
		id_(frame_counter_++),
		timestamp_(timestamp),
		cam_(cam),
		key_pts_(5),
		is_keyframe_(false)
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
		// ���ùؼ�֡��Ӧ��5���ؼ��������ʼ��Ϊ NULL
		std::for_each(key_pts_.begin(), key_pts_.end(), [&](Feature* ftr){ ftr = NULL; });
		// ����ͼ���������Ĭ�Ͻ������ĵȼ�Ϊ5
		createImgPyramid(img, 5, img_pyr_);
	}

	void Frame::setKeyframe()
	{
		// ��ʶ��һ֡Ϊ�ؼ�֡
		is_keyframe_ = true;
		setKeyPoints();
	}

	/// ��֡���������
	void Frame::addFeature(Feature* ftr)
	{
		fts_.push_back(ftr);
	}

	void Frame::setKeyPoints()
	{
		// �������ָ���3d��Ϊ�գ������ø�����ΪNULL
		for (size_t i = 0; i < 5; ++i)
			if (key_pts_[i] != NULL)
				if (key_pts_[i]->point == NULL)
					key_pts_[i] = NULL;
		// �ҵ�5������
		std::for_each(fts_.begin(), fts_.end(), [&](Feature* ftr){ if (ftr->point != NULL) checkKeyPoints(ftr); });
	}

	void Frame::checkKeyPoints(Feature* ftr)
	{
		// ���ȵõ�������ĵ�
		const int cu = cam_->width() / 2;
		const int cv = cam_->height() / 2;

		// �����һ������Ϊ�գ������ĵ�һ������תΪ�ؼ������������Ϊ�գ����ж��½���������Ƿ��֮ǰ����
		// ���ӽ����ģ��ǣ����滻������
		if (key_pts_[0] == NULL)
			key_pts_[0] = ftr;
		else if (std::max(std::fabs(ftr->px[0] - cu), std::fabs(ftr->px[1] - cv))
			< std::max(std::fabs(key_pts_[0]->px[0] - cu), std::fabs(key_pts_[0]->px[1] - cv)))
			key_pts_[0] = ftr;
		// �ҵ��м������֮�󣬽�ͼƬ�ֳ�4�飬��ÿ�����ҳ�1��������������ԽԶ������
		if (ftr->px[0] >= cu && ftr->px[1] >= cv)
		{
			if (key_pts_[1] == NULL)
				key_pts_[1] = ftr;
			else if ((ftr->px[0] - cu) * (ftr->px[1] - cv)
			> (key_pts_[1]->px[0] - cu) * (key_pts_[1]->px[1] - cv))
			key_pts_[1] = ftr;
		}
		if (ftr->px[0] >= cu && ftr->px[1] < cv)
		{
			if (key_pts_[2] == NULL)
				key_pts_[2] = ftr;
			else if ((ftr->px[0] - cu) * (ftr->px[1] - cv)
		> (key_pts_[2]->px[0] - cu) * (key_pts_[2]->px[1] - cv))
		key_pts_[2] = ftr;
		}
		if (ftr->px[0] < cu && ftr->px[1] < cv)
		{
			if (key_pts_[3] == NULL)
				key_pts_[3] = ftr;
			else if ((ftr->px[0] - cu) * (ftr->px[1] - cv)
		> (key_pts_[3]->px[0] - cu) * (key_pts_[3]->px[1] - cv))
		key_pts_[3] = ftr;
		}
		if (ftr->px[0] < cu && ftr->px[1] >= cv)
		{
			if (key_pts_[4] == NULL)
				key_pts_[4] = ftr;
			else if ((ftr->px[0] - cu) * (ftr->px[1] - cv)
		> (key_pts_[4]->px[0] - cu) * (key_pts_[4]->px[1] - cv))
		key_pts_[4] = ftr;
		}
	}

	void Frame::removeKeyPoint(Feature* ftr)
	{
		bool found = false;
		std::for_each(key_pts_.begin(), key_pts_.end(), [&](Feature*& i){
			if (i == ftr) {
				i = NULL;
				found = true;
			}
		});
		if (found)
			setKeyPoints();
	}

	/// �������������ϵ�ĵ��Ƿ���ͼ���пɼ�
	bool Frame::isVisible(const Vector3d& xyz_w) const
	{
		Vector3d xyz_f = T_f_w_*xyz_w;
		if (xyz_f.z() < 0.0)
			return false; // ��������ı���
		Vector2d px = f2c(xyz_f);
		if (px[0] >= 0.0 && px[1] >= 0.0 && px[0] < cam_->width() && px[1] < cam_->height())
			return true;
		return false;
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
	/// ֡�����ʵ�ú���
	namespace frame_utils {

		// ����һ֡���������ж�Ӧ��3D�㣬���������������ȣ�������С��ȣ���ƽ�����
		bool getSceneDepth(const Frame& frame, double& depth_mean, double& depth_min)
		{
			std::vector<double> depth_vec;
			depth_vec.reserve(frame.fts_.size());
			depth_min = std::numeric_limits<double>::max();
			for (auto it = frame.fts_.begin(), ite = frame.fts_.end(); it != ite; ++it)
			{
				if ((*it)->point != NULL)
				{
					// ��������������
					const double z = frame.w2f((*it)->point->pos_).z();
					depth_vec.push_back(z);
					depth_min = fmin(z, depth_min);
				}
			}
			if (depth_vec.empty())
			{
				return false;
			}
			depth_mean = getMedian(depth_vec);
			return true;
		}

	} // namespace frame_utils
	
}