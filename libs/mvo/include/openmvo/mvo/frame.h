/*************************************************************************
 * �ļ����� frame
 *
 * ���ߣ� ���
 * ʱ�䣺 2015/8/1
 *
 * ˵���� ֡����
 *************************************************************************/
#ifndef OPENMVO_MVO_FRAME_H_
#define OPENMVO_MVO_FRAME_H_

#include <vector>
#include <list>
#include <memory>
#include <opencv2/core/core.hpp>
#include "openmvo/utils/noncopyable.h"
#include "openmvo/mvo/abstract_camera.h"

namespace mvo{

	struct Feature;

	typedef std::list<Feature*> Features;//����list
	typedef std::vector<cv::Mat> ImgPyr;//ͼ�������

	/**	����֡����֤֡��Ψһ��
	 */
	class Frame : public Noncopyable
	{
	public:
		/**	֡��ʵ������ͨ�����������������õĵ�ǰ֡����ʱ�����ȷ��
		 */
		Frame(AbstractCamera* cam, const cv::Mat& img, double timestamp);
		~Frame();
		/// ��ʼ���µ�ͼ��֡������ͼ�������
		void initFrame(const cv::Mat& img);

	private:
		/// ͨ������õķ�ʽ����ͼ�������
		void createImgPyramid(const cv::Mat& img_level_0, int n_levels, ImgPyr& pyr);
		
	public:
		static int                    frame_counter_;         //!< ����֡�ļ���������������֡��Ψһid
		int                           id_;                    //!< ֡��Ψһid
		double                        timestamp_;             //!< ֡����¼��ʱ���
		AbstractCamera                *cam_;                  //!< ���ģ��
		ImgPyr                        img_pyr_;               //!< ͼ�������
		Features                      fts_;                   //!< ͼ���е�����List
	};
	typedef std::shared_ptr<Frame> FramePtr;
}

#endif // OPENMVO_MVO_FRAME_H_

