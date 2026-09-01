#include "jc/Io/Http/HttpService.h"
#include "jc/Io/PathResolver.h"
#include "jc/Io/Engine/HttpSource.h"
#include "jc/Io/Engine/MemoryDest.h"
#include "jc/Io/Engine/FileDest.h"
#include "jc/Io/Http/HttpRequest.h"
#include "jc/Io/Http/HttpResponse.h"
#include "jc/Io/Http/IHttpTransport.h"
#include "jc/Io/Http/SyncHttpTransport.h"
#include "jc/Debug/New.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
HttpService::HttpService(PathResolver& _resolver, IoJobEngine& _engine)
	: resolver_(_resolver)
	, engine_(_engine)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HttpService::Initialize(const HttpServiceConfig& _config, _s64 _memoryLimit, _s32 _readUnit)
{
	config_ = _config;
	memoryLimit_ = _memoryLimit;
	readUnit_ = _readUnit;

	pTransport_ = dbg_new SyncHttpTransport();
	if (!pTransport_->Initialize(config_))
	{
		JC_DELETE_SAFE(pTransport_);
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void HttpService::Shutdown()
{
	if (pTransport_ != nullptr)
	{
		pTransport_->Shutdown();
		JC_DELETE_SAFE(pTransport_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
IoHandle HttpService::SendAsync(const HttpRequest& _request, const IoCallback& _callback)
{
	// 메모리 목적지 — 상태 코드를 결과로 전달 (401/404도 전송 자체는 성공)
	return engine_.Submit(MakeShared<HttpSource>(_request, *pTransport_, false),
		MakeShared<MemoryDest>(memoryLimit_, (_u32)readUnit_),
		_request.GetUri().GetRaw(), TransferPolicy{}, _callback, true);
}

//////////////////////////////////////////////////////////////////////////////////////////
IoHandle HttpService::GetAsync(const String& _url, const IoCallback& _callback)
{
	return SendAsync(HttpRequest(_url), _callback);
}

//////////////////////////////////////////////////////////////////////////////////////////
IoHandle HttpService::DownloadAsync(const String& _url, const String& _destPath, const IoCallback& _callback)
{
	// 원격 URL은 해석 대상이 아니고, 로컬 목적지만 주입받은 resolver로 해석한다
	const ResolveResult dest = resolver_.ResolveWritable(_destPath);
	if (!dest.IsOk())
	{
		return engine_.FailImmediate(ieInvalidUri, _destPath, _callback);
	}

	// 파일 다운로드는 2xx만 성공 (구 규약: heHttpStatusFailed + detail_.httpStatus_ 무손실)
	return engine_.Submit(MakeShared<HttpSource>(HttpRequest(jc::Uri(_url)), *pTransport_, true),
		MakeShared<FileDest>(dest.fullPath_),
		_url + " -> " + _destPath, TransferPolicy{}, _callback, true);
}

//////////////////////////////////////////////////////////////////////////////////////////
HttpResponsePtr HttpService::Send(const HttpRequest& _request, bool _waitForBody)
{
	HttpError err = heNone;
	IHttpConnectionPtr spConn = pTransport_->Open(_request, OUT err);
	if (spConn == nullptr)
		return nullptr;

	HttpResponsePtr spRes = MakeShared<HttpResponse>();
	spRes->SetStatusCode(spConn->GetStatusCode());
	spConn->ReadHeaders(OUT spRes->GetHeaders());

	if (!_waitForBody)
	{
		spRes->AttachConnection(spConn);			// 스트리밍 — ReadBody로 소비 후 CloseBody
		return spRes;
	}

	const _s64 total = spRes->GetHeaders().GetContentLength();
	jc::MemoryStreamPtr spBody = MakeShared<jc::MemoryStream>(total > 0 ? (_u32)total : (_u32)readUnit_);

	_byte chunk[64 * 1024];
	for (;;)
	{
		int n = spConn->ReadBody(chunk, sizeof(chunk));
		if (n == 0)
			break;
		if (n < 0)
		{
			spConn->Close();
			return nullptr;
		}
		spBody->Write(chunk, n);
	}
	spConn->Close();
	spRes->AttachBody(spBody);
	return spRes;
}

//////////////////////////////////////////////////////////////////////////////////////////
HttpResponsePtr HttpService::Get(const String& _url)
{
	return Send(HttpRequest(_url), true);
}

//////////////////////////////////////////////////////////////////////////////////////////
IoResultPtr HttpService::Download(const String& _url, const String& _destPath)
{
	IoCallback emptyCallback;
	const ResolveResult dest = resolver_.ResolveWritable(_destPath);
	if (!dest.IsOk())
	{
		engine_.FailImmediate(ieInvalidUri, _destPath, emptyCallback);
		return nullptr;
	}

	return engine_.RunSync(MakeShared<HttpSource>(HttpRequest(jc::Uri(_url)), *pTransport_, true),
		MakeShared<FileDest>(dest.fullPath_), _url + " -> " + _destPath, TransferPolicy{}, true);
}

NS_END
