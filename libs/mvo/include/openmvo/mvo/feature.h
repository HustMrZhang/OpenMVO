/*************************************************************************
 * �ļ����� feature
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/8/2
 *
 * ˵���� �����������壬�ο�rpg_svo(https://github.com/uzh-rpg/rpg_svo)
 *************************************************************************/
#ifndef OPENMVO_MVO_FEATURE_H_
#define OPENMVO_MVO_FEATURE_H_

#include <vector>
#include <Eigen/Core>
#include "openmvo/mvo/frame.h"
#include "openmvo/mvo/point3d.h"

namespace mvo
{
	using namespace Eigen;

	/// ���ڽǵ������ʱ��������������߿�ʼ��ʼ��
	struct Corner
	{
		int x;        //!< ��ͼ���нǵ��x����
		int y;        //!< ��ͼ���нǵ��y����
		int level;    //!< �ǵ����ڽ������ĵȼ�
		float score;  //!< shi-tomasi �ǵ���С����ֵ
		float angle;  //!< �ݶ���������Ӧ�ݶ�ֵ
		Corner(int x, int y, float score, int level, float angle) :
			x(x), y(y), level(level), score(score), angle(angle)
		{}
	};
	typedef std::vector<Corner> Corners;

	/**	�������Ƕ�߶�
	 */
	struct Feature
	{
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		///��������,Ŀǰ��ֻ���ǽǵ㣬������������������ʱ���ٽ������
		enum FeatureType {
			CORNER//�ǵ�
		};
		FeatureType type;     //!< �������ͣ��ǵ�
		Frame* frame;         //!< ָ��ָ����������⵽����Ӧ��֡
		Vector2d px;          //!< �����ڽ������ȼ�Ϊ0ʱ����������
		int level;            //!< ��������ȡʱ��ͼ��������ĵȼ�
		Vector3d f;           //!< �����ĵ�λ��������
		Point3D* point;       //!< ָ��ָ���������Ӧ��3D��
		
		Feature(Frame* _frame, const Vector2d& _px, int _level) :
			type(CORNER),
			frame(_frame),
			px(_px),
			level(_level),
			f(frame->cam_->cam2world(px)),
			point(NULL)
		{}

		Feature(Frame* _frame, Point3D* _point, const Vector2d& _px, const Vector3d& _f, int _level) :
			type(CORNER),
			frame(_frame),
			px(_px),			
			level(_level),
			f(_f),
			point(_point)
		{}

		~Feature(){}
	};
}

#endif // OPENMVO_MVO_FEATURE_H_
