/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 9:51:36 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include "Struct.h"

#include <SteinsGate/Client/SceneBase.h>
#include <SteinsGate/Client/UILayer.h>

class MimicCamera;
class MapLayer;
class WorldScene : public SGScene
{
private:
	WorldScene();
public:
	~WorldScene() override;

	bool init() override;
	void initEventListeners();
	void InitUILayer();
	

	void update(float dt) override;
	void updateScene(float dt);

	void onWndMessageReceived(int code, WPARAM wParam, LPARAM lParam);
	void onWndFocused(SGEventCustom* custom);
	void onWndLostFocused(SGEventCustom* custom);
	void onWndResized(SGEventCustom* custom);
	void onWndCursorEnter(SGEventCustom* custom);
	void onWndCursorLeave(SGEventCustom* custom);

	void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) const;
	void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) const;
	void onMouseMove(SGEventMouse* mouseEvent) const;
	void onMouseDown(SGEventMouse* mouseEvent) const;
	void onMouseUp(SGEventMouse* mouseEvent) const;
	void onMouseScroll(SGEventMouse* mouseEvent) const;
	
	void onExit() override;

	void reserveScene(SceneType_t sceneType);
	void changeScene(SceneType_t sceneType);
	static SceneBase* createScene(SceneType_t sceneType);

	UILayer* getUILayer() const;

	static WorldScene* get();
	
	MapLayer* getMap();
	MimicCamera* getCamera();
private:
	SceneBase* m_pRunningScene;
	SceneType_t m_eReservedScene;
	UILayer* m_pUILayer;
};

