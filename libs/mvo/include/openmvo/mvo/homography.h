/*************************************************************************
 * �ļ����� homography
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/9/7
 *
 * ˵���� 
 *************************************************************************/
#ifndef OPENMVO_MVO_HOMOGRAPHY_H_
#define OPENMVO_MVO_HOMOGRAPHY_H_

#include <Eigen/Core>
#include <Eigen/StdVector>
#include <Eigen/SVD>
#include <sophus/se3.h>

namespace mvo {

	using namespace Eigen;
	using namespace std;

	/// ��Ӧ����ֽ�
	struct HomographyDecomposition
	{
		Vector3d t;
		Matrix3d R;
		double   d;
		Vector3d n;

		Sophus::SE3 T; //!< �ڶ���ͼ�񵽵�һ��ͼ��������ת��ƽ��
		int score;
	};

	class Homography
	{
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

			/// ��Ӧ�任 ����ͼ���ж�Ӧ�������㣬���༰��ͶӰ���
			Homography(const vector<Vector2d, aligned_allocator<Vector2d> >& fts1,
			const vector<Vector2d, aligned_allocator<Vector2d> >& fts2,
			double focal_length,
			double thresh_in_px);

		void calcFromPlaneParams(const Vector3d & normal,
			const Vector3d & point_on_plane);

		void calcFromMatches();

		size_t computeMatchesInliers();

		bool computeSE3fromMatches();

		bool decompose();

		void findBestDecomposition();


	public:

		double thresh_;//!< ��ͶӰ����ֵ
		double focal_length_;//!< �������㵥Ӧ������ransac����ֵ�����Ϊ����ֵ��
		const std::vector<Vector2d, aligned_allocator<Vector2d> >& fts_c1_; //!< �ڵ�һ��ͼ���ϵ�����
		const std::vector<Vector2d, aligned_allocator<Vector2d> >& fts_c2_; //!< �ڵڶ���ͼ���ϵ�����
		std::vector<bool> inliers_;
		Sophus::SE3 T_c2_from_c1_;             //!< ����ͼ��������ת��ƽ��
		Matrix3d H_c2_from_c1_;                   //!< ��Ӧ����
		//list<HomographyDecomposition> decompositions;
		HomographyDecomposition decompositions_[8];
		size_t decomp_size_;
	};
}

#endif // OPENMVO_MVO_HOMOGRAPHY_H_
