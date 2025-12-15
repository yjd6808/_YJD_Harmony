/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 1:08:09 AM
 * =====================
 *
 */


#pragma once

struct SgaSpriteRect
{
	int x_;
	int y_;
	int width_;
	int height_;
	int frameWidth_;
	int frameHeight_;

	float GetWidthF() const { return (float)width_; }
	float GetHeightF() const { return (float)height_; }
	float GetXF() const { return (float)x_; }
	float GetYF() const { return (float)y_; }
	float GetFrameWidthF() const { return (float)frameWidth_; }
	float GetFrameHeightF() const { return (float)frameHeight_; }
};
