/*
 * 작성자: 윤정도
 * 생성일: 1/9/2023 12:05:21 AM
 * =====================
 *
 */

#include "Core.h"
#include <SteinsGate/Common/NpkElementInitializer.h>
#include <SteinsGate/Common/AudioPlayer.h>

void InitializeCommonCore() {
	NpkElementInitializer::Initialize();
	AudioPlayer::Initilize();
	
}

void FinalizeCommonCore() {
	AudioPlayer::Finalize();
	NpkElementInitializer::Finalize();
}