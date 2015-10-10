/*************************************************************************
 * �ļ����� matcher
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/8/17
 *
 * ˵���� �ο�rpg_svo(https://github.com/uzh-rpg/rpg_svo)
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
	class Feature;
	namespace patch_score {
		template<int HALF_PATCH_SIZE> class ZMSSD;
	}

	/**	��Ƭƥ�䣬��Ҫͨ��ͶӰƥ���Լ���������
	 */
	class Matcher
	{
		static const int halfpatch_size_ = 4;
		static const int patch_size_ = 8;
		typedef patch_score::ZMSSD<halfpatch_size_> PatchScore;
	public:
		struct Options
		{
			int align_max_iter;         //!< ��˹ţ�ٷ��ĵ������������ڶ��������ж���
			size_t max_epi_search_steps;//!< ���ż��߷����ߵ����Ĳ�����Ŀ
			bool subpix_refinement;     //!< ÿһ�μ�������֮�󣬽���feature align
			Options() :
				align_max_iter(10),
				max_epi_search_steps(1000),
				subpix_refinement(true)
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

		/// ���ż�����������
		bool findEpipolarMatchDirect(
			const Frame& ref_frame,
			const Frame& cur_frame,
			const Feature& ref_ftr,
			const double d_estimate,
			const double d_min,
			const double d_max,
			double& depth);
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

		bool depthFromTriangulation(
			const SE3& T_search_ref,
			const Vector3d& f_ref,
			const Vector3d& f_cur,
			double& depth);

	public:
		Matrix2d A_cur_ref_;          //!< ����任����
		int search_level_;            //!< ���ڼ�������������ͼ�������ڵĽ������ĵȼ�
		Vector2d px_cur_;
	private:
#ifdef _MSC_VER
		__declspec (align(16)) uint8_t patch_[patch_size_*patch_size_];//!< ��Ƭ
		__declspec (align(16)) uint8_t patch_with_border_[(patch_size_ + 2)*(patch_size_ + 2)];//!<��Ƭ���+2
#else
		uint8_t __attribute__((aligned(16))) patch_[patch_size_*patch_size_];
		uint8_t __attribute__((aligned(16))) patch_with_border_[(patch_size_ + 2)*(patch_size_ + 2)];
#endif

		Vector2d epi_dir_;
		double epi_length_;           //!< ���߶εĳ��ȣ���Ҫ���ڼ�������
		
	};
}
#endif // OPENMVO_MVO_MATCHER_H_