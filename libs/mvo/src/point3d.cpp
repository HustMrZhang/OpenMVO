/*************************************************************************
 * �ļ����� point3d
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/9/7
 *
 * ˵���� ������Ӧ��3D��
 *************************************************************************/
#include "openmvo/mvo/point3d.h"
#include "openmvo/mvo/feature.h"

namespace mvo
{
	int Point3D::point_counter_ = 0;

	/// ���캯�������������������
	Point3D::Point3D(const Vector3d& pos) :
		id_(point_counter_++),
		pos_(pos)
	{}

	Point3D::~Point3D()
	{}

	void Point3D::addFrameRef(Feature* ftr)
	{
		obs_.push_front(ftr);
	}

	bool Point3D::getCloseViewObs(const Vector3d& framepos, Feature*& ftr) const
	{
		// TODO: ����Ҫȷ��������ͬ����ͼ����ͬ�Ľ�������
		// �õ��۲�ķ�������
		Vector3d obs_dir(framepos - pos_); 
		obs_dir.normalize();
		auto min_it = obs_.begin();
		double min_cos_angle = 0;
		for (auto it = obs_.begin(), ite = obs_.end(); it != ite; ++it)
		{
			Vector3d dir((*it)->frame->pos() - pos_); 
			dir.normalize();
			double cos_angle = obs_dir.dot(dir);// ��λ������˵õ�cos�Ƕ�
			if (cos_angle > min_cos_angle)//��֤�����Ǿ���Ͻ�������֡
			{
				min_cos_angle = cos_angle;
				min_it = it;
			}
		}
		ftr = *min_it;
		if (min_cos_angle < 0.5) // ����۲�нǴ���60��û����
			return false;
		return true;
	}
}