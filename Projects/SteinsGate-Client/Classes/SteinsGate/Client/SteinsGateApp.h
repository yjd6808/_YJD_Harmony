#pragma once

#include "cocos2d.h"

class SteinsGateApp : public cocos2d::Application
{
public:
    SteinsGateApp();
    ~SteinsGateApp() override;

    void initGLContextAttrs() override;

    void SetDesignResolutionSize(float width, float height);
    void SetDesignResolutionSize(const cocos2d::Size& resolutionSize) { SetDesignResolutionSize(resolutionSize.width, resolutionSize.height); }
    void SetFrameSize(float width, float height);
    void SetFrameSize(const cocos2d::Size& frameSize) { SetFrameSize(frameSize.width, frameSize.height); }


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
    bool applicationDidFinishLaunching() override;

    /**
    @brief  백그라운드 어플리케이션으로 이동시 호출
    @param  애플리케이션 포인터
    */
    void applicationDidEnterBackground() override;

    /**
    @brief  포그라운드 애플리케이션으로 이동시 호출
    @param  애플리케이션 포인터
    */
    void applicationWillEnterForeground() override;

    /**
     * \brief 앱 종료시 호출
     */
    void applicationDidExit() override;

public:
    cocos2d::GLViewImpl* View;
    

private:
    HHOOK m_hWndProcHook;
    WNDPROC m_hPrevWndProc;
};

