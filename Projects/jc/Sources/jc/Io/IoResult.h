#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * IoResult — 통합 전송 결과 (구 LoadResult/HttpResult 승계)
 *
 * 사용 예 (04 문서):
 *   daemon.Http().SendAsync(req, [](const IoResult& _r) {
 *       if (_r.error_ == ieNone)              { _r.http_.status_; _r.http_.headers_; _r.http_.body_; }
 *       else if (_r.error_ == ieHttpStatusFailed) { _r.detail_.httpStatus_; }  // 401/404/500 손실 없음
 *   });
 */

#include "jc/Namespace.h"
#include "jc/Primitives/SmartPtr.h"
#include "jc/Container/MemoryStream.h"
#include "jc/Io/IoDefine.h"
#include "jc/Io/Http/HttpHeaders.h"

NS_JC_BEGIN

// HTTP 전송의 상태·헤더·바디 묶음 (HTTP가 아닌 전송은 status_ == 0)
struct IoHttpPart
{
	_s32 status_ = 0;
	HttpHeaders headers_;
	MemoryStreamPtr body_;
};

class IoResult
{
public:
	IoResult() = default;
	~IoResult() = default;

	IoResult(const IoResult&) = delete;
	IoResult& operator=(const IoResult&) = delete;

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
	IoHandle handle_ = InvalidIoHandle;
	IoState state_ = isNone;
	IoError error_ = ieNone;
	IoErrorDetail detail_;			// channelError_(전송 계층 상세), httpStatus_
	IoHttpPart http_;				// HTTP 원천일 때 채워짐 — status_ / headers_ / body_
	MemoryStreamPtr data_;			// 메모리 목적지 결과 (파일 목적지는 nullptr)

private:
	friend class IoJobEngine;
	bool taken_ = false;
};

using IoResultPtr = SharedPtr<IoResult>;

NS_END
