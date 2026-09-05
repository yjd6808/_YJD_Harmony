#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * FileService — 파일 도메인 창구 (구 FileLoader API 승계)
 *   접수 시점에 데몬 직속 resolver로 1회 해석하고, 워커는 확정된 절대경로만 본다 (재바인딩 무오염).
 */

#include "jc/Namespace.h"
#include "jc/Pattern/NonCopyableh.h"
#include "jc/IO/IODefine.h"
#include "jc/IO/Engine/IOEngine.h"

NS_JC_BEGIN

class PathResolver;

class FileService final : public NonCopyableNonMovable
{
public:
	IOHandle LoadAsync(const String& _path, const IOCallback& _callback = nullptr, const LoadOptions& _options = LoadOptions());
	IOHandle DownloadAsync(const String& _srcPath, const String& _destPath, const IOCallback& _callback = nullptr);

	IOResultPtr Load(const String& _path, const LoadOptions& _options = LoadOptions());
	IOResultPtr Download(const String& _srcPath, const String& _destPath);

private:
	friend class IODaemon;
	FileService(PathResolver& _resolver, IOEngine& _engine);
	void InitializeDefaults(_s64 _memoryLimit, _s32 _readUnit);

	PathResolver& resolver_;
	IOEngine& engine_;
	_s64 memoryLimit_ = 256LL * 1024 * 1024;
	_s32 readUnit_ = 256 * 1024;
};

NS_END
