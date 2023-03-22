
#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"

class AppDelegate : private cocos2d::Application
{
public:
    HHOOK m_hWndProcHook;

    AppDelegate();
    virtual ~AppDelegate();

    virtual void initGLContextAttrs();

	void CreateOpenGLWindow();
	void CreateWorldScene();


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
};

#endif // _APP_DELEGATE_H_

