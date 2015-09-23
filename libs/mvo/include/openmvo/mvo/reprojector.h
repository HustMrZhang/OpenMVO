/*************************************************************************
 * �ļ����� reprojector
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/9/17
 *
 * ˵���� 
 *************************************************************************/
#ifndef OPENMVO_MVO_REPROJECTOR_H_
#define OPENMVO_MVO_REPROJECTOR_H_

#include <vector>
#include <Eigen/Core>
#include <openmvo/mvo/frame.h>
#include <openmvo/mvo/matcher.h>

namespace mvo
{
	class AbstractCamera;
	class Map;
	class Point3D;

	/// �Ե�ͼ�еĵ�ͶӰ��ͼ���У��ҵ�һ���������ǵ㣩�����ǲ������еĵ����ƥ�䣬ֻ��һ��
	/// ��Ԫ������ƥ��һ���㣬���������ǿ��Զ����е�ƥ���������о��ȷֲ������Ҳ���ͶӰ���еĵ�
	/// �������Ը��õĽ�Լʱ�䡣
	class Reprojector
	{
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Reprojector(AbstractCamera* cam, Map& map);

		~Reprojector();

		/// �ӵ�ͼ�е�ͶӰ��ͼ���У��ҵ����ص���Ұ�Ĺؼ�֡��������Щmappoints����ͶӰ
		void reprojectMap(
			FramePtr frame,
			std::vector< std::pair<FramePtr, std::size_t> >& overlap_kfs);

	public:
			/// ��ͶӰ�����ò���
		struct Options {
			size_t max_n_kfs;   //!< �Ե�ǰ֡Ϊ�ο���������ͶӰ�Ĺؼ�֡�������Ŀ
			bool find_match_direct;
			Options()
				: max_n_kfs(10),
				find_match_direct(true)
			{}
		} options_;

		size_t n_matches_;
		size_t n_trials_;

		

	private:

		/// candidate��һ��3D��Ͷ�Ӧ��ͶӰ��2D���أ�������ͼ��������õ�ƥ�������
		struct Candidate {
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW
				Point3D* pt;       //!< 3D��
			Vector2d px;     //!< ͶӰ��2D���ص�
			Candidate(Point3D* pt, Vector2d& px) : pt(pt), px(px) {}
		};
		typedef std::list<Candidate, aligned_allocator<Candidate> > Cell;//!< ���ڴ��3D��Ͷ�Ӧ��ͶӰ2D���������б�
		typedef std::vector<Cell*> CandidateGrid;

		/// grid���ڴ洢һϵ�к�ѡƥ��.����ÿһ��grid��Ԫ��Ŭ��Ѱ��һ��ƥ��
		struct Grid
		{
			CandidateGrid cells;//!< ���ڴ��3D��Ͷ�Ӧ��ͶӰ2D��������
			std::vector<int> cell_order;//!< ��Ԫ���˳����
			int cell_size;//!< ��Ӧ��Ĵ�С
			int grid_n_cols;//!< ͼ�񻮷ֵ�Ԫ�������
			int grid_n_rows;//!< ͼ�񻮷ֵ�Ԫ�������
		};

		Grid grid_;//!< ͼ�񻮷�Ϊ����
		Matcher matcher_;
		Map& map_;//!< �洢3D��͹ؼ�֡�ĵ�ͼ��Ϣ

		void initializeGrid(AbstractCamera* cam);
		/// ��������grid����ÿ��grid�еĺ�ѡ��������
		void resetGrid();

		bool reprojectCell(Cell& cell, FramePtr frame);
		bool reprojectPoint(FramePtr frame, Point3D* point);
	};
}

#endif // OPENMVO_MVO_REPROJECTOR_H_