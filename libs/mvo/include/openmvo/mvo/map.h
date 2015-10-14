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
#include <mutex>
#include <openmvo/utils/noncopyable.h>
#include <openmvo/mvo/frame.h>

namespace mvo
{
	class Point3D;

	/// �������ڶ���Щ3D��û�з��䵽�����ؼ�֡��
	class MapPointCandidates
	{
	public:
		typedef std::pair<Point3D*, Feature*> PointCandidate;//!< ��ѡ�㣬pair�ɵ�͵��Ӧ����������
		typedef std::list<PointCandidate> PointCandidateList;//!< ��ѡ��list

		/// ͨ��depth-filter�̶߳Ժ�ѡ�������䣬ͨ��������������ƶԺ�ѡ��Ķ�ȡ����
		std::mutex mut_;

		/// �����ӵ�����ںϴ���õ���ѡ��
		/// ֪����һ���ؼ�֡����Щ��������ͶӰ����̬�Ż�
		PointCandidateList candidates_;
		std::list< Point3D* > trash_points_;

		MapPointCandidates();
		~MapPointCandidates();

		/// ���һ����ѡ��
		void newCandidatePoint(Point3D* point, double depth_sigma2);

		/// ���������֡�������б���ɾ����ѡ
		void addCandidatePointToFrame(FramePtr frame);

		/// �ں�ѡ�б���ɾ��һ����ѡ��
		bool deleteCandidatePoint(Point3D* point);

		///ɾ���������֡�����к�ѡ��
		void removeFrameCandidates(FramePtr frame);

		/// �Ժ�ѡ���list�������ã��Ƴ���ɾ�����еĵ�
		void reset();

		void deleteCandidate(PointCandidate& c);

		void emptyTrash();
	};

	class Map : public Noncopyable
	{
	public:
		Map();
		~Map();
		/// �������õ�ͼ��ɾ�����еĹؼ�֡����������֡�͵�ļ�����
		void reset();

		/// �ڵ�ͼ��ɾ��һ���㣬�����Ƴ����������ص����йؼ�֡
		void safeDeletePoint(Point3D* pt);

		/// �Ƴ��㵽��������
		void deletePoint(Point3D* pt);

		/// �Ƴ�֡����������
		bool safeDeleteFrame(FramePtr frame);

		/// �Ƴ�����֮֡��Ĺ���
		void removePtFrameRef(Frame* frame, Feature* ftr);

		/// ����ͼ�����һ���µĹؼ�֡
		void addKeyframe(FramePtr new_keyframe);

		/// �õ���Ŀǰ֡���ص���Ұ�����йؼ�֡
		void getCloseKeyframes(const FramePtr& frame, std::list< std::pair<FramePtr, double> >& close_kfs) const;

		/// ������Ŀǰ֡�ڿռ�����������ص���Ұ�Ĺؼ�֡
		FramePtr getClosestKeyframe(const FramePtr& frame) const;

		/// ������Ŀǰλ����Զ�Ĺؼ�֡�����ڿ��ƹؼ�֡������
		FramePtr getFurthestKeyframe(const Vector3d& pos) const;

		/// ����id�õ��ؼ�֡
		bool getKeyframeById(const int id, FramePtr& frame) const;

		/// ��������ͼ���б任��������תR��ƽ��t��������s
		void transform(const Matrix3d& R, const Vector3d& t, const double& s);

		void emptyTrash();

		/// ������������ͼ�еĹؼ�֡
		inline FramePtr lastKeyframe() { return keyframes_.back(); }

		/// �����ڵ�ͼ�йؼ�֡����Ŀ
		inline size_t size() const { return keyframes_.size(); }
	public:
		std::list< FramePtr > keyframes_;          //!< ��ͼ�д洢�����йؼ�֡
		//std::list< Point3D* >   points_;         //!< ���3D��
		std::list< Point3D* > trash_points_;         //!< ���ɾ�����ĵ�
		MapPointCandidates point_candidates_;
	};
}


#endif // OPENMVO_MVO_MAP_H_