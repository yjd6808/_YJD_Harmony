/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:42:00 AM
 * 수정일: 8/9/2026 10:05:00 AM (v2.1: 윈도우별 씬 슬롯)
 * =====================
 * 디렉터 구현부
 */

#include "Core.h"
#include "sgf/Scene/Director.h"
#include "sgf/Scene/Scene.h"
#include "sgf/Core/Application.h"	// g_cWindow / g_cRenderer2D / g_cRenderer3D 매크로

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
Director::Director()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Director::~Director()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// nullptr이면 메인 윈도우로 바꿔준다.
Window* Director::ResolveWindow(Window* _pWindow) const
{
	if (_pWindow != nullptr)
	{
		return _pWindow;
	}

	// 인자를 생략하면 "메인 윈도우"를 뜻한다. (기존 단일 윈도우 코드와 호환)
	return &g_cWindow;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 해당 윈도우의 슬롯을 찾는다. (없으면 nullptr)
Director::SceneSlot* Director::FindSlot(Window* _pWindow)
{
	for (int i = 0; i < slots_.Size(); ++i)
	{
		if (slots_[i].pWindow_ == _pWindow)
		{
			return &slots_[i];
		}
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 해당 윈도우의 슬롯을 찾거나, 빈 슬롯(닫힌 창 자리)을 재활용하거나, 새로 만든다.
Director::SceneSlot& Director::GetOrCreateSlot(Window* _pWindow)
{
	// 1) 이미 있으면 그대로 사용
	SceneSlot* pFound = FindSlot(_pWindow);
	if (pFound != nullptr)
	{
		return *pFound;
	}

	// 2) 빈 슬롯(pWindow_ == nullptr)이 있으면 재활용
	for (int i = 0; i < slots_.Size(); ++i)
	{
		if (slots_[i].pWindow_ == nullptr)
		{
			slots_[i].pWindow_ = _pWindow;
			slots_[i].pCurrent_ = nullptr;
			slots_[i].pNext_ = nullptr;
			return slots_[i];
		}
	}

	// 3) 없으면 새 슬롯 추가
	SceneSlot slot;
	slot.pWindow_ = _pWindow;
	slot.pCurrent_ = nullptr;
	slot.pNext_ = nullptr;
	slots_.PushBack(slot);
	return slots_[slots_.Size() - 1];
}

//////////////////////////////////////////////////////////////////////////////////////////
// 지정 윈도우에 첫 씬을 시작한다.
void Director::RunScene(Scene* _pScene, Window* _pWindow)
{
	if (_pScene == nullptr)
	{
		return;
	}

	Window* pWindow = ResolveWindow(_pWindow);
	SceneSlot& slot = GetOrCreateSlot(pWindow);

	// 이미 씬이 돌고 있거나 교체가 예약되어 있으면 교체 요청과 동일하다.
	if (slot.pCurrent_ != nullptr || slot.pNext_ != nullptr)
	{
		ReplaceScene(_pScene, pWindow);
		return;
	}

	// 첫 씬은 즉시 시작한다. (아직 아무것도 그리고 있지 않으므로 안전)
	slot.pCurrent_ = _pScene;
	_pScene->SetWindow(pWindow);	// 씬에게 소속 윈도우를 알려준다 (v2.1)
	_pScene->OnEnter();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 다음 프레임 경계에서 해당 윈도우의 씬을 교체한다.
void Director::ReplaceScene(Scene* _pScene, Window* _pWindow)
{
	if (_pScene == nullptr)
	{
		return;
	}

	Window* pWindow = ResolveWindow(_pWindow);
	SceneSlot& slot = GetOrCreateSlot(pWindow);

	// 같은 씬으로 교체는 무의미하다.
	if (slot.pCurrent_ == _pScene || slot.pNext_ == _pScene)
	{
		return;
	}

	// 이미 예약된 씬이 있었다면 버린다. (한 번도 실행되지 않았으므로 OnExit 없이 삭제)
	if (slot.pNext_ != nullptr)
	{
		JC_DELETE_SAFE(slot.pNext_);
	}

	// 아직 씬이 없는 슬롯이면 즉시 시작한다.
	if (slot.pCurrent_ == nullptr)
	{
		slot.pCurrent_ = _pScene;
		_pScene->SetWindow(pWindow);
		_pScene->OnEnter();
		return;
	}

	// 프레임 도중 교체는 위험하므로 다음 Update 시작 시점에 교체한다.
	slot.pNext_ = _pScene;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 해당 윈도우에서 실행 중인 씬을 반환한다. (없으면 nullptr)
Scene* Director::CurrentScene(Window* _pWindow)
{
	SceneSlot* pSlot = FindSlot(ResolveWindow(_pWindow));
	return (pSlot != nullptr) ? pSlot->pCurrent_ : nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 매 프레임 갱신 (Application이 호출)
// 1) 프레임 경계에서 예약된 씬 교체를 먼저 처리하고
// 2) 모든 윈도우의 현재 씬을 갱신한다.
void Director::Update(const jc::TimeSpan& _dt)
{
	// 1. 예약된 씬 교체 처리 (프레임 경계이므로 안전하다)
	//    씬 콜백(OnExit/OnEnter/OnUpdate)이 slots_를 변경할 수 있으므로(재진입)
	//    참조를 오래 붙잡지 않고 매 접근마다 인덱스로 다시 조회한다.
	for (int i = 0; i < slots_.Size(); ++i)
	{
		if (slots_[i].pWindow_ == nullptr || slots_[i].pNext_ == nullptr)
		{
			continue;
		}

		// 이전 씬을 내리고 (OnExit 중 씬 교체 가능 → 이후 슬롯 재조회)
		if (slots_[i].pCurrent_ != nullptr)
		{
			slots_[i].pCurrent_->OnExit();
			JC_DELETE_SAFE(slots_[i].pCurrent_);
		}

		// OnExit 도중 예약이 바뀌었을 수 있으니 현재 예약 상태를 다시 읽는다.
		if (slots_[i].pNext_ == nullptr)
		{
			continue;	// (이론적) 교체가 취소된 경우
		}

		// 새 씬을 올린다.
		slots_[i].pCurrent_ = slots_[i].pNext_;
		slots_[i].pNext_ = nullptr;
		slots_[i].pCurrent_->SetWindow(slots_[i].pWindow_);
		slots_[i].pCurrent_->OnEnter();
	}

	// 2. 모든 윈도우의 현재 씬 갱신
	for (int i = 0; i < slots_.Size(); ++i)
	{
		if (slots_[i].pWindow_ != nullptr && slots_[i].pCurrent_ != nullptr)
		{
			slots_[i].pCurrent_->OnUpdate(_dt);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 지정 윈도우의 씬을 그린다. (Application이 창마다 BeginFrame/EndFrame 사이에 호출)
// [v2.1] 씬 카메라의 행렬로 3D/2D 배치를 모두 열어두고 OnRender를 부른다.
//  씬은 g_cRenderer2D / g_cRenderer3D로 자유롭게 섞어 그리면 된다.
//  End 순서는 3D -> 2D. (3D가 먼저 그려져 바닥이 되고, 2D가 UI처럼 그 위에 얹힌다)
void Director::Render(Window* _pWindow)
{
	SceneSlot* pSlot = FindSlot(ResolveWindow(_pWindow));
	if (pSlot == nullptr || pSlot->pCurrent_ == nullptr)
	{
		return;
	}

	Scene* pScene = pSlot->pCurrent_;
	const mat4 viewProjection = pScene->GetCamera().ViewProjection();

	g_cRenderer3D.Begin(viewProjection);
	g_cRenderer2D.Begin(viewProjection);

	pScene->OnRender();

	g_cRenderer3D.End();	// 3D 먼저 (깊이 테스트 켜고 그림)
	g_cRenderer2D.End();	// 2D 나중 (깊이 끄고 그 위에 그림)
}

//////////////////////////////////////////////////////////////////////////////////////////
// 창이 닫힐 때 해당 슬롯의 씬을 정리한다. (Application이 호출)
void Director::DetachWindow(Window* _pWindow)
{
	SceneSlot* pSlot = FindSlot(_pWindow);
	if (pSlot == nullptr)
	{
		return;
	}

	// 예약된 씬은 한 번도 실행되지 않았으므로 OnExit 없이 삭제한다.
	if (pSlot->pNext_ != nullptr)
	{
		JC_DELETE_SAFE(pSlot->pNext_);
	}

	if (pSlot->pCurrent_ != nullptr)
	{
		pSlot->pCurrent_->OnExit();
		JC_DELETE_SAFE(pSlot->pCurrent_);
	}

	// 빈 슬롯으로 표시해두면 GetOrCreateSlot이 재활용한다.
	pSlot->pWindow_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 모든 씬 정리 (앱 종료 시 Application::Finalize가 호출)
void Director::Cleanup()
{
	for (int i = 0; i < slots_.Size(); ++i)
	{
		SceneSlot& slot = slots_[i];

		if (slot.pNext_ != nullptr)
		{
			JC_DELETE_SAFE(slot.pNext_);
		}
		if (slot.pCurrent_ != nullptr)
		{
			slot.pCurrent_->OnExit();
			JC_DELETE_SAFE(slot.pCurrent_);
		}
		slot.pWindow_ = nullptr;
	}
	slots_.Clear();
}

NS_SGF_END
