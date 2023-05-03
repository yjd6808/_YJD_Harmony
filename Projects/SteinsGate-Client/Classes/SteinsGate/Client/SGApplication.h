#pragma once

#include "cocos2d.h"

class SGApplication : public cocos2d::Application
{
public:
    SGApplication();
    ~SGApplication() override;

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

public:
    cocos2d::GLViewImpl* View;
    

private:
    HHOOK m_hWndProcHook;
    WNDPROC m_hPrevWndProc;
};

