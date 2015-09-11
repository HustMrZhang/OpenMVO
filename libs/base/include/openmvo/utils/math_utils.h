/*************************************************************************
 * �ļ����� math_utils
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/9/7
 *
 * ˵���� 
 *************************************************************************/
#ifndef OPENMVO_UTILS_MATH_UTILS_H_
#define OPENMVO_UTILS_MATH_UTILS_H_
#include <vector>
#include <algorithm>
#include <Eigen/Dense>

namespace mvo
{
	using namespace Eigen;

	Vector3d triangulateFeatureNonLin(
		const Matrix3d& R,
		const Vector3d& t,
		const Vector3d& feature1,
		const Vector3d& feature2);

	double reprojError(
		const Vector3d& f1,
		const Vector3d& f2,
		double focal_length);

	double computeInliers(
		const std::vector<Vector3d>& features1,
		const std::vector<Vector3d>& features2,
		const Matrix3d& R,
		const Vector3d& t,
		const double reproj_thresh,
		double focal_length,
		std::vector<Vector3d>& xyz_vec,
		std::vector<int>& inliers,
		std::vector<int>& outliers);

	/// ͶӰ�����������ϵ������ת������������
	inline Vector2d project2d(const Vector3d& v)
	{
		return v.head<2>() / v[2];
	}

	/// ��ͶӰ�������ص���������ת����ڵ����������
	inline Vector3d unproject2d(const Vector2d& v)
	{
		return Vector3d(v[0], v[1], 1.0);
	}
	///�������Գƾ���
	inline Matrix3d sqew(const Vector3d& v)
	{
		Matrix3d v_sqew;
		v_sqew << 0, -v[2], v[1],
			v[2], 0, -v[0],
			-v[1], v[0], 0;
		return v_sqew;
	}
	double sampsonusError(
		const Vector2d &v2Dash,
		const Matrix3d& m3Essential,
		const Vector2d& v2);
	///������ֵ
	template<class T>
	T getMedian(std::vector<T>& data_vec)
	{
		assert(!data_vec.empty());
		typename std::vector<T>::iterator it = data_vec.begin() + floor(data_vec.size() / 2);
		nth_element(data_vec.begin(), it, data_vec.end());//��ǰn������������
		return *it;
	}
}

#endif // OPENMVO_UTILS_MATH_UTILS_H_