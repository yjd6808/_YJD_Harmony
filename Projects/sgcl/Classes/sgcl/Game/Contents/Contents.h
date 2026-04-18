/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 1:09:44 PM
 * =====================
 * 게임과 컨텐츠들 묶음
 */


#pragma once

#include "sgcl/Game/Contents/HostPlayer.h"
#include "sgcl/Game/Contents/Inven.h"
#include "sgcl/Game/Contents/ActorListenerManager.h"
#include "sgcl/Game/Contents/ActorManager.h"
#include "sgcl/Game/Contents/PopupManager.h"
#include "sgcl/Game/Contents/TimeManager.h"

struct Contents
{
	::HostPlayer* Player;
	::Inven* Inven;
	::ActorListenerManager* ActorListenerManager;
	::ActorManager* ActorManager;
	::PopupManager* PopupManager;
	::TimeManager* TimeManager;

	void Initialize();
	void Finalize();
};
