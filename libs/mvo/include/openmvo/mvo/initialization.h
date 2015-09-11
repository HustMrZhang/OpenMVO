/*************************************************************************
 * �ļ����� initialization
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/9/7
 *
 * ˵���� ��ʼ����ȷ����ʼλ�ã��ο�rpg_svo(https://github.com/uzh-rpg/rpg_svo)
 *************************************************************************/
#ifndef OPENMVO_MVO_INITIALIZATION_H_
#define OPENMVO_MVO_INITIALIZATION_H_

#include <Eigen/Core>
#include <sophus/se3.h>
#include "openmvo/mvo/frame.h"

namespace mvo
{
	using namespace Eigen;
	using namespace Sophus;

	enum InitResult
	{
		FAILURE, // ʧ��
		NO_KEYFRAME, // û�йؼ�֡
		SUCCESS // �ɹ�
	};

	class Initialization {

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		
		Initialization() {};
		~Initialization() {};
		InitResult addFirstFrame(FramePtr frame_ref);
		InitResult addSecondFrame(FramePtr frame_ref);
		void reset();

	protected:
		/// ���ͼ����Fast�ǵ�
		void detectFeatures(
			FramePtr frame,
			std::vector<cv::Point2f>& px_vec,
			std::vector<Vector3d>& f_vec);

		/// ��ѡ���������������(Lucas Kanade)
		void trackKlt(
			FramePtr frame_ref,
			FramePtr frame_cur,
			std::vector<cv::Point2f>& px_ref,
			std::vector<cv::Point2f>& px_cur,
			std::vector<Vector3d>& f_ref,
			std::vector<Vector3d>& f_cur,
			std::vector<double>& disparities);

		/// ���㵥Ӧ����
		void computeHomography(
			const std::vector<Vector3d>& f_ref,
			const std::vector<Vector3d>& f_cur,
			double focal_length,
			double reprojection_threshold,
			std::vector<int>& inliers,
			std::vector<Vector3d>& xyz_in_cur,
			SE3& T_cur_from_ref);

	protected:
		FramePtr frame_ref_;                   //!< �ο�֡
		std::vector<cv::Point2f> px_ref_;      //!< �ڲο�֡�����ڸ��ٵ�������
		std::vector<cv::Point2f> px_cur_;      //!< �ڵ�ǰ֡�и��ٵ�������
		std::vector<Vector3d> f_ref_;          //!< ��Ӧ�ο�ͼ���е������㵥λ��������
		std::vector<Vector3d> f_cur_;          //!< ��Ӧ��ǰͼ���е������㵥λ��������
		std::vector<double> disparities_;      //!< ��һ֡��ڶ�֡��Ӧ�������ٵ�����֮������ز�ֵ
		std::vector<int> inliers_;             //!< ���м��μ��֮����ڵ㣨�絥Ӧ�任��
		std::vector<Vector3d> xyz_in_cur_;     //!< ��ǰ�����3D��
		SE3 T_cur_from_ref_;                   //!< ���㿪ʼ����֡�ı任��ϵ
	};
}

#endif // OPENMVO_MVO_INITIALIZATION_H_