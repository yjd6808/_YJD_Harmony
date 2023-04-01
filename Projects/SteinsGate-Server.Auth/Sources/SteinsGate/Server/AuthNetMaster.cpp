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

USING_NS_JC;
USING_NS_JNET;

AuthNetMaster::AuthNetMaster() {}

void AuthNetMaster::Initialize() {
	const auto spAuthNetGroup = MakeShared<AuthNetGroup>();
	AddNetGroup(1, spAuthNetGroup);
	spAuthNetGroup->Initialize();
}

void AuthNetMaster::MainLoop() {
	constexpr int PulseInterval = 100;
	Pulser pulser(PulseInterval);

	while (true) {
		int iPulseIntervalCount = pulser.Wait();
		
	}
}
