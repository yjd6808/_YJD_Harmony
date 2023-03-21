/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 9:51:36 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include "SGStruct.h"

#include <SteinsGate/Client/SGSceneBase.h>
#include <SteinsGate/Client/SGUILayer.h>

class SGCamera;
class SGMapLayer;
class SGWorldScene : public SGScene
{
private:
	SGWorldScene();
public:
	~SGWorldScene() override;

	bool init() override;
	void initEventListeners();
	void InitUILayer();
	

	void update(float dt) override;
	void updateScene(float dt);

	void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) const;
	void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) const;
	void onMouseMove(SGEventMouse* mouseEvent) const;
	void onMouseDown(SGEventMouse* mouseEvent) const;
	void onMouseUp(SGEventMouse* mouseEvent) const;
	void onMouseScroll(SGEventMouse* mouseEvent) const;
	void onExit() override;

	void reserveScene(SceneType_t sceneType);
	void changeScene(SceneType_t sceneType);
	static SGSceneBase* createScene(SceneType_t sceneType);

	SGUILayer* getUILayer() const;

	static SGWorldScene* get();
	
	SGMapLayer* getMap();
	SGCamera* getCamera();
private:
	SGEventListenerKeyboard* m_pKeyboardListener;
	SGEventListenerMouse* m_pMouseListener;

	SGSceneBase* m_pRunningScene;
	SceneType_t m_eReservedScene;
	SGUILayer* m_pUILayer;
};

