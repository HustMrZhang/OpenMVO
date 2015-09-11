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

	public:
		static int                  point_counter_;           //!< ������ļ�������������Ψһ��id
		int                         id_;                      //!< ��Ψһ��id
		Vector3d                    pos_;                     //!< ������������ϵ�е�λ��
		std::list<Feature*>         obs_;                     //!< ��Ӧ����������
	};
}

#endif // OPENMVO_MVO_POINT3D_H_