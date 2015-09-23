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
	public:
		static int                  point_counter_;           //!< ������ļ�������������Ψһ��id
		int                         id_;                      //!< ��Ψһ��id
		Vector3d                    pos_;                     //!< ������������ϵ�е�λ��
		std::list<Feature*>         obs_;                     //!< ��Ӧ����������
		int                         last_projected_kf_id_;    //!< ��ͶӰ�ı�ʶ������ͬһ������ͶӰ����
		int                         n_failed_reproj_;         //!< ��ͶӰʧ�ܵ��������������۵������
		int                         n_succeeded_reproj_;      //!< ��ͶӰ�ɹ����������������۵������
	};
}

#endif // OPENMVO_MVO_POINT3D_H_