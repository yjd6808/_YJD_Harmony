/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 4:24:01 AM
 * =====================
 *
 */

#include "Core.h"
#include "GameCore.h"
#include "GameCoreHeader.h"

#include <sgcl/_API/sgapiClient.h>
#include <sgcl/ImagePackManager.h>
#include <sgcl/FontManager.h>

// ===========================================================
//     슈타인즈 게이트 모든 세계션이 만나는 곳
//     사실.. 소괄호 작성하기 귀찮아서 이렇게 모아놓음
// ===========================================================

NS_SG_BEGIN
::SteinsGateApp* App;
::Contents Contents;
::DataManager* DataManager;
::NetCore* Net;
NS_SG_END

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeClientCore()
{
	sg::DataManager = DataManager::Get();
	sg::App = (SteinsGateApp*)cocos2d::Application::getInstance();
	sg::Net = NetCore::Get();

	g_cImagePackMgr.LoadAllPackages();
	g_cFontMgr.Init();
	Global::Get()->init();

	if (sg::DataManager)
	{
		sg::DataManager->LoadAll();
		sg::CharCommon = sg::DataManager->GetCharCommonInfo(1);
		sg::ServerProcessInfoPackage = sg::DataManager->GetServerProcessInfoPackage(1);
	}

	if (sg::Net)
		sg::Net->Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void FinalizeClientCore()
{
	JC_DELETE_SINGLETON_SAFE(sg::Net);

	sg::DataManager->Free();
	
	sg::Contents.Finalize();
	g_cFontMgr.Free();
	g_cImagePackMgr.Free();


	sgapiBase::Free();
}
