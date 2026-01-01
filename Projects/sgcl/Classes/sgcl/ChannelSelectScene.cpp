/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 10:28:18 AM
 * =====================
 *
 */

#include "Core.h"
#include "ChannelSelectScene.h"

#include <sgcl/WorldScene.h>
#include <sgcl/Define_UI.h>
#include <sgcl/S_LOBBY.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
SGChannelSelectScene* SGChannelSelectScene::create()
{
	SGChannelSelectScene* pScene = dbg_new SGChannelSelectScene();

	if (pScene && pScene->init())
	{
		pScene->autorelease();
		return pScene;
	}

	CC_SAFE_DELETE(pScene);
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGChannelSelectScene::SGChannelSelectScene()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SGChannelSelectScene::init()
{
	if (!SceneBase::init())
	{
		return false;
	}

	pUILayer_->AddUIGroup(GROUP_UI_CHANNEL);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGChannelSelectScene::onEnterTransitionDidFinish()
{
	SceneBase::onEnterTransitionDidFinish();
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGChannelSelectScene::update(float _dt)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGChannelSelectScene::OnKeyPressed(c2d::EventKeyboard::KeyCode _keyCode, c2d::Event* _pEvent)
{
	if (_keyCode == EventKeyboard::KeyCode::KEY_F8)
	{
		WorldScene::Get()->ReserveScene(SceneType::Game);
	}
	else if (_keyCode == EventKeyboard::KeyCode::KEY_F7)
	{
		WorldScene::Get()->ReserveScene(SceneType::Login);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGChannelSelectScene::OnKeyReleased(c2d::EventKeyboard::KeyCode _keyCode, c2d::Event* _pEvent)
{
}
