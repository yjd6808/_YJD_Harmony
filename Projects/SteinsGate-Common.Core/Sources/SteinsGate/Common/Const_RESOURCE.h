/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 12:58:37 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/Namepsace.h>

NS_CONST_BEGIN

namespace Resource
{
	constexpr const char* ImageDirName = "image";
	constexpr const char* SoundDirName = "sound";
	constexpr const char* FontDirName = "font";

	constexpr int MaxSgaFileCount = 200;
	constexpr int MaxSgaParallelLoadingThreadCount = 8;
}

NS_CONST_END