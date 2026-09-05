#include "jc/IO/File/FileService.h"
#include "jc/IO/PathResolver.h"
#include "jc/IO/File/FileSource.h"
#include "jc/IO/Memory/MemorySource.h"
#include "jc/IO/Memory/MemoryDest.h"
#include "jc/IO/File/FileDest.h"

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
IOResult FileService::Load(const String& _path, const MemoryStreamPtr& _spOut, _s64 _srcOffset, _s64 _srcCount, const LoadOptions& _options)
{
	if (_spOut == nullptr)
		return MakeLocalError(ieInvalidUri);

	// ★ 접수 시점 1회 해석 — 이후 Mount 재바인딩이 일어나도 진행 중 잡은 오염되지 않는다 (R11.5)
	const ResolveResult r = resolver_.Resolve(_path, _options.baseAlias_);
	if (r.error_ == preEmptyPath || r.error_ == preUnknownAlias)
		return MakeLocalError(ieInvalidUri);
	if (r.error_ == preNotFound)
		return MakeLocalError(ieOpenFailed);

	TransferPolicy policy = _options.policy_;
	if (policy.readUnit_ == 0)
		policy.readUnit_ = readUnit_;

	return engine_.RunSync(MakeShared<FileSource>(r.fullPath_, _srcOffset, _srcCount),
		MakeShared<MemoryDest>(_spOut, memoryLimit_), _path, policy);
}

//////////////////////////////////////////////////////////////////////////////////////////
IOResult FileService::Load(const String& _path, _byte* _pOut, _u32 _outCapacity, _s64 _srcOffset, _s64 _srcCount, const LoadOptions& _options)
{
	if (_pOut == nullptr)
		return MakeLocalError(ieInvalidUri);

	// raw 버퍼는 non-owner 스트림으로 감싼다 — 용량 초과 시 ieBufferTooSmall (§1 계약 2)
	//   호출자 버퍼에는 기록된 만큼만 유효 — bytesTransferred_가 힌트
	MemoryStreamPtr spWrap = MakeShared<MemoryStream>(_pOut, _outCapacity, false);
	return Load(_path, spWrap, _srcOffset, _srcCount, _options);
}

//////////////////////////////////////////////////////////////////////////////////////////
IOHandle FileService::LoadAsync(const String& _path, const MemoryStreamPtr& _spOut, const IOCallback& _callback, _s64 _srcOffset, _s64 _srcCount, const LoadOptions& _options)
{
	if (_spOut == nullptr)
		return engine_.FailImmediate(ieInvalidUri, _path, _callback);

	const ResolveResult r = resolver_.Resolve(_path, _options.baseAlias_);
	if (r.error_ == preEmptyPath || r.error_ == preUnknownAlias)
		return engine_.FailImmediate(ieInvalidUri, _path, _callback);
	if (r.error_ == preNotFound)
		return engine_.FailImmediate(ieOpenFailed, _path, _callback);

	TransferPolicy policy = _options.policy_;
	if (policy.readUnit_ == 0)
		policy.readUnit_ = readUnit_;

	return engine_.Submit(MakeShared<FileSource>(r.fullPath_, _srcOffset, _srcCount),
		MakeShared<MemoryDest>(_spOut, memoryLimit_), _path, policy, _callback);
}

//////////////////////////////////////////////////////////////////////////////////////////
IOResult FileService::Save(const String& _path, const MemoryStreamPtr& _spData)
{
	if (_spData == nullptr)
		return MakeLocalError(ieInvalidUri);

	const ResolveResult dest = resolver_.ResolveWritable(_path);	// 쓰기: 존재검사 없이 주 디렉토리
	if (!dest.IsOk())
		return MakeLocalError(ieInvalidUri);

	return engine_.RunSync(MakeShared<MemorySource>(_spData),
		MakeShared<FileDest>(dest.fullPath_), _path + " -> " + dest.fullPath_, TransferPolicy{});
}

//////////////////////////////////////////////////////////////////////////////////////////
IOResult FileService::Save(const String& _path, const _byte* _pData, _u32 _len)
{
	if (_pData == nullptr && _len > 0)
		return MakeLocalError(ieInvalidUri);

	// non-owner 랩은 writeOffset_=0이라 기존 내용이 안 보인다 — raw는 1회 복사본으로 저장
	MemoryStreamPtr spCopy = MakeShared<MemoryStream>(_len);
	if (_len > 0)
		spCopy->Write(_pData, _len);
	return Save(_path, spCopy);
}

//////////////////////////////////////////////////////////////////////////////////////////
IOHandle FileService::SaveAsync(const String& _path, const MemoryStreamPtr& _spData, const IOCallback& _callback)
{
	if (_spData == nullptr)
		return engine_.FailImmediate(ieInvalidUri, _path, _callback);

	const ResolveResult dest = resolver_.ResolveWritable(_path);
	if (!dest.IsOk())
		return engine_.FailImmediate(ieInvalidUri, _path, _callback);

	return engine_.Submit(MakeShared<MemorySource>(_spData),
		MakeShared<FileDest>(dest.fullPath_), _path + " -> " + dest.fullPath_, TransferPolicy{}, _callback);
}

//////////////////////////////////////////////////////////////////////////////////////////
String FileService::LoadText(const String& _path, const LoadOptions& _options)
{
	MemoryStreamPtr spOut = MakeShared<MemoryStream>(0u);
	IOResult result = Load(_path, spOut, 0, 0, _options);
	if (!result.IsOk() || spOut->GetSize() == 0)
		return String();
	String text;
	text.Append((const char*)spOut->GetData(), (int)spOut->GetSize());
	return text;
}

//////////////////////////////////////////////////////////////////////////////////////////
IOResult FileService::SaveText(const String& _path, const String& _text)
{
	return Save(_path, (const _byte*)_text.Source(), (_u32)_text.Length());
}

NS_END
