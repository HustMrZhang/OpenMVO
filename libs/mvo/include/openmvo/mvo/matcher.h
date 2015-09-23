/*************************************************************************
 * �ļ����� matcher
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/9/17
 *
 * ˵���� 
 *************************************************************************/
#ifndef OPENMVO_MVO_MATCHER_H_
#define OPENMVO_MVO_MATCHER_H_

#include <stdint.h>
#include <Eigen/Core>
#include <sophus/se3.h>
#include <opencv2/opencv.hpp>

namespace mvo{

	using namespace Eigen;
	using namespace Sophus;

	class Point3D;
	class Frame;
	class AbstractCamera;

	/**	��Ƭƥ�䣬��Ҫͨ��ͶӰƥ���Լ���������
	 */
	class Matcher
	{
		static const int halfpatch_size_ = 4;
		static const int patch_size_ = 8;
	public:
		struct Options
		{
			int align_max_iter;         //!< ��˹ţ�ٷ��ĵ������������ڶ��������ж���
			Options() :
				align_max_iter(10)
			{}
		} options_;

		//��������Ĭ�Ϲ��캯��
		Matcher() = default;
		~Matcher() = default;

		/// Ӧ��������ֱ�ӽ���ƥ��
		/// ���Ҫע��!����������赱ǰ������px_cur�������ƵĽ��ֻ���2-3����
		bool findMatchDirect(
			const Point3D& pt,
			const Frame& frame,
			Vector2d& px_cur);
	private:
		void getWarpMatrixAffine(
			const AbstractCamera& cam_ref,
			const AbstractCamera& cam_cur,
			const Vector2d& px_ref,
			const Vector3d& f_ref,
			const double depth_ref,
			const SE3& T_cur_ref,
			const int level_ref,
			Matrix2d& A_cur_ref);

		int getBestSearchLevel(const Matrix2d& A_cur_ref, const int max_level);

		void warpAffine(
			const Matrix2d& A_cur_ref,
			const cv::Mat& img_ref,
			const Vector2d& px_ref,
			const int level_ref,
			const int level_cur,
			const int halfpatch_size,
			uint8_t* patch);
		/// �Ӵ��߿����Ƭ������Ƭ
		void createPatchFromPatchWithBorder();

	public:
		Matrix2d A_cur_ref_;          //!< ����任����
		int search_level_;            //!< ���ڼ�������������ͼ�������ڵĽ������ĵȼ�

	private:
#ifdef _MSC_VER
		__declspec (align(16)) uint8_t patch_[patch_size_*patch_size_];//!< ��Ƭ
		__declspec (align(16)) uint8_t patch_with_border_[(patch_size_ + 2)*(patch_size_ + 2)];//!<��Ƭ���+2
#else
		uint8_t __attribute__((aligned(16))) patch_[patch_size_*patch_size_];
		uint8_t __attribute__((aligned(16))) patch_with_border_[(patch_size_ + 2)*(patch_size_ + 2)];
#endif
	};
}
#endif // OPENMVO_MVO_MATCHER_H_