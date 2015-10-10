/*************************************************************************
 * �ļ����� matcher
 *
 * ���ߣ� ���
 * �ʼ��� fengbing123@gmail.com
 * ʱ�䣺 2015/8/18
 *
 * ˵���� �ο�rpg_svo(https://github.com/uzh-rpg/rpg_svo)
 *************************************************************************/
#include <openmvo/mvo/matcher.h>
#include <openmvo/mvo/config.h>
#include <openmvo/mvo/feature.h>
#include <openmvo/utils/image_utils.h>
#include <openmvo/mvo/feature_alignment.h>
#include <openmvo/utils/math_utils.h>
#include <openmvo/mvo/patch_score.h>

namespace mvo
{
	bool Matcher::findMatchDirect(
		const Point3D& pt,
		const Frame& cur_frame,
		Vector2d& px_cur)
	{
		Feature* ref_ftr;// ͨ���õ����֡��Ӧ������
		if (!pt.getCloseViewObs(cur_frame.pos(), ref_ftr))
			return false;

		if (!ref_ftr->frame->cam_->isInFrame(
			ref_ftr->px.cast<int>() / (1 << ref_ftr->level), halfpatch_size_ + 2, ref_ftr->level))
			return false;

		// ���з���任
		getWarpMatrixAffine(
			*ref_ftr->frame->cam_, *cur_frame.cam_, ref_ftr->px, ref_ftr->f,
			(ref_ftr->frame->pos() - pt.pos_).norm(),
			cur_frame.T_f_w_ * ref_ftr->frame->T_f_w_.inverse(), ref_ftr->level, A_cur_ref_);
		search_level_ = getBestSearchLevel(A_cur_ref_, Config::nPyrLevels() - 1);
		warpAffine(A_cur_ref_, ref_ftr->frame->img_pyr_[ref_ftr->level], ref_ftr->px,
			ref_ftr->level, search_level_, halfpatch_size_ + 1, patch_with_border_);
		createPatchFromPatchWithBorder();

		// ���ó߶�Ϊ1������px_cur
		Vector2d px_scaled(px_cur / (1 << search_level_));

		bool success = false;
		
		success = feature_alignment::align2D(
				cur_frame.img_pyr_[search_level_], patch_with_border_, patch_,
				options_.align_max_iter, px_scaled);
		
		px_cur = px_scaled * (1 << search_level_);
		return success;
	}

	void Matcher::getWarpMatrixAffine(
		const AbstractCamera& cam_ref,
		const AbstractCamera& cam_cur,
		const Vector2d& px_ref,
		const Vector3d& f_ref,
		const double depth_ref,
		const SE3& T_cur_ref,
		const int level_ref,
		Matrix2d& A_cur_ref)
	{
		// �������任����A_ref_cur
		const int halfpatch_size = 5;
		const Vector3d xyz_ref(f_ref*depth_ref);
		Vector3d xyz_du_ref(cam_ref.cam2world(px_ref + Vector2d(halfpatch_size, 0)*(1 << level_ref)));
		Vector3d xyz_dv_ref(cam_ref.cam2world(px_ref + Vector2d(0, halfpatch_size)*(1 << level_ref)));
		xyz_du_ref *= xyz_ref[2] / xyz_du_ref[2];
		xyz_dv_ref *= xyz_ref[2] / xyz_dv_ref[2];
		const Vector2d px_cur(cam_cur.world2cam(T_cur_ref*(xyz_ref)));
		const Vector2d px_du(cam_cur.world2cam(T_cur_ref*(xyz_du_ref)));
		const Vector2d px_dv(cam_cur.world2cam(T_cur_ref*(xyz_dv_ref)));
		A_cur_ref.col(0) = (px_du - px_cur) / halfpatch_size;
		A_cur_ref.col(1) = (px_dv - px_cur) / halfpatch_size;
	}

