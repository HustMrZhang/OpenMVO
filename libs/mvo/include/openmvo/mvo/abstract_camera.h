/*************************************************************************
 * �ļ����� abstract_camera
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/8/1
 *
 * ˵���� �ο�rpg_svo(https://github.com/uzh-rpg/rpg_svo)
 *************************************************************************/
#ifndef OPENMVO_MVO_ABSTRACT_CAMERA_H_
#define OPENMVO_MVO_ABSTRACT_CAMERA_H_

#include "openmvo/mvo/link_pragmas.h"
#include <Eigen/Core>

namespace mvo
{
	using namespace Eigen;

	/**	������������
	 */
	class MVO_IMPEXP AbstractCamera
	{
	public:
		AbstractCamera() {}; // �˹��캯����ȫ�����ģ��ʹ��
		AbstractCamera(int width, int height) : width_(width), height_(height) {};

		virtual ~AbstractCamera() {};

		/// ͼ����������ת���������ϵ�µĵ�
		virtual Vector3d cam2world(const double& x, const double& y) const = 0;

		/// ͼ����������ת���������ϵ�µĵ�
		virtual Vector3d cam2world(const Vector2d& px) const = 0;

		/// ���������ϵ�µĵ�תͼ����������
		virtual Vector2d world2cam(const Vector3d& xyz_c) const = 0;

		/// ͼ��ƽ�����ص���������ת��������
		virtual Vector2d world2cam(const Vector2d& uv) const = 0;

		/// ����x����Ľ���ֵ
		virtual double getFocalLength() const = 0;

		/// ��������ֱ��ʵĿ��
		inline int width() const { return width_; }
		/// ��������ֱ��ʵĸ߶�
		inline int height() const { return height_; }

	protected:
		int width_; //!< ����ֱ��ʵĿ��
		int height_; //!< ����ֱ��ʵĸ߶�
	};
}

#endif // OPENMVO_MVO_ABSTRACT_CAMERA_H_

