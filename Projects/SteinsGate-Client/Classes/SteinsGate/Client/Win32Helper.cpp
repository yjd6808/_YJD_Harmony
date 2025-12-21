/*
 * 작성자: 윤정도
 * 생성일: 3/23/2023 12:43:02 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "Win32Helper.h"

USING_NS_CC;
USING_NS_JC;

static SGSize GetMonitorSizeFromPoint(POINT _point);

//////////////////////////////////////////////////////////////////////////////////////////
void Win32Helper::LazyInit()
{
	Director = Director::getInstance();
	View = dynamic_cast<GLViewImpl*>(Director->getOpenGLView());
	ViewHandle = View->getWin32Window();
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 Win32Helper::GetWindowPos()
{
	RECT rect;
	::GetWindowRect(ViewHandle, &rect);
	const SGSize monitorSize = GetMonitorSizeFromPoint({ rect.left, rect.bottom });
	return { (float)rect.left, monitorSize.height - rect.bottom };
}

//////////////////////////////////////////////////////////////////////////////////////////
SGRect Win32Helper::GetWindowRect()
{
	RECT rect;
	::GetWindowRect(ViewHandle, &rect);
	const SGSize monitorSize = GetMonitorSizeFromPoint({ rect.left, rect.bottom });
	return {
		(float)rect.left, monitorSize.height - rect.bottom, (float)rect.right - rect.left, (float)rect.bottom - rect.top
	};
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 Win32Helper::GetCursorPos()
{
	POINT cursorPos;
	::GetCursorPos(&cursorPos);
	const SGSize monitorSize = GetMonitorSizeFromPoint(cursorPos);
	return { (float)cursorPos.x, monitorSize.height - cursorPos.y };
}

//////////////////////////////////////////////////////////////////////////////////////////
static SGSize GetMonitorSizeFromPoint(POINT _point)
{
	const HMONITOR pMonitor = MonitorFromPoint(_point, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(pMonitor, &monitorInfo);
	const int width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
	const int height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
	return { (float)width, (float)height };
}
