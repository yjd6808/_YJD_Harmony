/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:40:00 AM
 * 수정일: 8/9/2026 10:00:00 AM (v2.1: 윈도우별 씬 슬롯 - 멀티 윈도우 지원)
 * =====================
 * 디렉터 (씬 관리자)
 *
 * [디렉터란?]
 *  Cocos2d-x의 cocos2d::Director와 같은 역할.
 *  "지금 어떤 씬을 보여줄지"를 관리하는 유일한 관리자다.
 *  씬 전환 요청이 오면 프레임 경계에서 안전하게 교체한다.
 *  (프레임 도중 씬을 바꾸면 그리던 씬이 사라져 크래시 위험이 있다)
 *
 * [v2.1 변경점 - 윈도우별 씬 슬롯]
 *  애플리케이션이 여러 윈도우를 가질 수 있게 되면서,
 *  디렉터는 "윈도우마다 씬 하나"를 슬롯(SceneSlot)으로 관리한다.
 *   - RunScene(pScene)            : 메인 윈도우에 씬 시작 (기존과 동일)
 *   - RunScene(pScene, pWindow)   : 특정 윈도우에 씬 시작
 *  씬이 슬롯에 올라갈 때 Scene::SetWindow로 소속 윈도우를 알려준다.
 *  (그래서 씬은 GetWindow()로 자신이 그려지는 창을 언제든 알 수 있다)
 *
 * [사용법 - g_c 전역 접근 (sgcl 스타일)]
 *  g_cDirector.RunScene(new MyScene());                  // 메인 창 씬 시작
 *  g_cDirector.ReplaceScene(new NextScene());            // 메인 창 씬 교체
 *  Window* pSub = g_cApp.CreateSubWindow(L"미니맵", 400, 300);
 *  g_cDirector.RunScene(new MiniMapScene(), pSub);       // 서브 창 씬 시작
 */

#pragma once

#include "jc/Time.h"

#include <jc/Pattern/Singleton.h>
#include <jc/Container/Vector.h>

NS_SGF_BEGIN

class Scene;
class Window;

class Director : public jc::SingletonStatic<Director>
{
	friend class jc::SingletonStatic<Director>;	// Get()이 생성자에 접근할 수 있도록

private:
	Director();
	~Director();

public:
	// 지정 윈도우에 첫 씬을 시작한다. 이미 씬이 있으면 ReplaceScene과 동일.
	// @param _pScene  : 실행할 씬 (소유권이 Director로 넘어온다)
	// @param _pWindow : 씬을 그릴 윈도우. nullptr이면 메인 윈도우.
	void RunScene(Scene* _pScene, Window* _pWindow = nullptr);

	// 다음 프레임 경계에서 해당 윈도우의 씬을 교체한다.
	// 이전 씬은 OnExit 호출 후 delete된다.
	void ReplaceScene(Scene* _pScene, Window* _pWindow = nullptr);

	// 해당 윈도우에서 실행 중인 씬 (없으면 nullptr)
	Scene* CurrentScene(Window* _pWindow = nullptr);

	// === Application이 매 프레임 부르는 내부 함수 ===

	// 모든 슬롯의 예약된 씬 교체를 처리하고 각 씬을 갱신한다.
	void Update(const jc::TimeSpan& _dt);

	// 지정 윈도우의 씬을 그린다.
	// [v2.1] 씬 카메라의 ViewProjection으로 3D/2D 배치를 열고(Begin)
	//  Scene::OnRender()를 부른 뒤 3D -> 2D 순서로 닫는다(End).
	//  (3D가 바닥, 2D가 그 위에 얹혀 UI처럼 보이도록)
	void Render(Window* _pWindow);

	// 창이 닫힐 때 해당 슬롯의 씬을 정리한다. (Application이 호출)
	void DetachWindow(Window* _pWindow);

	// 모든 씬 정리 (앱 종료 시 Application이 호출)
	void Cleanup();

private:
	// 윈도우 하나당 씬 하나. (현재 씬 + 교체 예약 씬)
	struct SceneSlot
	{
		Window* pWindow_;	// 이 슬롯의 윈도우 (빌림. nullptr이면 빈 슬롯)
		Scene* pCurrent_;	// 현재 실행 중인 씬 (소유함)
		Scene* pNext_;		// 다음 프레임에 교체될 씬 (소유함)
	};

	// nullptr이면 메인 윈도우로 바꿔준다.
	Window* ResolveWindow(Window* _pWindow) const;

	// 해당 윈도우의 슬롯을 찾는다. (없으면 nullptr)
	SceneSlot* FindSlot(Window* _pWindow);

	// 해당 윈도우의 슬롯을 찾거나, 빈 슬롯을 재활용하거나, 새로 만든다.
	SceneSlot& GetOrCreateSlot(Window* _pWindow);

private:
	jc::Vector<SceneSlot> slots_;	// 윈도우별 씬 슬롯들
};

NS_SGF_END

// sgcl 스타일 전역 접근 매크로: g_cDirector.RunScene(...) 처럼 사용
#define g_cDirector (*sgf::Director::Get())
