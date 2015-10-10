/*************************************************************************
 * �ļ����� abstract_detector
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/8/2
 *
 * ˵���� �ο�rpg_svo(https://github.com/uzh-rpg/rpg_svo)
 *************************************************************************/
#ifndef OPENMVO_MVO_ABSTRACT_DETECTOR_H_
#define OPENMVO_MVO_ABSTRACT_DETECTOR_H_

#include <memory>
#include <vector>
#include "openmvo/mvo/frame.h"
#include "openmvo/mvo/feature.h"

namespace mvo
{
	/// �������ĳ�����
	class AbstractDetector
	{
	public:
		AbstractDetector(
			const int img_width,
			const int img_height,
			const int cell_size,
			const int n_pyr_levels);

		virtual ~AbstractDetector() {};

		virtual void detect(
			Frame* frame,
			const ImgPyr& img_pyr,
			const double detection_threshold,
			Features& fts) = 0;

		/// ���Ѿ����������ĵ�Ԫ������Ϊ��ռ��
		void setExistingFeatures(const Features& fts);

		/// ��ʶ���õ�Ԫ���Ѿ���ռ��
		void setGridOccupancy(const Vector2d& px);
	protected:
		/// �����и����������ã�����Ϊû��ռ��
		void resetGrid();

	protected:

		const int cell_size_;             //!< ����Ѱ�ҽǵ㵥Ԫ��Ĵ�С
		const int n_pyr_levels_;          //!< ͼ��������ĵȼ�
		const int grid_n_cols_;           //!< ��ͼ�񻮷�Ϊ���Ӻ������
		const int grid_n_rows_;           //!< ��ͼ�񻮷�Ϊ���Ӻ������
		std::vector<bool> grid_occupancy_;//!< �趨���ֵ����и������Ƿ�ռ��
	};
	typedef std::shared_ptr<AbstractDetector> DetectorPtr;
}

#endif // OPENMVO_MVO_ABSTRACT_DETECTOR_H_