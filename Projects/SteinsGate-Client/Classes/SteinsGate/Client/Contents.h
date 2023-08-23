/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 1:09:44 PM
 * =====================
 * 게임과 컨텐츠들 묶음
 */


#pragma once

#include <SteinsGate/Client/UIManager.h>
#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/Inven.h>
#include <SteinsGate/Client/WorldScene.h>
#include <SteinsGate/Client/FontManager.h>
#include <SteinsGate/Client/ActorListenerManager.h>
#include <SteinsGate/Client/ActorManager.h>
#include <SteinsGate/Client/Global.h>
#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/PopupManager.h>
#include <SteinsGate/Client/TimeManager.h>

struct Contents
{
	::Global*				Global;
	::ImagePackManager*		PackManager;
	::FontManager*			FontManager;
	::UIManager*			UIManager;
	::HostPlayer*			Player;
	::Inven*				Inven;
	::WorldScene*			World;
	::ActorListenerManager*	ActorListenerManager;
	::ActorManager*			ActorManager;
	::PopupManager*			PopupManager;
	::TimeManager*			TimeManager;

	void Initialize();
	void Finalize();
};



