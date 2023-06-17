/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:28 PM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthNetMaster.h"

#include <SteinsGate/Server/AuthNetGroup.h>
#include <SteinsGate/Server/AuthInterServerClientNetGroup.h>

USING_NS_JC;
USING_NS_JNET;

AuthNetMaster::AuthNetMaster() {
}

AuthNetMaster::~AuthNetMaster() {
}

void AuthNetMaster::Initialize() {
	CommonNetMaster::Initialize();

	const auto spAuthNetGroup = MakeShared<AuthNetGroup>();
	const auto spInterServerNetGroup = MakeShared<AuthInterServerClientNetGroup>();

	AddNetGroup(NETGROUP_ID_MAIN,			spAuthNetGroup);
	AddNetGroup(NETGROUP_ID_INTERSERVER,	spInterServerNetGroup);

	spAuthNetGroup->Initialize();
	spInterServerNetGroup->Initialize();
}

void AuthNetMaster::OnLoop(PulserStatistics* pulserStat) {

}

void AuthNetMaster::OnCapturedInputEvent(int inputEvent) {
	CommonNetMaster::OnCapturedInputEvent(inputEvent);
}

void AuthNetMaster::OnStopped() {
}


