/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 10:59:06 AM
 * =====================
 *
 */


#pragma once

union SgaResourceIndex
{
	SgaResourceIndex();

	SgaResourceIndex(int _sgaIndex, int _imgIndex, int _frameIndex)
	: un_{ _sgaIndex, _imgIndex, _frameIndex }
	{
	}

	struct
	{
		int sgaIndex_ : 9; // 512
		int imgIndex_ : 11; // 2048
		int frameIndex_ : 12; // 4096
	} un_;

	jc::String ToString() const;
	Int32U value_;
};
