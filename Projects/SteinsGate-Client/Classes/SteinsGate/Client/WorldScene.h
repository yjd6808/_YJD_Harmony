/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 9:51:36 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>

#include <SteinsGate/Client/SceneBase.h>

#include <SteinsGate/Client/UILayer.h>
#include <SteinsGate/Common/GridLayer.h>

class MimicCamera;
class MapLayer;
class WorldScene final : public SGScene
{
private:
	friend class TSingleton;
	WorldScene();
	~WorldScene() override;
public:
	static WorldScene* get();

	bool init() override;
	void initEventListeners();
	void InitLayers();
	
	void update(float dt) override;
	void updateScene(float dt);
	void updateNet(float dt);
	void updateTime(float dt);
	void updateCLI(float dt);

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
	void terminate();

	static SceneBase* createScene(SceneType_t sceneType);

	UILayer* getUILayer() const;
	MapLayer* getMap();
	MimicCamera* getCamera();
private:
	SceneBase* m_pRunningScene;
	SceneType_t m_eReservedScene;
	UILayer* m_pUILayer;
	GridLayer* m_pGridLayer;
};

