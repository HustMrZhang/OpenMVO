/*************************************************************************
 * �ļ����� reprojector
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/9/17
 *
 * ˵���� 
 *************************************************************************/
#include <openmvo/mvo/reprojector.h>

#include <openmvo/mvo/config.h>
#include <openmvo/mvo/map.h>
#include <openmvo/mvo/feature.h>

namespace mvo
{
	Reprojector::Reprojector(AbstractCamera* cam, Map& map) :
		map_(map)
	{
		initializeGrid(cam);
	}

	Reprojector::~Reprojector()
	{
		std::for_each(grid_.cells.begin(), grid_.cells.end(), [&](Cell* c){ delete c; });
	}

	void Reprojector::initializeGrid(AbstractCamera* cam)
	{
		grid_.cell_size = Config::gridSize();
		grid_.grid_n_cols = ceil(static_cast<double>(cam->width()) / grid_.cell_size);
		grid_.grid_n_rows = ceil(static_cast<double>(cam->height()) / grid_.cell_size);
		grid_.cells.resize(grid_.grid_n_cols*grid_.grid_n_rows);
		// ��ʼ����Ԫ����������ɾ��
		std::for_each(grid_.cells.begin(), grid_.cells.end(), [&](Cell*& c){ c = new Cell; });
		grid_.cell_order.resize(grid_.cells.size());
		for (size_t i = 0; i < grid_.cells.size(); ++i)
			grid_.cell_order[i] = i;
		std::random_shuffle(grid_.cell_order.begin(), grid_.cell_order.end()); // ������У�һ�ֲ���
	}

	/// ��������grid����ÿ��grid�еĺ�ѡ��������
	void Reprojector::resetGrid()
	{
		n_matches_ = 0;
		n_trials_ = 0;
		std::for_each(grid_.cells.begin(), grid_.cells.end(), [&](Cell* c){ c->clear(); });
	}

	bool compareDistance(std::pair<FramePtr, double>  &x, std::pair<FramePtr, double> &y)
	{
		if (x.second < y.second)
			return true;
		else
			return false;
	}

	void Reprojector::reprojectMap(
		FramePtr frame,
		std::vector< std::pair<FramePtr, std::size_t> >& overlap_kfs)
	{
		resetGrid();

		// ѡ����Ŀǰ֡���ص���Ұ�Ĺؼ�֡
		std::list< std::pair<FramePtr, double> > close_kfs;
		map_.getCloseKeyframes(frame, close_kfs);

		// �Կ����Ĺؼ�֡���ݿ����̶Ƚ�������
		close_kfs.sort(compareDistance);

		// �������ص����ֵ�N���ؼ�֡��Ӧ��mappoints������ͶӰ������ֻ�洢��������������ٵ�
		size_t n = 0;
		overlap_kfs.reserve(options_.max_n_kfs);
		// �������N���ؼ�֡���е������ҵ����ص���Ұ
		for (auto it_frame = close_kfs.begin(), ite_frame = close_kfs.end();
			it_frame != ite_frame && n < options_.max_n_kfs; ++it_frame, ++n)
		{
			FramePtr ref_frame = it_frame->first;
			overlap_kfs.push_back(std::pair<FramePtr, size_t>(ref_frame, 0));

			// ������ο�֡�۲쵽�ĵ�ͶӰ����ǰ֡��
			for (auto it_ftr = ref_frame->fts_.begin(), ite_ftr = ref_frame->fts_.end();
				it_ftr != ite_ftr; ++it_ftr)
			{
				// �����������Ƿ��з����mappoint
				if ((*it_ftr)->point == NULL)
					continue;

				// ȷ������ֻͶӰһ�Σ���ͬ֡�ϵ��������Ӧͬһ��3D��
				if ((*it_ftr)->point->last_projected_kf_id_ == frame->id_)
					continue;
				(*it_ftr)->point->last_projected_kf_id_ = frame->id_;
				if (reprojectPoint(frame, (*it_ftr)->point))
					overlap_kfs.back().second++;//��ͬ�۲�����Ŀ
			}
		}

		// ���ڱ������еĵ�Ԫ��ѡ��һ�������ƥ��
		for (size_t i = 0; i<grid_.cells.size(); ++i)
		{
			// ��������Ѱ�������õĵ�
			if (reprojectCell(*grid_.cells.at(grid_.cell_order[i]), frame))
				++n_matches_;
			if (n_matches_ >(size_t) Config::maxFts())
				break;
		}

	}

	/// ��ͶӰ3D�㣬��ͶӰ��֡�ڵ����ص㣬��Ϊ��ѡ����
	bool Reprojector::reprojectPoint(FramePtr frame, Point3D* point)
	{
		Vector2d px(frame->w2c(point->pos_));//����������ĵ�ת������
		if (frame->cam_->isInFrame(px.cast<int>(), 8)) // �ж������Ƿ���֡�У��ṩ8px�ı߽�
		{
			// �������ĸ�������
			const int k = static_cast<int>(px[1] / grid_.cell_size)*grid_.grid_n_cols
				+ static_cast<int>(px[0] / grid_.cell_size);
			grid_.cells.at(k)->push_back(Candidate(point, px));// �ڸ�������Ӻ�ѡ����
			return true;
		}
		return false;
	}

	bool Reprojector::reprojectCell(Cell& cell, FramePtr frame)
	{
		Cell::iterator it = cell.begin();
		while (it != cell.end())
		{
			++n_trials_;//��ͶӰ��Ԫ��Ĵ���

			bool found_match = true;
			if (options_.find_match_direct)
				found_match = matcher_.findMatchDirect(*it->pt, *frame, it->px);
			if (!found_match)
			{
				it->pt->n_failed_reproj_++;
				//TODO:��Map���д���
				it = cell.erase(it);
				continue;
			}
			it->pt->n_succeeded_reproj_++;
			
			Feature* new_feature = new Feature(frame.get(), it->px, matcher_.search_level_);
			frame->addFeature(new_feature);

			// ���������ӵ㵽����������
			new_feature->point = it->pt;

			// ����ؼ�֡�Ѿ�ѡ��Ҳ��ͶӰ�����㣬�����ǲ���Ҫ��������
			it = cell.erase(it);

			// ÿ����Ԫ���б�֤��һ����
			return true;
		}
		return false;
	}


}