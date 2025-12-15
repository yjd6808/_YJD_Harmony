/*
 * 작성자 : 윤정도
 *
 * Cocos2d-x 자체는 다 쓰레드 세이프 하지 않음
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "GameScene.h"


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
void SGGameScene::onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent)
{
	if (mapLayer_)
		mapLayer_->onKeyPressed(_keyCode, _pEvent);

	if (_keyCode == EventKeyboard::KeyCode::KEY_F8)
	{
		WorldScene::get()->reserveScene(SceneType::Login);
	}
	else if (_keyCode == EventKeyboard::KeyCode::KEY_F7)
	{
		WorldScene::get()->reserveScene(SceneType::ChannelSelect);
	}
	else if (_keyCode == EventKeyboard::KeyCode::KEY_F9)
	{
		if (gridLayer_ == nullptr)
			return;

		gridLayer_->setVisible(!gridLayer_->isVisible());
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGGameScene::onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent)
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

	mapLayer_ = MapLayer::create();
	mapLayer_->setAnchorPoint(Vec2::ZERO);
	mapLayer_->addChild(gridLayer_, 1);
	addChild(mapLayer_);

	Core::Contents.ActorManager->init(mapLayer_);

	cmdEnterMap();
	cmdLoadChar();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGGameScene::cmdLoadChar()
{
	Core::Contents.Player->initialize();
	Core::Contents.Player->setPositionRealCenter(300, 200);
	Core::Contents.Player->setMapLayer(mapLayer_);
	Core::Contents.Player->setCleanUpFlag(Actor::CF_ReleaseActorSprite);

	Core::Contents.ActorManager->registerPlayerOnMap(Core::Contents.Player);
	mapLayer_->getCamera()->setFollowTarget(Core::Contents.Player);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGGameScene::cmdEnterMap()
{
	mapLayer_->loadMap(10000);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGGameScene::onExit()
{
	Core::Contents.Player->cleanUp();
	ActorManager::Get()->clearAll();
	SceneBase::onExit();
}
