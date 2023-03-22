/*
 * 작성자: 윤정도
 * 생성일: 3/23/2023 12:43:02 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGWin32Helper.h"

USING_NS_CC;
USING_NS_JC;

static SGSize GetMonitorSizeFromPoint(POINT p);


void SGWin32Helper::LazyInit() {
    Director = Director::getInstance();
    View = dynamic_cast<GLViewImpl*>(Director->getOpenGLView());
    ViewHandle = View->getWin32Window();
}

SGVec2 SGWin32Helper::getWindowPos() {
    RECT rect;
    GetWindowRect(ViewHandle, &rect);
    const SGSize monitorSize = GetMonitorSizeFromPoint({ rect.left, rect.bottom });
    return { (float)rect.left, monitorSize.height - rect.bottom };
}


SGRect SGWin32Helper::getWindowRect() {
    RECT rect;
    GetWindowRect(ViewHandle, &rect);
    const SGSize monitorSize = GetMonitorSizeFromPoint({ rect.left, rect.bottom });
    return { (float)rect.left, monitorSize.height - rect.bottom, (float)rect.right - rect.left,  (float)rect.bottom - rect.top };
}

SGVec2 SGWin32Helper::getCursorPos() {
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    const SGSize monitorSize = GetMonitorSizeFromPoint(cursorPos);
    return { (float)cursorPos.x, monitorSize.height - cursorPos.y };
}


// =====================================

static SGSize GetMonitorSizeFromPoint(POINT p) {
    const HMONITOR hMonitor = MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(hMonitor, &monitorInfo);
    const int width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    const int height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    return { (float)width, (float)height };
}

