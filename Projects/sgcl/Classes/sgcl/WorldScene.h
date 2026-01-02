/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 9:51:36 AM
 * =====================
 *
 */


#pragma once

#include <sgcl/Core.h>

#include <sgcl/SceneBase.h>
#include <sgcl/UILayer.h>

class MimicCamera;
class MapLayer;
class GridLayer;

class WorldScene final : public cc::Scene
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

	void OnWndMessageReceived(int _code, WPARAM _wParam, LPARAM _lParam);
	void OnWndFocused(cc::EventCustom* _pCustom);
	void OnWndLostFocused(cc::EventCustom* _pCustom);
	void OnWndResized(cc::EventCustom* _pCustom);
	void OnWndCursorEnter(cc::EventCustom* _pCustom);
	void OnWndCursorLeave(cc::EventCustom* _pCustom);

	void OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) const;
	void OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) const;
	void OnMouseMove(cc::EventMouse* _pMouseEvent) const;
	void OnMouseDown(cc::EventMouse* _pMouseEvent) const;
	void OnMouseUp(cc::EventMouse* _pMouseEvent) const;
	void OnMouseScroll(cc::EventMouse* _pMouseEvent) const;

	void onExit() override;

	void ReserveScene(SceneType_t _sceneType);
	void ChangeScene(SceneType_t _sceneType);
	void Terminate();

	static SceneBase* CreateScene(SceneType_t _sceneType);

	UILayer&		GetUILayer() const;
	MapLayer*		GetMap();
	MimicCamera*	GetCamera();

private:
	SceneBase*	pRunningScene_;
	SceneType_t reservedScene_;
	UILayer*	pUILayer_ = nullptr;
	GridLayer*	pGridLayer_ = nullptr;
};

#define g_cWorld	(*WorldScene::Get())