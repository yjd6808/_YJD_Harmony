#include "jc/IO/Http/HttpService.h"
#include "jc/IO/PathResolver.h"
#include "jc/IO/Http/HttpSource.h"
#include "jc/IO/Memory/MemoryDest.h"
#include "jc/IO/File/FileDest.h"
#include "jc/IO/Http/HttpRequest.h"
#include "jc/IO/Http/HttpResponse.h"
#include "jc/IO/Http/IHttpTransport.h"
#include "jc/IO/Http/SyncHttpTransport.h"
#include "jc/Debug/New.h"

NS_JC_BEGIN

namespace
{
	//////////////////////////////////////////////////////////////////////////////////////
	IOResult MakeLocalError(IOError _error)
	{
		IOResult result;
		result.state_ = isFailed;
		result.error_ = _error;
		return result;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
HttpService::HttpService(PathResolver& _resolver, IOEngine& _engine)
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
IOSourcePtr HttpService::MakeSource(const HttpRequest& _request, bool _failNon2xx, const HttpResponsePtr& _spResponse)
{
	return MakeShared<HttpSource>(_request, *pTransport_, _failNon2xx, _spResponse);
}

//////////////////////////////////////////////////////////////////////////////////////////
IODestPtr HttpService::MakeMemoryDest(const HttpResponsePtr& _spResponse)
{
	// 미리 붙여둔 버퍼 재사용, 없으면 생성 — 호출자 그릇에 직접 기록
	if (_spResponse->GetBody() == nullptr)
		_spResponse->AttachBody(MakeShared<MemoryStream>(0u));
	return MakeShared<MemoryDest>(_spResponse->GetBodyPtr(), memoryLimit_);
}

//////////////////////////////////////////////////////////////////////////////////////////
IOResult HttpService::Send(const HttpRequest& _request, const HttpResponsePtr& _spResponse)
{
	if (_spResponse == nullptr)
		return MakeLocalError(ieInvalidUri);

	return engine_.RunSync(MakeSource(_request, false, _spResponse),
		MakeMemoryDest(_spResponse), _request.GetUri().GetRaw(), TransferPolicy{});
}

//////////////////////////////////////////////////////////////////////////////////////////
IOHandle HttpService::SendAsync(const HttpRequest& _request, const HttpResponsePtr& _spResponse, const IOCallback& _callback)
{
	if (_spResponse == nullptr)
		return engine_.FailImmediate(ieInvalidUri, _request.GetUri().GetRaw(), _callback);

	return engine_.Submit(MakeSource(_request, false, _spResponse),
		MakeMemoryDest(_spResponse), _request.GetUri().GetRaw(), TransferPolicy{}, _callback);
}

//////////////////////////////////////////////////////////////////////////////////////////
IOResult HttpService::Get(const String& _url, const HttpResponsePtr& _spResponse)
{
	return Send(HttpRequest(_url), _spResponse);
}

//////////////////////////////////////////////////////////////////////////////////////////
IOResult HttpService::Post(const String& _url, const MemoryStreamPtr& _spBody, const String& _contentType, const HttpResponsePtr& _spResponse)
{
	HttpRequest request(_url);
	request.SetMethod(hmPost);
	if (!_contentType.IsEmpty())
		request.SetHeader("Content-Type", _contentType);
	if (_spBody != nullptr)
		request.SetBody(_spBody);
	return Send(request, _spResponse);
}

//////////////////////////////////////////////////////////////////////////////////////////
IOHandle HttpService::GetAsync(const String& _url, const HttpResponsePtr& _spResponse, const IOCallback& _callback)
{
	return SendAsync(HttpRequest(_url), _spResponse, _callback);
}

//////////////////////////////////////////////////////////////////////////////////////////
IOHandle HttpService::PostAsync(const String& _url, const MemoryStreamPtr& _spBody, const String& _contentType, const HttpResponsePtr& _spResponse, const IOCallback& _callback)
{
	HttpRequest request(_url);
	request.SetMethod(hmPost);
	if (!_contentType.IsEmpty())
		request.SetHeader("Content-Type", _contentType);
	if (_spBody != nullptr)
		request.SetBody(_spBody);
	return SendAsync(request, _spResponse, _callback);
}

//////////////////////////////////////////////////////////////////////////////////////////
IOResult HttpService::Download(const String& _url, const String& _destPath, const HttpResponsePtr& _spResponse)
{
	// 원격 URL은 해석 대상이 아니고, 로컬 목적지만 주입받은 resolver로 해석한다
	const ResolveResult dest = resolver_.ResolveWritable(_destPath);
	if (!dest.IsOk())
		return MakeLocalError(ieInvalidUri);

	// 파일 다운로드는 2xx만 성공 — 상태/헤더는 _spResponse가 있으면 보존
	return engine_.RunSync(MakeSource(HttpRequest(jc::Uri(_url)), true, _spResponse),
		MakeShared<FileDest>(dest.fullPath_), _url + " -> " + _destPath, TransferPolicy{});
}

//////////////////////////////////////////////////////////////////////////////////////////
IOHandle HttpService::DownloadAsync(const String& _url, const String& _destPath, const HttpResponsePtr& _spResponse, const IOCallback& _callback)
{
	const ResolveResult dest = resolver_.ResolveWritable(_destPath);
	if (!dest.IsOk())
		return engine_.FailImmediate(ieInvalidUri, _destPath, _callback);

	return engine_.Submit(MakeSource(HttpRequest(jc::Uri(_url)), true, _spResponse),
		MakeShared<FileDest>(dest.fullPath_), _url + " -> " + _destPath, TransferPolicy{}, _callback);
}

NS_END
