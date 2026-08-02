/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:27:51 AM
 * =====================
 *
 */


#include "GameCoreHeader.h"
#include "Scene_Login.h"

#include "sgcl/Scene/Scene_World.h"
#include "sgcl/Game/UI/Host/UIManager.h"

USING_NS_CC;
USING_NS_JC;

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

SGLoginScene::SGLoginScene()
: SceneBase()
{
}

bool SGLoginScene::init()
{
	if (!SceneBase::init())
	{
		return false;
	}

	//g_cUIMgr.Show("ui_login");
	g_cUIMgr.Show("ui_theme_test");	// TEMP: 테마/스킴 전환 테스트용
	//g_cUIMgr.Show("ui_channel");
	return true;
}

void SGLoginScene::update(float _dt)
{
}

void SGLoginScene::OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
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

void SGLoginScene::OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
{
}