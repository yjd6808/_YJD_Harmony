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
	SGImage(NpkImage* img) : m_pImage(img) {}
private:
	NpkImage* m_pImage;
	
};