	int Matcher::getBestSearchLevel(const Matrix2d& A_cur_ref, const int max_level)
	{
		// ����������ͼ������Ƭ�����������ȼ�
		int search_level = 0;
		double D = A_cur_ref.determinant();
		while (D > 3.0 && search_level < max_level)
		{
			search_level += 1;
			D *= 0.25;
		}
		return search_level;
	}

	void Matcher::warpAffine(
		const Matrix2d& A_cur_ref,
		const cv::Mat& img_ref,
		const Vector2d& px_ref,
		const int level_ref,
		const int search_level,
		const int halfpatch_size,
		uint8_t* patch)
	{
		const int patch_size = halfpatch_size * 2;
		const Matrix2f A_ref_cur = A_cur_ref.inverse().cast<float>();
		if (isnan(A_ref_cur(0, 0)))
		{
			printf("Affine warp is NaN, probably camera has no translation\n"); // TODO
			return;
		}

		// ����Ƭִ��warp����
		uint8_t* patch_ptr = patch;
		const Vector2f px_ref_pyr = px_ref.cast<float>() / (1 << level_ref);
		for (int y = 0; y < patch_size; ++y)
		{
			for (int x = 0; x < patch_size; ++x, ++patch_ptr)
			{
				Vector2f px_patch(x - halfpatch_size, y - halfpatch_size);
				px_patch *= (1 << search_level);
				const Vector2f px(A_ref_cur*px_patch + px_ref_pyr);// ���з���任
				if (px[0] < 0 || px[1] < 0 || px[0] >= img_ref.cols - 1 || px[1] >= img_ref.rows - 1)
					*patch_ptr = 0;
				else
					*patch_ptr = (uint8_t)interpolateMat_8u(img_ref, px[0], px[1]);
			}
		}
	}

	void Matcher::createPatchFromPatchWithBorder()
	{
		uint8_t* ref_patch_ptr = patch_;
		for (int y = 1; y < patch_size_ + 1; ++y, ref_patch_ptr += patch_size_)
		{
			uint8_t* ref_patch_border_ptr = patch_with_border_ + y*(patch_size_ + 2) + 1;
			for (int x = 0; x < patch_size_; ++x)
				ref_patch_ptr[x] = ref_patch_border_ptr[x];
		}
	}

