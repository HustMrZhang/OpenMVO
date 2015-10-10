/*************************************************************************
 * �ļ����� abstract_detector
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/8/4
 *
 * ˵���� 
 *************************************************************************/
#include "openmvo/mvo/abstract_detector.h"

namespace mvo
{
	AbstractDetector::AbstractDetector(
		const int img_width,
		const int img_height,
		const int cell_size,
		const int n_pyr_levels) :
		cell_size_(cell_size),
		n_pyr_levels_(n_pyr_levels),
		grid_n_cols_(ceil(static_cast<double>(img_width) / cell_size_)),
		grid_n_rows_(ceil(static_cast<double>(img_height) / cell_size_)),
		grid_occupancy_(grid_n_cols_*grid_n_rows_, false)
	{}

	void AbstractDetector::resetGrid()
	{
		std::fill(grid_occupancy_.begin(), grid_occupancy_.end(), false);
	}

	/// ���Ѿ����������ĵ�Ԫ������Ϊ��ռ��
	void AbstractDetector::setExistingFeatures(const Features& fts)
	{
		std::for_each(fts.begin(), fts.end(), [&](Feature* i){
			grid_occupancy_.at(
				static_cast<int>(i->px[1] / cell_size_)*grid_n_cols_
				+ static_cast<int>(i->px[0] / cell_size_)) = true;
		});
	}

	void AbstractDetector::setGridOccupancy(const Vector2d& px)
	{
		grid_occupancy_.at(
			static_cast<int>(px[1] / cell_size_)*grid_n_cols_
			+ static_cast<int>(px[0] / cell_size_)) = true;
	}

}
