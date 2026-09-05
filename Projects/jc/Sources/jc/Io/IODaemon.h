#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * IODaemon — 통합 I/O 데몬 「조립 지휘자」 (04 문서 §3.1 최종 구조)
 *
 *   PathResolver resolver_      이름공간 · 데몬 직속 · 순수 클래스
 *         ▲ 참조 주입   ▲ 참조 주입
 *   FileService files_          파일 창구 (대등한 소비자)
 *   HttpService  http_          HTTP 창구 (대등한 소비자)
 *   IOEngine  engine_        핸들·큐·워커·펌프·취소 (공용 기계 1벌)
 *
 * 조립 코드는 이 클래스의 멤버 선언 순서(1→2→3) 한 곳에만 존재한다. 역방향 의존 0.
 */

#include "jc/Namespace.h"
#include "jc/Pattern/NonCopyableh.h"
#include "jc/IO/IODefine.h"
#include "jc/IO/Engine/IIOListener.h"
#include "jc/IO/IOResult.h"
#include "jc/IO/PathResolver.h"
#include "jc/IO/Engine/IOEngine.h"
#include "jc/IO/File/FileService.h"
#include "jc/IO/Http/HttpService.h"

NS_JC_BEGIN

class IODaemon final : public NonCopyableNonMovable
{
public:
	IODaemon();
	~IODaemon();

	bool Initialize(const IODaemonConfig& _config = IODaemonConfig());
	void Shutdown();

	void SetListener(IIOListener* _pListener, bool _transferOwnership = false);

	void Update();

	PathResolver& Resolver() { return resolver_; }
	FileService& Files() { return files_; }
	HttpService& Http() { return http_; }

	IOState GetState(IOHandle _handle) const { return engine_.GetState(_handle); }
	bool GetProgress(IOHandle _handle, OUT IOProgress& _out) const { return engine_.GetProgress(_handle, OUT _out); }
	bool Cancel(IOHandle _handle) { return engine_.Cancel(_handle); }
	void CancelAll() { engine_.CancelAll(); }
	int GetActiveCount() const { return engine_.GetActiveCount(); }
	void GetActiveSnapshot(OUT Vector<IOProgress>& _out) const { engine_.GetActiveSnapshot(OUT _out); }

private:
	PathResolver resolver_;
	IOEngine engine_;
	FileService files_;
	HttpService http_;
	bool initialized_ = false;
};

NS_END
