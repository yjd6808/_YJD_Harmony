#pragma once

#include "cocos2d.h"

class SteinsGateApp : public cocos2d::Application
{
public:
	SteinsGateApp();
	~SteinsGateApp() override;

	void initGLContextAttrs() override;

	void SetDesignResolutionSize(float _width, float _height);

	void SetDesignResolutionSize(const cocos2d::Size& _resolutionSize)
	{
		SetDesignResolutionSize(_resolutionSize.width, _resolutionSize.height);
	}

	void SetFrameSize(float _width, float _height);
	void SetFrameSize(const cocos2d::Size& _frameSize) { SetFrameSize(_frameSize.width, _frameSize.height); }

	void CreateOpenGLWindow();
	void CreateWorldScene();

	HHOOK GetWndProcHook() { return pWndProcHook_; }
	WNDPROC GetPrevWndProc() { return pPrevWndProc_; }

	static LRESULT CALLBACK GLFWWindowHookProc(int _code, WPARAM _wParam, LPARAM _lParam);
	static LRESULT CALLBACK GLFWWindowProc(HWND _pHwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam);

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
	HHOOK pWndProcHook_;
	WNDPROC pPrevWndProc_;
};
