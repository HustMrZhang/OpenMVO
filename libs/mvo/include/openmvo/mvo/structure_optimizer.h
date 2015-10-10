/*************************************************************************
 * �ļ����� structure_optimizer
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/8/24
 *
 * ˵���� 
 *************************************************************************/
#ifndef OPENMVO_MVO_STRUCTURE_OPTIMIZER_H_
#define OPENMVO_MVO_STRUCTURE_OPTIMIZER_H_
#include <openmvo/mvo/frame.h>

namespace mvo
{
	/// ��һЩ�۲쵽��3D������Ż�
	void structureOptimize(FramePtr frame, size_t max_n_pts, int max_iter);
}

#endif // OPENMVO_MVO_STRUCTURE_OPTIMIZER_H_