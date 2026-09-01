#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * IoDaemon — 통합 I/O 데몬 「조립 지휘자」 (04 문서 §3.1 최종 구조)
 *
 *   PathResolver resolver_      이름공간 · 데몬 직속 · 순수 클래스
 *         ▲ 참조 주입   ▲ 참조 주입
 *   FileService files_          파일 창구 (대등한 소비자)
 *   HttpService  http_          HTTP 창구 (대등한 소비자)
 *   IoJobEngine  engine_        핸들·큐·워커·펌프·취소 (공용 기계 1벌)
 *
 * 조립 코드는 이 클래스의 멤버 선언 순서(1→2→3) 한 곳에만 존재한다. 역방향 의존 0.
 */

#include "jc/Namespace.h"
#include "jc/Pattern/NonCopyableh.h"
#include "jc/Io/IoDefine.h"
#include "jc/Io/IIoListener.h"
#include "jc/Io/IoResult.h"
#include "jc/Io/PathResolver.h"
#include "jc/Io/Engine/IoJobEngine.h"
#include "jc/Io/Files/FileService.h"
#include "jc/Io/Http/HttpService.h"

NS_JC_BEGIN

class IoDaemon final : public NonCopyableNonMovable
{
public:
	IoDaemon();
	~IoDaemon();

	bool Initialize(const IoDaemonConfig& _config = IoDaemonConfig());
	void Shutdown();

	void SetListener(IIoListener* _pListener, bool _transferOwnership = false);

	void Update();

	PathResolver& Resolver() { return resolver_; }
	FileService& Files() { return files_; }
	HttpService& Http() { return http_; }

	IoState GetState(IoHandle _handle) const { return engine_.GetState(_handle); }
	bool GetProgress(IoHandle _handle, OUT IoProgress& _out) const { return engine_.GetProgress(_handle, OUT _out); }
	bool Cancel(IoHandle _handle) { return engine_.Cancel(_handle); }
	void CancelAll() { engine_.CancelAll(); }
	int GetActiveCount() const { return engine_.GetActiveCount(); }
	void GetActiveSnapshot(OUT Vector<IoProgress>& _out) const { engine_.GetActiveSnapshot(OUT _out); }

private:
	PathResolver resolver_;
	IoJobEngine engine_;
	FileService files_;
	HttpService http_;
	bool initialized_ = false;
};

NS_END
