/*
 * 작성자: 윤정도
 * 생성일: 1/9/2023 12:05:21 AM
 * =====================
 *
 */

#include <SteinsGate/Common/Core.h>
#include <SteinsGate/Common/Core/Npk/NpkElementInitializer.h>
#include <SteinsGate/Common/Core/BassPlayer.h>

void InitializeCommonCore() {
	NpkElementInitializer::Initialize();
	BassPlayer::Initilize();
	
}

void FinalizeCommonCore() {
	BassPlayer::Finalize();
	NpkElementInitializer::Finalize();
}