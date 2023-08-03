/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Game.h"
#include "GameCoreHeader.h"

USING_NS_JC;

void IntializeScheduler();
void FinalizeScheduler();

void InitializeGameContents() {
	IntializeScheduler();
}

void FinalizeGameContents() {
	FinalizeScheduler();
}


void IntializeScheduler() {
}

void FinalizeScheduler() {
}