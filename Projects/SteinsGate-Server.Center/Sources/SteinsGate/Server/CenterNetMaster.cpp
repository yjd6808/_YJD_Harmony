/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:28 PM
 * =====================
 *
 */

#include "Center.h"
#include "CenterCoreHeader.h"
#include "CenterNetMaster.h"

#include <SteinsGate/Server/CenterNetGroup.h>

USING_NS_JC;
USING_NS_JNET;

CenterNetMaster::CenterNetMaster() {}

CenterNetMaster::~CenterNetMaster() {
}

void CenterNetMaster::Initialize() {
	const auto spCenterNetGroup = MakeShared<CenterNetGroup>();
	AddNetGroup(1, spCenterNetGroup);
	spCenterNetGroup->Initialize();
}

void CenterNetMaster::OnLoop(int sleepMs) {
}

void CenterNetMaster::OnCapturedInputEvent(int inputEvent) {
	CommonNetMaster::OnCapturedInputEvent(inputEvent);
}

void CenterNetMaster::OnStopped() {
}

