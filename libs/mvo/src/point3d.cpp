/*************************************************************************
 * �ļ����� point3d
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/8/7
 *
 * ˵���� ������Ӧ��3D��
 *************************************************************************/
#include "openmvo/mvo/point3d.h"
#include "openmvo/mvo/feature.h"
#include <openmvo/utils/math_utils.h>

namespace mvo
{
	const double EPS = 0.0000000001;
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

	void Point3D::optimize(const size_t n_iter)
	{
		Vector3d old_point = pos_;
		double chi2 = 0.0;
		Matrix3d A;
		Vector3d b;

		for (size_t i = 0; i < n_iter; i++)
		{
			A.setZero();
			b.setZero();
			double new_chi2 = 0.0;

			// ����в�
			for (auto it = obs_.begin(); it != obs_.end(); ++it)
			{
				Matrix23d J;
				const Vector3d p_in_f((*it)->frame->T_f_w_ * pos_);
				Point3D::jacobian_xyz2uv(p_in_f, (*it)->frame->T_f_w_.rotation_matrix(), J);
				const Vector2d e(project2d((*it)->f) - project2d(p_in_f));
				new_chi2 += e.squaredNorm();
				A.noalias() += J.transpose() * J;
				b.noalias() -= J.transpose() * e;
			}

			// �������ϵͳ
			const Vector3d dp(A.ldlt().solve(b));

			// ��������û������
			if ((i > 0 && new_chi2 > chi2) || (bool)std::isnan((double)dp[0]))
			{
				pos_ = old_point; // �ع�
				break;
			}

			// ����ģ��
			Vector3d new_point = pos_ + dp;
			old_point = pos_;
			pos_ = new_point;
			chi2 = new_chi2;

			// ������ֹͣ
			if (norm_max(dp) <= EPS)
				break;
		}

	}
}