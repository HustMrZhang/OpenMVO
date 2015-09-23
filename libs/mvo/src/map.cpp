/*************************************************************************
 * �ļ����� map
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/8/17
 *
 * ˵���� ��ͼ���壬��Ҫ�洢3d��͹ؼ�֡���ο�rpg_svo(https://github.com/uzh-rpg/rpg_svo)
 *************************************************************************/
#include <openmvo/mvo/map.h>
#include <openmvo/mvo/feature.h>

namespace mvo
{
	Map::Map() {}

	Map::~Map()
	{
		keyframes_.clear();//������йؼ�֡
		std::for_each(points_.begin(), points_.end(), [&](Point3D*& pt){
			delete pt;
			pt = NULL;
		});
		points_.clear();
	}

	/// ������ص���Ұ�Ĺؼ�֡���͸��ݵ�ǰ֡��������Ӧ��3D�㣬�Ƿ���ͶӰ�������ؼ�֡�У�������
	/// ����ؼ�֡�뵱ǰ֡�������Ұ
	void Map::getCloseKeyframes(
		const FramePtr& frame,
		std::list< std::pair<FramePtr, double> >& close_kfs) const
	{
		for (auto kf : keyframes_)
		{
			// ��⵱ǰ֡��ؼ�֮֡���Ƿ����ص�����Ұ��ͨ���ؼ���(����)�����м���
			for (auto keypoint : kf->key_pts_)
			{
				if (keypoint == nullptr)
					continue;

				if (frame->isVisible(keypoint->point->pos_))// �ж�Ŀǰ֡����������Ӧ��3d���Ƿ��ڹؼ�֡�пɼ�
				{
					close_kfs.push_back(std::make_pair(
						kf, (frame->T_f_w_.translation() - kf->T_f_w_.translation()).norm()));
					break; // ����ؼ�֡��Ŀǰ֡���ص�����Ұ�������close_kfs
				}
			}
		}
	}

	void Map::addKeyframe(FramePtr new_keyframe)
	{
		keyframes_.push_back(new_keyframe);
	}

}