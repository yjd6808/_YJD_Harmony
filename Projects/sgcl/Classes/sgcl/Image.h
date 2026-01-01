/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 7:38:49 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/Core/Npk/NpkImage.h>


class SGImage
{
public:
	SGImage(NpkImage* _pImage)
		: image_(_pImage)
	{
	}

private:
	NpkImage* image_;
};
