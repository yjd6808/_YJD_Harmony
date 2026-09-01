#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * IIoSource — 바이트를 내놓는 쪽 계약 (C안 데이터 평면)
 *   요청 정보(URL/헤더/경로)는 생성자에서 주입받고, Open 성공 시 메타를 채운다.
 *   Read: >0 읽은 바이트, 0 = EOF, <0 = 에러
 */

#include "jc/Namespace.h"
#include "jc/Declspec.h"
#include "jc/Primitives/SmartPtr.h"
#include "jc/Io/Http/HttpHeaders.h"
#include "jc/Io/IoDefine.h"

NS_JC_BEGIN

struct IoSourceInfo
{
	_s64 totalBytes_ = -1;					// 미상 = -1
	_s32 httpStatus_ = 0;					// HTTP가 아니면 0
	SharedPtr<HttpHeaders> spHeaders_;		// 응답 헤더 (HTTP일 때만 채움)
};

class JC_NOVTABLE IIoSource
{
public:
	virtual ~IIoSource() = default;

	virtual bool Open(OUT IoSourceInfo& _info) = 0;
	virtual _s32 Read(_byte* _pBuffer, _s32 _len) = 0;
	virtual void Close() = 0;
	virtual IoError GetLastError() const = 0;
	virtual IoErrorDetail GetLastErrorDetail() const = 0;
};

using IoSourcePtr = SharedPtr<IIoSource>;

NS_END
