/*************************************************************************
 * �ļ����� pinhole_camera
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/8/1
 *
 * ˵���� �ο�rpg_svo(https://github.com/uzh-rpg/rpg_svo)
 *************************************************************************/
#ifndef OPENMVO_MVO_PINHOLE_CAMERA_H_
#define OPENMVO_MVO_PINHOLE_CAMERA_H_

#include <opencv2/opencv.hpp>
#include "openmvo/mvo/abstract_camera.h"

namespace mvo
{
	class PinholeCamera : public AbstractCamera {
	
	public:
		// ���ǻ������k1,k2,p1,p2,k3
		PinholeCamera(double width, double height,
			double fx, double fy, double cx, double cy,
			double k1 = 0.0, double k2 = 0.0, double p1 = 0.0, double p2 = 0.0, double k3 = 0.0);

		~PinholeCamera();

		/// ͼ����������ת���������ϵ�µĵ�
		virtual Vector3d cam2world(const double& x, const double& y) const;

		/// ͼ����������ת���������ϵ�µĵ�
		virtual Vector3d cam2world(const Vector2d& px) const;

		/// ���������ϵ�µĵ�תͼ����������
		virtual Vector2d world2cam(const Vector3d& xyz_c) const;

		/// ͼ��ƽ�����ص���������ת��������
		virtual Vector2d world2cam(const Vector2d& uv) const;

		/// ����x����Ľ���ֵ
		virtual double getFocalLength() const
		{
			return fabs(fx_);
		}

		/// ��ý���֮���ͼ����Ҫ������ʾ
		void undistortImage(const cv::Mat& raw, cv::Mat& rectified);

		/// �ֱ�õ���������4������
		inline double fx() const { return fx_; };
		inline double fy() const { return fy_; };
		inline double cx() const { return cx_; };
		inline double cy() const { return cy_; };

	private:
		double fx_, fy_;  //!< �����������Ľ���ֵ
		double cx_, cy_;  //!< ��������ĵ�
		bool distortion_; //!< �ǵ�����С�����ģ�ͣ����Ǵ��л��䣿
		double d_[5];     //!< ����������ο� http://docs.opencv.org/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html
		cv::Mat cvK_, cvD_;//!< ͨ��OpenCV��ʾ�������������������������
		cv::Mat undist_map1_, undist_map2_;//!<������������������map���ṩ��remap����ʹ��
	};
}

#endif // OPENMVO_MVO_PINHOLE_CAMERA_H_
