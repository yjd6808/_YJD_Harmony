/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:27:51 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "LoginScene.h"

#include <SteinsGate/Client/WorldScene.h>
#include <SteinsGate/Client/UILayer.h>
#include <SteinsGate/Client/Define_UI.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
SGLoginScene* SGLoginScene::Create()
{
	SGLoginScene* pScene = dbg_new SGLoginScene();
	if (pScene && pScene->init())
	{
		pScene->autorelease();
		return pScene;
	}

	CC_SAFE_DELETE(pScene);
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGLoginScene::SGLoginScene()
: SceneBase()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SGLoginScene::init()
{
	if (!SceneBase::init())
	{
		return false;
	}

	pUILayer_->AddUIGroup(GROUP_UI_LOGIN);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGLoginScene::update(float _dt)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGLoginScene::OnKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent)
{
	if (_keyCode == EventKeyboard::KeyCode::KEY_F8)
	{
		WorldScene::Get()->ReserveScene(SceneType::ChannelSelect);
	}
	else if (_keyCode == EventKeyboard::KeyCode::KEY_F7)
	{
		WorldScene::Get()->ReserveScene(SceneType::Game);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGLoginScene::OnKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent)
{
}
