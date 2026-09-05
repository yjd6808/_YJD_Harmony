#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * HTTP 도메인 경량 타입 (jc/Io 소속)
 *   - HttpMethod(hm*) / HttpMethodName / HttpError(he*, 전송 계층 상세 코드)
 *   - HttpServiceConfig : IODaemonConfig.http_ 에 주입되는 HTTP 전용 설정
 */

#include "jc/Namespace.h"
#include "jc/Type.h"
#include "jc/Define.h"
#include "jc/Primitives/String.h"

NS_JC_BEGIN

enum HttpMethod
{
	hmGet,
	hmPost,
	hmPut,
	hmDelete,
	hmHead,
	hmPatch
};

inline const char* HttpMethodName(HttpMethod _method)
{
	switch (_method)
	{
	case hmGet: return "GET";
	case hmPost: return "POST";
	case hmPut: return "PUT";
	case hmDelete: return "DELETE";
	case hmHead: return "HEAD";
	case hmPatch: return "PATCH";
	default: return "GET";
	}
}

// 전송 계층(WinHTTP) 상세 에러 — IOResult.channelError_ 로 보존된다 (R4)
//   HTTP 고유값만 유지 (엔진 공용값과 겹치는 거울값 heNone/heWriteFailed/heMemoryLimit/heCancelledByUser/heShutdown 제거)
enum HttpError
{
	heInvalidUri,
	heConnectFailed,
	heTlsFailed,
	heSendFailed,
	heRecvFailed,
	heTimeout,
	heTooManyRedirects,
	heHttpStatusFailed
};

struct HttpServiceConfig
{
	String userAgent_ = "jnet-http/1.0";
	int connectTimeoutMs_ = 10000;
	int sendTimeoutMs_ = 10000;
	int receiveTimeoutMs_ = 30000;
	int maxRedirects_ = 5;
};

NS_END
