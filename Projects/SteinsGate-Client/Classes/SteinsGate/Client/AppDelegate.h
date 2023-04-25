
#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"

class AppDelegate : private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();
    virtual void initGLContextAttrs();

	void CreateOpenGLWindow();
	void CreateWorldScene();

    HHOOK GetWndProcHook() { return m_hWndProcHook; }
    WNDPROC GetPrevWndProc() { return m_hPrevWndProc; }

    static LRESULT CALLBACK GLFWWindowHookProc(int code, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK GLFWWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void InitializeWindowProcedure();
    /**
    @brief    디렉터 씬 초기화 코드 삽입
    @return true    작업 성공시 앱은 계속 실행된다.
    @return false   앱 종료
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  백그라운드 어플리케이션으로 이동시 호출
    @param  애플리케이션 포인터
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  포그라운드 애플리케이션으로 이동시 호출
    @param  애플리케이션 포인터
    */
    virtual void applicationWillEnterForeground();

private:
    HHOOK m_hWndProcHook;
    WNDPROC m_hPrevWndProc;
};

#endif // _APP_DELEGATE_H_

