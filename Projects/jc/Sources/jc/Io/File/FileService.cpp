#include "jc/IO/File/FileService.h"
#include "jc/IO/PathResolver.h"
#include "jc/IO/File/FileSource.h"
#include "jc/IO/Memory/MemoryDest.h"
#include "jc/IO/File/FileDest.h"
#include "jc/IO/IOResult.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
FileService::FileService(PathResolver& _resolver, IOEngine& _engine)
	: resolver_(_resolver)
	, engine_(_engine)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void FileService::InitializeDefaults(_s64 _memoryLimit, _s32 _readUnit)
{
	memoryLimit_ = _memoryLimit;
	readUnit_ = _readUnit;
}

//////////////////////////////////////////////////////////////////////////////////////////
IOHandle FileService::LoadAsync(const String& _path, const IOCallback& _callback, const LoadOptions& _options)
{
	// ★ 접수 시점 1회 해석 — 이후 Mount 재바인딩이 일어나도 진행 중 잡은 오염되지 않는다 (R11.5)
	const ResolveResult r = resolver_.Resolve(_path, _options.baseAlias_);
	if (r.error_ == preEmptyPath || r.error_ == preUnknownAlias)
	{
		return engine_.FailImmediate(ieInvalidUri, _path, _callback);
	}
	if (r.error_ == preNotFound)
	{
		return engine_.FailImmediate(ieOpenFailed, _path, _callback);
	}

	TransferPolicy policy = _options.policy_;
	if (policy.readUnit_ == 0)
		policy.readUnit_ = readUnit_;

	return engine_.Submit(MakeShared<FileSource>(r.fullPath_),
		MakeShared<MemoryDest>(memoryLimit_, (readUnit_ > (_s32)0 ? (_u32)readUnit_ : 256u * 1024u)),
		_path, policy, _callback, false);
}

//////////////////////////////////////////////////////////////////////////////////////////
IOHandle FileService::DownloadAsync(const String& _srcPath, const String& _destPath, const IOCallback& _callback)
{
	const ResolveResult src = resolver_.Resolve(_srcPath);
	if (!src.IsOk())
	{
		return engine_.FailImmediate(src.error_ == preNotFound ? ieOpenFailed : ieInvalidUri, _srcPath, _callback);
	}

	const ResolveResult dest = resolver_.ResolveWritable(_destPath);
	if (!dest.IsOk())
	{
		return engine_.FailImmediate(ieInvalidUri, _destPath, _callback);
	}

	return engine_.Submit(MakeShared<FileSource>(src.fullPath_),
		MakeShared<FileDest>(dest.fullPath_),						// .part → Commit 규약
		_srcPath + " -> " + _destPath, TransferPolicy{}, _callback, false);
}

//////////////////////////////////////////////////////////////////////////////////////////
IOResultPtr FileService::Load(const String& _path, const LoadOptions& _options)
{
	const ResolveResult r = resolver_.Resolve(_path, _options.baseAlias_);
	if (r.error_ == preEmptyPath || r.error_ == preUnknownAlias)
	{
		IOResultPtr spRes = MakeShared<IOResult>();
		spRes->error_ = ieInvalidUri;
		spRes->state_ = isFailed;
		return spRes;
	}
	if (r.error_ == preNotFound)
	{
		IOResultPtr spRes = MakeShared<IOResult>();
		spRes->error_ = ieOpenFailed;
		spRes->state_ = isFailed;
		return spRes;
	}

	TransferPolicy policy = _options.policy_;
	if (policy.readUnit_ == 0)
		policy.readUnit_ = readUnit_;

	return engine_.RunSync(MakeShared<FileSource>(r.fullPath_),
		MakeShared<MemoryDest>(memoryLimit_, (_u32)readUnit_), _path, policy, false);
}

//////////////////////////////////////////////////////////////////////////////////////////
IOResultPtr FileService::Download(const String& _srcPath, const String& _destPath)
{
	const ResolveResult src = resolver_.Resolve(_srcPath);
	const ResolveResult dest = resolver_.ResolveWritable(_destPath);
	if (!src.IsOk())
	{
		IOResultPtr spRes = MakeShared<IOResult>();
		spRes->error_ = src.error_ == preNotFound ? ieOpenFailed : ieInvalidUri;
		spRes->state_ = isFailed;
		return spRes;
	}
	if (!dest.IsOk())
	{
		IOResultPtr spRes = MakeShared<IOResult>();
		spRes->error_ = ieInvalidUri;
		spRes->state_ = isFailed;
		return spRes;
	}

	return engine_.RunSync(MakeShared<FileSource>(src.fullPath_),
		MakeShared<FileDest>(dest.fullPath_), _srcPath + " -> " + _destPath, TransferPolicy{}, false);
}

NS_END
