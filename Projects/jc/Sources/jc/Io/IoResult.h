#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * IOResult — 통합 전송 결과 (구 LoadResult/HttpResult 승계)
 *
 * 사용 예 (04 문서):
 *   daemon.Http().SendAsync(req, [](const IOResult& _r) {
 *       if (_r.error_ == ieNone)              { _r.http_.status_; _r.http_.headers_; _r.http_.body_; }
 *       else if (_r.error_ == ieHttpStatusFailed) { _r.detail_.httpStatus_; }  // 401/404/500 손실 없음
 *   });
 */

#include "jc/Namespace.h"
#include "jc/Primitives/SmartPtr.h"
#include "jc/Container/MemoryStream.h"
#include "jc/IO/IODefine.h"
#include "jc/IO/Http/HttpHeaders.h"

NS_JC_BEGIN

// HTTP 전송의 상태·헤더·바디 묶음 (HTTP가 아닌 전송은 status_ == 0)
struct IOHttpPart
{
	_s32 status_ = 0;
	HttpHeaders headers_;
	MemoryStreamPtr body_;
};

class IOResult
{
public:
	IOResult() = default;
	~IOResult() = default;

	IOResult(const IOResult&) = delete;
	IOResult& operator=(const IOResult&) = delete;

	bool IsSuccess() const { return error_ == ieNone; }

	MemoryStreamPtr TakeData()
	{
		taken_ = true;
		MemoryStreamPtr spOut = data_;
		data_ = nullptr;
		return spOut;
	}
	bool IsTaken() const { return taken_; }

public:
	IOHandle handle_ = InvalidIOHandle;
	IOState state_ = isNone;
	IOError error_ = ieNone;
	IOErrorDetail detail_;			// channelError_(전송 계층 상세), httpStatus_
	IOHttpPart http_;				// HTTP 원천일 때 채워짐 — status_ / headers_ / body_
	MemoryStreamPtr data_;			// 메모리 목적지 결과 (파일 목적지는 nullptr)

private:
	friend class IOEngine;
	bool taken_ = false;
};

using IOResultPtr = SharedPtr<IOResult>;

NS_END
