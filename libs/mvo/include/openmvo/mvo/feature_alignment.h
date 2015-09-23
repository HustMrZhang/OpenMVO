/*************************************************************************
 * �ļ����� feature_alignment
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/9/18
 *
 * ˵���� 
 *************************************************************************/
#ifndef OPENMVO_MVO_FEATURE_ALIGNMENT_H_
#define OPENMVO_MVO_FEATURE_ALIGNMENT_H_

#include <stdint.h>
#include <Eigen/Core>
#include <opencv2/opencv.hpp>

namespace mvo
{
	namespace feature_alignment 
	{
		using namespace Eigen;

		bool align2D(
			const cv::Mat& cur_img,
			uint8_t* ref_patch_with_border,
			uint8_t* ref_patch,
			const int n_iter,
			Vector2d& cur_px_estimate,
			bool no_simd = false);
	}
}

#endif // OPENMVO_MVO_FEATURE_ALIGNMENT_H_