/*************************************************************************
 * �ļ����� frame
 *
 * ���ߣ� ���
 * ʱ�䣺 2015/8/1
 *
 * ˵���� ֡����ο�rpg_svo(https://github.com/uzh-rpg/rpg_svo)
 *************************************************************************/
#ifndef OPENMVO_MVO_FRAME_H_
#define OPENMVO_MVO_FRAME_H_

#include <vector>
#include <list>
#include <memory>
#include <opencv2/core/core.hpp>
#include <sophus/se3.h>
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
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		/**	֡��ʵ������ͨ�����������������õĵ�ǰ֡����ʱ�����ȷ��
		 */
		Frame(AbstractCamera* cam, const cv::Mat& img, double timestamp);
		~Frame();
		/// ��ʼ���µ�ͼ��֡������ͼ�������
		void initFrame(const cv::Mat& img);

		/// ��֡���������
		void addFeature(Feature* ftr);

		/// �õ�֡����Ӧ��ԭʼͼ��
		inline const cv::Mat& img() const { return img_pyr_[0]; }
		/// ����������ϵ�еĵ�ת����������
		inline Vector2d w2c(const Vector3d& xyz_w) const { return cam_->world2cam(T_f_w_ * xyz_w); }

		/// ����������ת����λ���������
		inline Vector3d c2f(const Vector2d& px) const { return cam_->cam2world(px[0], px[1]); }

		/// ����������ת����λ���������
		inline Vector3d c2f(const double x, const double y) const { return cam_->cam2world(x, y); }

		/// ����������ϵ�ĵ�ת���������ϵ
		inline Vector3d w2f(const Vector3d& xyz_w) const { return T_f_w_ * xyz_w; }

		///���������ϵ�µĵ�ת����������ϵ��
		inline Vector3d f2w(const Vector3d& f) const { return T_f_w_.inverse() * f; }

		/// ���������ϵ�µĵ�ת��������
		inline Vector2d f2c(const Vector3d& f) const { return cam_->world2cam(f); }
		/// ����֡����������ϵ�е�λ��
		inline Vector3d pos() const { return T_f_w_.inverse().translation(); }

	private:
		/// ͨ������õķ�ʽ����ͼ�������
		void createImgPyramid(const cv::Mat& img_level_0, int n_levels, ImgPyr& pyr);
		
	public:
		static int                    frame_counter_;         //!< ����֡�ļ���������������֡��Ψһid
		int                           id_;                    //!< ֡��Ψһid
		double                        timestamp_;             //!< ֡����¼��ʱ���
		AbstractCamera                *cam_;                  //!< ���ģ��
		Sophus::SE3                   T_f_w_;                 //!< ����������ϵ(w)orldת�����������ϵ(f)rame�����Ա任Rt
		ImgPyr                        img_pyr_;               //!< ͼ�������
		Features                      fts_;                   //!< ͼ���е�����List
	};
	typedef std::shared_ptr<Frame> FramePtr;
}

#endif // OPENMVO_MVO_FRAME_H_

