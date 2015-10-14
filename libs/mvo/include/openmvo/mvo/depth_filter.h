/*************************************************************************
 * �ļ����� depth_filter
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/8/29
 *
 * ˵���� 
 *************************************************************************/
#ifndef OPENMVO_MVO_DEPTH_FILTER_H_
#define OPENMVO_MVO_DEPTH_FILTER_H_
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <Eigen/Core>
#include <openmvo/mvo/feature.h>
#include <openmvo/mvo/abstract_detector.h>
#include <openmvo/mvo/matcher.h>

namespace mvo
{
	///һ��seed����һ�����صĸ�������ȹ���
	struct Seed
	{
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		static int batch_counter;    //!< �����������ӵ��Ӧ֡����Ŀ
		static int seed_counter;     //!< �����������ӵ��Ψһid
		int batch_id;                //!< batch_id�����ӵ㱻��������Ӧ�Ĺؼ�֡��id
		int id;                      //!< ����ID,���������ӻ���ʾ
		Feature* ftr;                //!< �ڹؼ�֡�ϵ���������Щ�����������Ҫ������
		float a;                     //!< Beta�ֲ��Ĳ���a: aԽ�ߣ����ڵ�ĸ��ʾ�Խ��
		float b;                     //!< Beta�ֲ��Ĳ���b: bԽ�ߣ������ĸ��ʾ�Խ��
		float mu;                    //!< ��̬�ֲ��ľ�ֵ
		float z_range;               //!< ������ȵ����Χ
		float sigma2;                //!< ��̬�ֲ��ķ���
		Seed(Feature* ftr, float depth_mean, float depth_min);
	};

	class DepthFilter
	{
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		typedef std::unique_lock<std::mutex> lock_t;//��ռ��
		typedef std::function<void(Point3D*, double)> callback_t;//�󶨺���ָ��

		DepthFilter(
			DetectorPtr feature_detector,
			callback_t seed_converged_cb);

		virtual ~DepthFilter();

		/// �������ӵ�����߳�
		void startThread();

		/// ֹͣ�������е��߳�
		void stopThread();

		/// ���֡��������Ķ���
		void addFrame(FramePtr frame);

		/// ����µĹؼ�֡������
		void addKeyframe(FramePtr frame, double depth_mean, double depth_min);

		void removeKeyframe(FramePtr frame);

		void reset();

		/// ��Ҷ˹����¸������ӵ㣬x��ʾ������tau2��ʾ�����Ĳ�ȷ����
		static void updateSeed(
			const float x,
			const float tau2,
			Seed* seed);

		/// ��������Ĳ�ȷ����
		static double computeTau(
			const SE3& T_ref_cur,
			const Vector3d& f,
			const double z,
			const double px_error_angle);
	protected:
		/// ��һ��֡�г�ʼ���µ����ӵ��б�
		void initializeSeeds(FramePtr frame);

		/// ����һ���µĲ���֡�������е����ӵ�
		virtual void updateSeeds(FramePtr frame);

		/// ������һ���ؼ�֡����֡�Ķ��н������
		void clearFrameQueue();

		/// һ���߳������������ϵĸ������ӵ�
		void updateSeedsLoop();
	public:
		/// Depth-filter���ò���
		struct Options
		{
			bool verbose;                               //!< �Ƿ���ʾ���
			int max_n_kfs;                              //!< ����ά�����ӵ�Ĺؼ�֡�������Ŀ
			double sigma_i_sq;                          //!< ͼ������
			double seed_convergence_sigma2_thresh;      //!< threshold on depth uncertainty for convergence.
			Options()
				: verbose(false),
				max_n_kfs(3),
				sigma_i_sq(5e-4),
				seed_convergence_sigma2_thresh(200.0)
			{}
		} options_;
	protected:
		DetectorPtr feature_detector_;        //!< �������
		callback_t seed_converged_cb_;        //!< �󶨵Ļص�����
		bool seeds_updating_halt_;            //!< ֵ��Ϊtrue�������������ӵ����ɣ���Ҫ�жϣ���ȷ�����ӵ��б����
		std::list<Seed, aligned_allocator<Seed> > seeds_;//!< ���ӵ��б�
		bool new_keyframe_set_;               //!< �Ƿ���һ���µĹؼ�֡������
		double new_keyframe_min_depth_;       //!< ���¹ؼ�֡�е���С��ȣ����������ӵ�ķ�Χ
		double new_keyframe_mean_depth_;      //!< ���¹ؼ�֡�е������ȣ����������ӵ�ķ�Χ
		std::mutex seeds_mut_;                //!< �����������ڶ����ӵ�ĸ��¿���	
		std::thread *thread_;                 //!< Ŀǰ�����߳�
		std::mutex frame_queue_mut_;
		std::condition_variable frame_queue_cond_;
		std::queue<FramePtr> frame_queue_;
		FramePtr new_keyframe_;               //!< ��һ���ؼ�֡������ȡ�µ����ӵ�
		bool is_runing; //!<��ʾ�߳��Ƿ�����ִ��
		Matcher matcher_;
	};
}

#endif // OPENMVO_MVO_DEPTH_FILTER_H_