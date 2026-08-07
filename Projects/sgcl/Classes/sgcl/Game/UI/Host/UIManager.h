/*
 * 작성자: 윤정도
 * =====================
 * sgui - 윈도우 팩토리/표시 관리자 (구 ViewManager 대체)
 * - 윈도우 클래스를 이름으로 등록(REGISTER_WINDOW)하고 Show()로 생성/표시한다.
 * - WPF 스타일 XML(layout/*.xml) 레이아웃 로딩을 담당한다. (등록된 팩토리가 없으면 XML 로드)
 * - SGA(UI 전용) 텍스처 관리는 제거되었으며, 테마 기반 렌더링만 사용한다.
 */

#pragma once

#include "sgcl/Game/UI/Host/UIHost.h"

#include "jc/Container/DataMap.h"
#include "jc/Primitives/String.h"

#include <functional>
#include <map>

namespace sgui
{

class UIManager final : public jc::SingletonPointer<UIManager>
{
	friend class TSingleton;

public:
	using WindowFactory = std::function<Window*()>;

	void Init();
	void Free();

	// ==================== 윈도우 팩토리 ====================
	void RegisterWindowFactory(const char* _name, const WindowFactory& _factory);

	// 등록된 이름의 윈도우를 표시한다.
	// 1) REGISTER_WINDOW로 등록된 코드 빌드 윈도우 2) WPF 스타일 XML(layout/{name}.xml)
	Window* Show(const char* _name, const jc::CDataMap<>& _dataMap = {}, int _zOrder = 0);

	// XML 레이아웃으로만 윈도우를 표시한다.
	Window* ShowXml(const char* _name, const jc::CDataMap<>& _dataMap = {}, int _zOrder = 0);

	// ==================== 호스트 ====================
	void SetHost(UIHost* _pHost) { pHost_ = _pHost; }
	UIHost* GetHost() const { return pHost_; }

	// ==================== XML 레이아웃 ====================
	bool HasXml(const char* _name) const;
	jc::String ResolveXmlPath(const char* _name) const;

	// ==================== 리소스 ====================
	// SGA 텍스처 관리는 제거되어 더 이상 언로드할 것이 없다. (호환용 빈 구현)
	void UnloadAll() {}

private:
	UIManager();
	~UIManager();

	Window* ShowWindow(Window* _pWindow, const jc::CDataMap<>& _dataMap, int _zOrder);

	UIHost* pHost_ = nullptr;
	std::map<jc::String, WindowFactory> factories_;
};

} // namespace sgui

#define g_cUIMgr (*sgui::UIManager::Get())
