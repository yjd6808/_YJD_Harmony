#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * Io 통합 리스너 — 단일 통지 창구 (ILoadListener/HttpEventListener 승계)
 * 모든 콜백/리스너는 메인 스레드 Update() 펌프에서만 호출된다 (동기 API 제외).
 */

#include "jc/Namespace.h"
#include "jc/Declspec.h"
#include "jc/IO/Engine/IOTypes.h"

NS_JC_BEGIN

class JC_NOVTABLE IIOListener
{
public:
	virtual ~IIOListener() = default;

	virtual void OnProgress(const IOProgress& _progress) {}
	virtual void OnCompleted(const IOResult& _result) {}
	virtual void OnCancelled(const IOResult& _result) {}
	virtual void OnFailed(const IOResult& _result) {}
};

NS_END
