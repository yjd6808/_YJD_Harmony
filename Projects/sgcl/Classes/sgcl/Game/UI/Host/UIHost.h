/*
 * 작성자: 윤정도
 * =====================
 * sgui - 윈도우 컨테이너 레이어 (구 Layer_UI 대체)
 * 씨(Scene)에 부착되어 윈도우 관리 / 입력 전달 / 레이아웃 갱신 / 테마 갱신을 담당한다.
 */

#pragma once

#include "sgcl/Game/UI/Controls/Window.h"

namespace sgui
{

class UIHost : public cc::Layer
{
public:
	static UIHost* Create();
	virtual ~UIHost() override;

	// ==================== 윈도우 관리 ====================
	void AddWindow(Window* _pWindow, int _zOrder = 0);
	void RemoveWindow(Window* _pWindow);
	Window* FindWindow(const char* _name) const;

	int GetWindowCount() const { return (int)windows_.size(); }
	Window* GetWindowAt(int _index) const;
	Window* GetTopmostWindow() const;

	void Clear();			// 모든 윈도우 제거
	void ClearUnload();		// 모든 윈도우 제거 + UI 텍스처 언로드

	// ==================== 입력 (Scene에서 호출) ====================
	bool OnMouseDown(cc::EventMouse* _pEvent);
	bool OnMouseUp(cc::EventMouse* _pEvent);
	bool OnMouseMove(cc::EventMouse* _pEvent);
	bool OnMouseScroll(cc::EventMouse* _pEvent);
	bool OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode);
	bool OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode);

	// ==================== 프레임 갱신 ====================
	virtual void update(float _delta) override;

protected:
	virtual bool init() override;

	void FlushDirtyLayouts();
	void FlushCloseRequests();
	void PollThemeRevision();

	std::vector<Window*> windows_;
	uint64_t lastThemeTextureRevision_ = 0;
};

} // namespace sgui
