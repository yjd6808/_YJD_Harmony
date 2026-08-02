/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Host/UIHost.h"
#include "sgcl/Game/UI/Host/UIManager.h"
#include "sgcl/Game/UI/Core/InputDispatcher.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"

#include <vector>

USING_NS_CC;

namespace sgui
{

UIHost* UIHost::Create()
{
	UIHost* pHost = dbg_new UIHost;

	if (!pHost->init())
	{
		delete pHost;
		return nullptr;
	}

	pHost->autorelease();
	return pHost;
}

UIHost::~UIHost()
{
	_LogDebug_("[UIHost] ~UIHost winCount=%d running=%d", (int)windows_.size(), isRunning() ? 1 : 0);
	if (InputDispatcher::Get()->GetHost() == this)
	{
		InputDispatcher::Get()->SetHost(nullptr);
		InputDispatcher::Get()->Clear();
	}
}

bool UIHost::init()
{
	if (!Layer::init())
	{
		return false;
	}

	InputDispatcher::Get()->SetHost(this);
	lastThemeTextureRevision_ = UIThemeManager::Get()->GetRevision().textureRevision;
	scheduleUpdate();
	return true;
}

// ==================== 윈도우 관리 ====================

void UIHost::AddWindow(Window* _pWindow, int _zOrder /* = 0 */)
{
	jc_assert_msg(_pWindow != nullptr, "null 윈도우를 추가할 수 없습니다.");

	_pWindow->pHost_ = this;
	windows_.push_back(_pWindow);
	addChild(_pWindow, _zOrder);
	_LogDebug_("[UIHost] AddWindow %s rc=%d running=%d", _pWindow->getName().c_str(), _pWindow->getReferenceCount(), _pWindow->isRunning() ? 1 : 0);

	_pWindow->MarkLayoutDirty();
	_pWindow->FlushLayout(getContentSize());
}

void UIHost::RemoveWindow(Window* _pWindow)
{
	if (_pWindow == nullptr)
	{
		return;
	}

	auto it = std::find(windows_.begin(), windows_.end(), _pWindow);

	if (it == windows_.end())
	{
		return;
	}

	windows_.erase(it);

	_pWindow->OnRemoved();
	_pWindow->OnClosed();
	_pWindow->pHost_ = nullptr;
	_LogDebug_("[UIHost] RemoveWindow %s rc=%d running=%d hostRunning=%d", _pWindow->getName().c_str(), _pWindow->getReferenceCount(), _pWindow->isRunning() ? 1 : 0, isRunning() ? 1 : 0);
	_pWindow->removeFromParent();
}

Window* UIHost::FindWindow(const char* _name) const
{
	if (_name == nullptr)
	{
		return nullptr;
	}

	for (Window* pWindow : windows_)
	{
		if (pWindow->getName() == _name)
		{
			return pWindow;
		}
	}

	return nullptr;
}

Window* UIHost::GetWindowAt(int _index) const
{
	if (_index < 0 || _index >= (int)windows_.size())
	{
		return nullptr;
	}

	return windows_[_index];
}

Window* UIHost::GetTopmostWindow() const
{
	if (windows_.empty())
	{
		return nullptr;
	}

	Window* pTopmost = windows_.front();

	for (Window* pWindow : windows_)
	{
		if (pWindow->getLocalZOrder() >= pTopmost->getLocalZOrder())
		{
			pTopmost = pWindow;
		}
	}

	return pTopmost;
}

void UIHost::Clear()
{
	_LogDebug_("[UIHost] Clear winCount=%d", (int)windows_.size());
	while (!windows_.empty())
	{
		RemoveWindow(windows_.back());
	}

	InputDispatcher::Get()->Clear();
}

void UIHost::ClearUnload()
{
	Clear();
	g_cUIMgr.UnloadAll();
}

// ==================== 입력 ====================

bool UIHost::OnMouseDown(cc::EventMouse* _pEvent)
{
	return InputDispatcher::Get()->DispatchMouseDown(_pEvent->getCursorPos());
}

bool UIHost::OnMouseUp(cc::EventMouse* _pEvent)
{
	return InputDispatcher::Get()->DispatchMouseUp(_pEvent->getCursorPos());
}

bool UIHost::OnMouseMove(cc::EventMouse* _pEvent)
{
	return InputDispatcher::Get()->DispatchMouseMove(_pEvent->getCursorPos());
}

bool UIHost::OnMouseScroll(cc::EventMouse* _pEvent)
{
	return InputDispatcher::Get()->DispatchMouseWheel(_pEvent->getCursorPos(), _pEvent->getScrollX(), _pEvent->getScrollY());
}

bool UIHost::OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode)
{
	return InputDispatcher::Get()->DispatchKeyDown(_keyCode);
}

bool UIHost::OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode)
{
	return InputDispatcher::Get()->DispatchKeyUp(_keyCode);
}

// ==================== 프레임 갱신 ====================

void UIHost::update(float _delta)
{
	// 테마 비동기 베이크(디바운스/실행/완료 처리/텍스처 교체)를 프레임마다 구동한다.
	UIThemeManager::Get()->Update(_delta);

	PollThemeRevision();
	FlushDirtyLayouts();

	// 윈도우 갱신 (순회 중 추가/제거 대비 스냅샷)
	std::vector<Window*> snapshot = windows_;

	for (Window* pWindow : snapshot)
	{
		if (std::find(windows_.begin(), windows_.end(), pWindow) != windows_.end())
		{
			pWindow->OnUpdate(_delta);
		}
	}

	FlushCloseRequests();
}

void UIHost::FlushDirtyLayouts()
{
	const cc::size hostSize = getContentSize();

	for (Window* pWindow : windows_)
	{
		if (pWindow->IsLayoutDirty())
		{
			pWindow->FlushLayout(hostSize);
		}
	}
}

void UIHost::FlushCloseRequests()
{
	for (int idx = (int)windows_.size() - 1; idx >= 0; --idx)
	{
		if (windows_[idx]->IsCloseRequested())
		{
			RemoveWindow(windows_[idx]);
		}
	}
}

void UIHost::PollThemeRevision()
{
	const uint64_t textureRevision = UIThemeManager::Get()->GetRevision().textureRevision;

	if (textureRevision == lastThemeTextureRevision_)
	{
		return;
	}

	lastThemeTextureRevision_ = textureRevision;

	// 테마 텍스처가 교체되었으므로 모든 윈도우의 테마 비주얼을 갱신한다.
	for (Window* pWindow : windows_)
	{
		pWindow->RefreshThemeVisuals();
		pWindow->MarkLayoutDirty();
	}
}

} // namespace sgui
