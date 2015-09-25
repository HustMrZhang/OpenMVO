/*************************************************************************
 * �ļ����� pose_optimizer
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/8/23
 *
 * ˵���� 
 *************************************************************************/
#ifndef OPENMVO_MVO_POSE_OPTIMIZER_H_
#define OPENMVO_MVO_POSE_OPTIMIZER_H_

#include <openmvo/mvo/frame.h>

namespace mvo
{
	/**	��Ҫ����Խ����˶���BA����С����֡��ͶӰ���
	 */
	void poseOptimize(
		const double reproj_thresh,
		const size_t n_iter,
		const bool verbose,
		FramePtr& frame,
		double& estimated_scale,
		double& error_init,
		double& error_final,
		size_t& num_obs);
}

#endif // OPENMVO_MVO_POSE_OPTIMIZER_H_