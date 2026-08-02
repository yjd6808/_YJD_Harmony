/*
 * 작성자: 윤정도
 * =====================
 * sgui - 윈도우 자동 등록 매크로
 * 윈도우 cpp 파일 하단에 REGISTER_WINDOW(클래스명, "등록이름") 한 줄로 등록한다.
 * 사용: g_cUIMgr.Show("등록이름")
 */

#pragma once

#include "sgcl/Game/UI/Host/UIManager.h"

namespace sgui
{

struct WindowRegistrar
{
	WindowRegistrar(const char* _name, const UIManager::WindowFactory& _factory)
	{
		UIManager::Get()->RegisterWindowFactory(_name, _factory);
	}
};

} // namespace sgui

#define REGISTER_WINDOW(ClassName, Name) \
	static sgui::WindowRegistrar s_windowRegistrar_##ClassName(Name, []() -> sgui::Window* \
	{ \
		ClassName* pWindow = dbg_new ClassName; \
		pWindow->autorelease(); \
		return pWindow; \
	});
