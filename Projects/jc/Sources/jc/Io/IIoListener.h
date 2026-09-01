#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * Io 통합 리스너 — 단일 통지 창구 (ILoadListener/HttpEventListener 승계)
 * 모든 콜백/리스너는 메인 스레드 Update() 펌프에서만 호출된다 (동기 API 제외).
 */

#include "jc/Namespace.h"
#include "jc/Declspec.h"
#include "jc/Io/IoDefine.h"

NS_JC_BEGIN

class JC_NOVTABLE IIoListener
{
public:
	virtual ~IIoListener() = default;

	virtual void OnProgress(const IoProgress& _progress) {}
	virtual void OnCompleted(const IoResult& _result) {}
	virtual void OnCancelled(const IoResult& _result) {}
	virtual void OnFailed(const IoResult& _result) {}
};

NS_END
