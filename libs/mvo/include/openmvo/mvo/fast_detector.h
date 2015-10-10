/*************************************************************************
 * �ļ����� fast_detector
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/8/3
 *
 * ˵���� fast�������ο�rpg_svo(https://github.com/uzh-rpg/rpg_svo)
 *************************************************************************/
#include "openmvo/mvo/abstract_detector.h"
#include "openmvo/mvo/frame.h"

namespace mvo
{
	class FastDetector : public AbstractDetector
	{
	public:
		FastDetector(
			const int img_width,
			const int img_height,
			const int cell_size,
			const int n_pyr_levels);

		virtual ~FastDetector() {}

		virtual void detect(
			Frame* frame,
			const ImgPyr& img_pyr,
			const double detection_threshold,
			Features& fts);

	private:
		float shiTomasiScore(const cv::Mat& img, int u, int v);
	};
}
