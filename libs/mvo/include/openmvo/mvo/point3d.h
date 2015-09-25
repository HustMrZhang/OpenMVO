/*************************************************************************
 * �ļ����� point3d
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/9/7
 *
 * ˵���� ������Ӧ��3D��
 *************************************************************************/
#ifndef OPENMVO_MVO_POINT3D_H_
#define OPENMVO_MVO_POINT3D_H_

#include <list>
#include <Eigen/Core>
#include "openmvo/utils/noncopyable.h"

namespace mvo
{
	class Feature;

	using namespace Eigen;
	typedef Matrix<double, 2, 3> Matrix23d;
	/**	ȷ�������Ψһ
	 */
	class Point3D : Noncopyable
	{
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Point3D(const Vector3d& pos);
		~Point3D();
		/// ���������һ��֡��
		void addFrameRef(Feature* ftr);
		/// �õ���������ӽǵĹ۲�����
		bool getCloseViewObs(const Vector3d& pos, Feature*& obs) const;
		/// �Ż����λ��ͨ����С��ͶӰ���
		void optimize(const size_t n_iter);

		/// 3D��ͶӰ����λƽ��(focal length = 1)�Ե�����ſ˱Ⱦ���
		inline static void jacobian_xyz2uv(
			const Vector3d& p_in_f,
			const Matrix3d& R_f_w,
			Matrix23d& point_jac)
		{
			const double z_inv = 1.0 / p_in_f[2];
			const double z_inv_sq = z_inv*z_inv;
			point_jac(0, 0) = z_inv;
			point_jac(0, 1) = 0.0;
			point_jac(0, 2) = -p_in_f[0] * z_inv_sq;
			point_jac(1, 0) = 0.0;
			point_jac(1, 1) = z_inv;
			point_jac(1, 2) = -p_in_f[1] * z_inv_sq;
			point_jac = -point_jac * R_f_w;
		}
	public:
		static int                  point_counter_;           //!< ������ļ�������������Ψһ��id
		int                         id_;                      //!< ��Ψһ��id
		Vector3d                    pos_;                     //!< ������������ϵ�е�λ��
		std::list<Feature*>         obs_;                     //!< ��Ӧ����������
		int                         last_projected_kf_id_;    //!< ��ͶӰ�ı�ʶ������ͬһ������ͶӰ����
		int                         n_failed_reproj_;         //!< ��ͶӰʧ�ܵ��������������۵������
		int                         n_succeeded_reproj_;      //!< ��ͶӰ�ɹ����������������۵������
		int                         last_structure_optim_;    //!< ������Ż���ʱ���
	};

}

#endif // OPENMVO_MVO_POINT3D_H_