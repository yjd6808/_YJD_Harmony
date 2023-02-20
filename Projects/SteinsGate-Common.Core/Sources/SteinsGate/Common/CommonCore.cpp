/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:35:17 AM
 * =====================
 *
 */

#include "Core.h"
#include "CommonCore.h"
#include "CommonCoreHeader.h"

#include <SteinsGate/Common/SgaElementInitializer.h>
#include <SteinsGate/Common/AudioPlayer.h>

// 메인 프로그램에서 주입해줄 것
CommonInfo* CoreCommon_v;

void InitializeCommonCore() {
	SgaElementInitializer::Initialize();
	AudioPlayer::Initilize();
}

void FinalizeCommonCore() {
	AudioPlayer::Finalize();
	SgaElementInitializer::Finalize();
}