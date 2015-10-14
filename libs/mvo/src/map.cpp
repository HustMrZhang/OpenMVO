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
	MapPointCandidates::MapPointCandidates()
	{}

	MapPointCandidates::~MapPointCandidates()
	{
		reset();
	}

	void MapPointCandidates::newCandidatePoint(Point3D* point, double depth_sigma2)
	{
		point->type_ = Point3D::TYPE_CANDIDATE;
		std::unique_lock<std::mutex> lock(mut_);
		candidates_.push_back(PointCandidate(point, point->obs_.front()));
	}

	void MapPointCandidates::addCandidatePointToFrame(FramePtr frame)
	{
		std::unique_lock<std::mutex> lock(mut_);
		PointCandidateList::iterator it = candidates_.begin();
		while (it != candidates_.end())
		{
			if (it->first->obs_.front()->frame == frame.get())
			{
				// ������������ǰ֡��
				it->first->type_ = Point3D::TYPE_UNKNOWN;
				it->first->n_failed_reproj_ = 0;
				it->second->frame->addFeature(it->second);
				it = candidates_.erase(it);
			}
			else
				++it;
		}
	}

	bool MapPointCandidates::deleteCandidatePoint(Point3D* point)
	{
		std::unique_lock<std::mutex> lock(mut_);
		for (auto it = candidates_.begin(), ite = candidates_.end(); it != ite; ++it)
		{
			if (it->first == point)
			{
				deleteCandidate(*it);
				candidates_.erase(it);
				return true;
			}
		}
		return false;
	}

	void MapPointCandidates::removeFrameCandidates(FramePtr frame)
	{
		std::unique_lock<std::mutex> lock(mut_);
		auto it = candidates_.begin();
		while (it != candidates_.end())
		{
			if (it->second->frame == frame.get())
			{
				deleteCandidate(*it);
				it = candidates_.erase(it);
			}
			else
				++it;
		}
	}

	void MapPointCandidates::reset()
	{
		std::unique_lock<std::mutex> lock(mut_);
		std::for_each(candidates_.begin(), candidates_.end(), [&](PointCandidate& c){
			delete c.first;
			delete c.second;
		});
		candidates_.clear();
	}

	void MapPointCandidates::deleteCandidate(PointCandidate& c)
	{
		// ����һ֡������Ȼִ�к�ѡ�㣬������ǲ�������ɾ�������������ѡ�����ɾ����ʶ
		delete c.second; c.second = NULL;
		c.first->type_ = Point3D::TYPE_DELETED;
		trash_points_.push_back(c.first);
	}

	void MapPointCandidates::emptyTrash()
	{
		std::for_each(trash_points_.begin(), trash_points_.end(), [&](Point3D*& p){
			delete p; p = NULL;
		});
		trash_points_.clear();
	}

	Map::Map() {}

	Map::~Map()
	{
		reset();
	}

	void Map::reset()
	{
		keyframes_.clear();//������йؼ�֡
		point_candidates_.reset();//��պ�ѡ��
		emptyTrash();//�Դ洢ɾ�����ĵ�������������
	}

	/// �ڵ�ͼ��ɾ��һ���㣬�����Ƴ����������ص����йؼ�֡
	bool Map::safeDeleteFrame(FramePtr frame)
	{
		bool found = false;
		for (auto it = keyframes_.begin(), ite = keyframes_.end(); it != ite; ++it)
		{
			if (*it == frame)
			{
				std::for_each((*it)->fts_.begin(), (*it)->fts_.end(), [&](Feature* ftr){
					removePtFrameRef(it->get(), ftr);
				});
				keyframes_.erase(it);
				found = true;
				break;
			}
		}

		point_candidates_.removeFrameCandidates(frame);

		if (found)
			return true;

		return false;
	}

	void Map::removePtFrameRef(Frame* frame, Feature* ftr)
	{
		if (ftr->point == NULL)
			return; // ������Ѿ���ɾ��
		Point3D* pt = ftr->point;
		ftr->point = NULL;
		if (pt->obs_.size() <= 2)
		{
			// If the references list of mappoint has only size=2, delete mappoint
			safeDeletePoint(pt);
			return;
		}
		pt->deleteFrameRef(frame);  // Remove reference from map_point
		frame->removeKeyPoint(ftr); // Check if mp was keyMp in keyframe
	}

	void Map::safeDeletePoint(Point3D* pt)
	{
		// �����йؼ�֡��ɾ����Ŀǰ����ӳ��ĵ�
		std::for_each(pt->obs_.begin(), pt->obs_.end(), [&](Feature* ftr){
			ftr->point = NULL;
			ftr->frame->removeKeyPoint(ftr);
		});
		pt->obs_.clear();

		// Delete mappoint
		deletePoint(pt);
	}

	void Map::deletePoint(Point3D* pt)
	{
		pt->type_ = Point3D::TYPE_DELETED;
		trash_points_.push_back(pt);
	}

	void Map::addKeyframe(FramePtr new_keyframe)
	{
		keyframes_.push_back(new_keyframe);
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
					close_kfs.push_back(
						std::make_pair(
						kf, (frame->T_f_w_.translation() - kf->T_f_w_.translation()).norm()));
					break; // ����ؼ�֡��Ŀǰ֡���ص�����Ұ�������close_kfs
				}
			}
		}
	}

	bool frameComparator(std::pair<FramePtr, double>  lhs, std::pair<FramePtr, double>  rhs)
	{
		return (lhs.second < rhs.second);
	}

	FramePtr Map::getClosestKeyframe(const FramePtr& frame) const
	{
		std::list< std::pair<FramePtr, double> > close_kfs;
		getCloseKeyframes(frame, close_kfs);//�õ����ص���Ұ�����йؼ�֡
		if (close_kfs.empty())
		{
			return nullptr;
		}

		// ���������ص��ؼ�֡���ݿ����̶Ƚ�������ʹ��boost��д��
		close_kfs.sort(frameComparator);

		// �жϿ��ܵ�һ���ؼ�֡���ǵ�ǰ֡
		if (close_kfs.front().first != frame)
			return close_kfs.front().first;
		close_kfs.pop_front();
		return close_kfs.front().first;
	}

	FramePtr Map::getFurthestKeyframe(const Vector3d& pos) const
	{
		FramePtr furthest_kf;
		double maxdist = 0.0;
		for (auto it = keyframes_.begin(), ite = keyframes_.end(); it != ite; ++it)
		{
			double dist = ((*it)->pos() - pos).norm();
			if (dist > maxdist) {
				maxdist = dist;
				furthest_kf = *it;
			}
		}
		return furthest_kf;
	}

	bool Map::getKeyframeById(const int id, FramePtr& frame) const
	{
		bool found = false;
		for (auto it = keyframes_.begin(), ite = keyframes_.end(); it != ite; ++it)
			if ((*it)->id_ == id) {
			found = true;
			frame = *it;
			break;
			}
		return found;
	}

	void Map::transform(const Matrix3d& R, const Vector3d& t, const double& s)
	{
		for (auto it = keyframes_.begin(), ite = keyframes_.end(); it != ite; ++it)
		{
			Vector3d pos = s*R*(*it)->pos() + t;
			Matrix3d rot = R*(*it)->T_f_w_.rotation_matrix().inverse();
			(*it)->T_f_w_ = Sophus::SE3(rot, pos).inverse();
			for (auto ftr = (*it)->fts_.begin(); ftr != (*it)->fts_.end(); ++ftr)
			{
				if ((*ftr)->point == NULL)
					continue;
				if ((*ftr)->point->last_published_ts_ == -1000)
					continue;
				(*ftr)->point->last_published_ts_ = -1000;
				(*ftr)->point->pos_ = s*R*(*ftr)->point->pos_ + t;
			}
		}
	}

	void Map::emptyTrash()
	{
		std::for_each(trash_points_.begin(), trash_points_.end(), [&](Point3D*& pt){
			delete pt;
			pt = NULL;
		});
		trash_points_.clear();
		point_candidates_.emptyTrash();
	}

}