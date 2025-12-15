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

	void update(float _dt) override;
	void updateScene(float _dt);
	void updateNet(float _dt);
	void updateTime(float _dt);
	void updateCLI(float _dt);

	void onWndMessageReceived(int _code, WPARAM _wParam, LPARAM _lParam);
	void onWndFocused(SGEventCustom* _pCustom);
	void onWndLostFocused(SGEventCustom* _pCustom);
	void onWndResized(SGEventCustom* _pCustom);
	void onWndCursorEnter(SGEventCustom* _pCustom);
	void onWndCursorLeave(SGEventCustom* _pCustom);

	void onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) const;
	void onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) const;
	void onMouseMove(SGEventMouse* _pMouseEvent) const;
	void onMouseDown(SGEventMouse* _pMouseEvent) const;
	void onMouseUp(SGEventMouse* _pMouseEvent) const;
	void onMouseScroll(SGEventMouse* _pMouseEvent) const;

	void onExit() override;

	void reserveScene(SceneType_t _sceneType);
	void changeScene(SceneType_t _sceneType);
	void terminate();

	static SceneBase* createScene(SceneType_t _sceneType);

	UILayer* getUILayer() const;
	MapLayer* getMap();
	MimicCamera* getCamera();

private:
	SceneBase* runningScene_;
	SceneType_t reservedScene_;
	UILayer* uiLayer_;
	GridLayer* gridLayer_;
};
