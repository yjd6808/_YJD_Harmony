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
	static WorldScene* Get();

	bool init() override;
	void InitEventListeners();
	void InitLayers();

	void update(float _dt) override;
	void UpdateScene(float _dt);
	void UpdateNet(float _dt);
	void UpdateTime(float _dt);
	void UpdateCLI(float _dt);

	void OnWndMessageReceived(int _code, WPARAM _wParam, LPARAM _lParam);
	void OnWndFocused(SGEventCustom* _pCustom);
	void OnWndLostFocused(SGEventCustom* _pCustom);
	void OnWndResized(SGEventCustom* _pCustom);
	void OnWndCursorEnter(SGEventCustom* _pCustom);
	void OnWndCursorLeave(SGEventCustom* _pCustom);

	void OnKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) const;
	void OnKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) const;
	void OnMouseMove(SGEventMouse* _pMouseEvent) const;
	void OnMouseDown(SGEventMouse* _pMouseEvent) const;
	void OnMouseUp(SGEventMouse* _pMouseEvent) const;
	void OnMouseScroll(SGEventMouse* _pMouseEvent) const;

	void onExit() override;

	void ReserveScene(SceneType_t _sceneType);
	void ChangeScene(SceneType_t _sceneType);
	void Terminate();

	static SceneBase* CreateScene(SceneType_t _sceneType);

	UILayer* GetUILayer() const;
	MapLayer* GetMap();
	MimicCamera* GetCamera();

private:
	SceneBase* pRunningScene_;
	SceneType_t reservedScene_;
	UILayer*	pUILayer_ = nullptr;
	GridLayer*	pGridLayer_ = nullptr;
};