	bool Matcher::findEpipolarMatchDirect(
		const Frame& ref_frame,
		const Frame& cur_frame,
		const Feature& ref_ftr,
		const double d_estimate,
		const double d_min,
		const double d_max,
		double& depth)
	{
		SE3 T_cur_ref = cur_frame.T_f_w_ * ref_frame.T_f_w_.inverse();// ���㵱ǰ֡��������֡�ı任
		int zmssd_best = PatchScore::threshold();
		Vector2d uv_best;

		//�ھɵĹؼ�֡�ϼ��㼫�ߵĿ�ʼ�ͽ�������ƥ��
		Vector2d A = project2d(T_cur_ref * (ref_ftr.f*d_min));
		Vector2d B = project2d(T_cur_ref * (ref_ftr.f*d_max));
		epi_dir_ = A - B;//���߷���

		// �������任����
		getWarpMatrixAffine(
			*ref_frame.cam_, *cur_frame.cam_, ref_ftr.px, ref_ftr.f,
			d_estimate, T_cur_ref, ref_ftr.level, A_cur_ref_);

		search_level_ = getBestSearchLevel(A_cur_ref_, Config::nPyrLevels() - 1);

		// ת����������
		Vector2d px_A(cur_frame.cam_->world2cam(A));
		Vector2d px_B(cur_frame.cam_->world2cam(B));
		// �������������ĳ���
		epi_length_ = (px_A - px_B).norm() / (1 << search_level_);

		// ����Ƭ���з���任
		warpAffine(A_cur_ref_, ref_frame.img_pyr_[ref_ftr.level], ref_ftr.px,
			ref_ftr.level, search_level_, halfpatch_size_ + 1, patch_with_border_);
		createPatchFromPatchWithBorder();

		if (epi_length_ < 2.0)
		{
			px_cur_ = (px_A + px_B) / 2.0;
			Vector2d px_scaled(px_cur_ / (1 << search_level_));
			bool res = feature_alignment::align2D(
				cur_frame.img_pyr_[search_level_], patch_with_border_, patch_,
				options_.align_max_iter, px_scaled);
			if (res)
			{
				px_cur_ = px_scaled*(1 << search_level_);
				if (depthFromTriangulation(T_cur_ref, ref_ftr.f, cur_frame.cam_->cam2world(px_cur_), depth))
					return true;
			}
			return false;
		}

		size_t n_steps = epi_length_ / 0.7; // �ؼ��߲��ҵĲ�����Ŀ
		Vector2d step = epi_dir_ / n_steps;

		if (n_steps > options_.max_epi_search_steps)
		{
			printf("WARNING: skip epipolar search: %d evaluations, px_lenght=%f, d_min=%f, d_max=%f.\n",
				n_steps, epi_length_, d_min, d_max);
			return false;
		}

		PatchScore patch_score(patch_);

		// ���ż��߽���ȡ��
		Vector2d uv = B - step;
		Vector2i last_checked_pxi(0, 0);
		++n_steps;
		for (size_t i = 0; i < n_steps; ++i, uv += step)
		{
			Vector2d px(cur_frame.cam_->world2cam(uv));// ת����
			Vector2i pxi(px[0] / (1 << search_level_) + 0.5,
				px[1] / (1 << search_level_) + 0.5); // +0.5 ת����ӽ�������

			if (pxi == last_checked_pxi)
				continue;
			last_checked_pxi = pxi;

			// �����Ƭ�Ƿ�����֡��ȫ���ɼ�
			if (!cur_frame.cam_->isInFrame(pxi, patch_size_, search_level_))
				continue;

			// TODO����߿��Խ��в�ֵ
			uint8_t* cur_patch_ptr = cur_frame.img_pyr_[search_level_].data
				+ (pxi[1] - halfpatch_size_)*cur_frame.img_pyr_[search_level_].cols
				+ (pxi[0] - halfpatch_size_);
			int zmssd = patch_score.computeScore(cur_patch_ptr, cur_frame.img_pyr_[search_level_].cols);

			if (zmssd < zmssd_best) {
				zmssd_best = zmssd;
				uv_best = uv;
			}
		}

		if (zmssd_best < PatchScore::threshold())
		{
			if (options_.subpix_refinement)
			{
				px_cur_ = cur_frame.cam_->world2cam(uv_best);
				Vector2d px_scaled(px_cur_ / (1 << search_level_));
				bool res = feature_alignment::align2D(
					cur_frame.img_pyr_[search_level_], patch_with_border_, patch_,
					options_.align_max_iter, px_scaled);
				if (res)
				{
					px_cur_ = px_scaled*(1 << search_level_);
					if (depthFromTriangulation(T_cur_ref, ref_ftr.f, cur_frame.cam_->cam2world(px_cur_), depth))
						return true;
				}
				return false;
			}
			px_cur_ = cur_frame.cam_->world2cam(uv_best);
			if (depthFromTriangulation(T_cur_ref, ref_ftr.f, unproject2d(uv_best).normalized(), depth))
				return true;
		}
		return false;
	}

	bool Matcher::depthFromTriangulation(
		const SE3& T_search_ref,
		const Vector3d& f_ref,
		const Vector3d& f_cur,
		double& depth)
	{
		Matrix<double, 3, 2> A; A << T_search_ref.rotation_matrix() * f_ref, f_cur;
		const Matrix2d AtA = A.transpose()*A;
		if (AtA.determinant() < 0.000001)
			return false;
		const Vector2d depth2 = -AtA.inverse()*A.transpose()*T_search_ref.translation();
		depth = fabs(depth2[0]);
		return true;
	}

}