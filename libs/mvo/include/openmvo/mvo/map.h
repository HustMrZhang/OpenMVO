/*************************************************************************
 * �ļ����� map
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/8/16
 *
 * ˵���� ��ͼ���壬��Ҫ�洢3d��͹ؼ�֡���ο�rpg_svo(https://github.com/uzh-rpg/rpg_svo)
 *************************************************************************/
#ifndef OPENMVO_MVO_MAP_H_
#define OPENMVO_MVO_MAP_H_

#include <list>
#include <openmvo/utils/noncopyable.h>
#include <openmvo/mvo/frame.h>

namespace mvo
{
	class Point3D;

	class Map : public Noncopyable
	{
	public:
		Map();
		~Map();

		/// �õ���Ŀǰ֡���ص���Ұ�����йؼ�֡
		void getCloseKeyframes(const FramePtr& frame, std::list< std::pair<FramePtr, double> >& close_kfs) const;
		/// ����ͼ�����һ���µĹؼ�֡
		void addKeyframe(FramePtr new_keyframe);
	public:
		std::list< FramePtr > keyframes_;          //!< ��ͼ�д洢�����йؼ�֡
		std::list< Point3D* >   points_;         //!< ���3D��
	};
}


#endif // OPENMVO_MVO_MAP_H_