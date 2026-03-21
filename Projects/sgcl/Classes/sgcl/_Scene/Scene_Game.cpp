/*
 * 작성자 : 윤정도
 *
 * Cocos2d-x 자체는 다 쓰레드 세이프 하지 않음
 *
 */

#include "GameCoreHeader.h"
#include "Scene_Game.h"

#include "sgcl/_Layer/Layer_Grid.h"
#include "sgcl/_Scene/Scene_World.h"

USING_NS_CC;


//////////////////////////////////////////////////////////////////////////////////////////
SGGameScene::SGGameScene()
: SceneBase()
, mapLayer_(nullptr)
, gridLayer_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SGGameScene::~SGGameScene()
{
	CC_SAFE_RELEASE(gridLayer_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGGameScene::OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
{
	if (mapLayer_)
		mapLayer_->onKeyPressed(_keyCode, _pEvent);

	if (_keyCode == EventKeyboard::KeyCode::KEY_F8)
	{
		WorldScene::Get()->ReserveScene(SceneType::Login);
	}
	else if (_keyCode == EventKeyboard::KeyCode::KEY_F7)
	{
		WorldScene::Get()->ReserveScene(SceneType::ChannelSelect);
	}
	else if (_keyCode == EventKeyboard::KeyCode::KEY_F9)
	{
		if (gridLayer_ == nullptr)
			return;

		gridLayer_->setVisible(!gridLayer_->isVisible());
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGGameScene::OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent)
{
	if (mapLayer_)
		mapLayer_->onKeyReleased(_keyCode, _pEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGGameScene::update(float _dt)
{
	if (mapLayer_)
		mapLayer_->update(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SGGameScene::init()
{
	if (!SceneBase::init())
		return false;

	gridLayer_ = GridLayer::create(100, Color4F(Color3B::GREEN, 0.2f), GridLayer::GridEvent::ShowGridAndMousePoint);
	gridLayer_->setAnchorPoint(Vec2::ZERO);
	gridLayer_->setVisible(false);
	gridLayer_->retain();

	mapLayer_ = MapLayer::Create();
	mapLayer_->setAnchorPoint(Vec2::ZERO);
	mapLayer_->addChild(gridLayer_, 1);
	addChild(mapLayer_);

	sg::Contents.ActorManager->Init(mapLayer_);

	CmdEnterMap();
	CmdLoadChar();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGGameScene::CmdLoadChar()
{
	sg::Contents.Player->Initialize();
	sg::Contents.Player->SetPositionRealCenter(300, 200);
	sg::Contents.Player->SetMapLayer(mapLayer_);
	sg::Contents.Player->SetCleanUpFlag(Actor::cfReleaseActorSprite);

	sg::Contents.ActorManager->RegisterPlayerOnMap(sg::Contents.Player);
	mapLayer_->GetCamera()->SetFollowTarget(sg::Contents.Player);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGGameScene::CmdEnterMap()
{
	mapLayer_->LoadMap(10000);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGGameScene::onExit()
{
	sg::Contents.Player->CleanUp();
	ActorManager::Get()->ClearAll();
	SceneBase::onExit();
}
