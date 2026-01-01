/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 1:09:44 PM
 * =====================
 * 게임과 컨텐츠들 묶음
 */


#pragma once

#include <sgcl/HostPlayer.h>
#include <sgcl/Inven.h>
#include <sgcl/ActorListenerManager.h>
#include <sgcl/ActorManager.h>
#include <sgcl/Global.h>
#include <sgcl/PopupManager.h>
#include <sgcl/TimeManager.h>

struct Contents
{
	::Global* Global;
	::HostPlayer* Player;
	::Inven* Inven;
	::ActorListenerManager* ActorListenerManager;
	::ActorManager* ActorManager;
	::PopupManager* PopupManager;
	::TimeManager* TimeManager;

	void Initialize();
	void Finalize();
};
