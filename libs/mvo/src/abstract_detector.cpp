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


}
