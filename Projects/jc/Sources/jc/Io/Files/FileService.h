#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * FileService — 파일 도메인 창구 (구 FileLoader API 승계)
 *   접수 시점에 데몬 직속 resolver로 1회 해석하고, 워커는 확정된 절대경로만 본다 (재바인딩 무오염).
 */

#include "jc/Namespace.h"
#include "jc/Pattern/NonCopyableh.h"
#include "jc/Io/IoDefine.h"
#include "jc/Io/Engine/IoJobEngine.h"

NS_JC_BEGIN

class PathResolver;

class FileService final : public NonCopyableNonMovable
{
public:
	IoHandle LoadAsync(const String& _path, const IoCallback& _callback = nullptr, const LoadOptions& _options = LoadOptions());
	IoHandle DownloadAsync(const String& _srcPath, const String& _destPath, const IoCallback& _callback = nullptr);

	IoResultPtr Load(const String& _path, const LoadOptions& _options = LoadOptions());
	IoResultPtr Download(const String& _srcPath, const String& _destPath);

private:
	friend class IoDaemon;
	FileService(PathResolver& _resolver, IoJobEngine& _engine);
	void InitializeDefaults(_s64 _memoryLimit, _s32 _readUnit);

	PathResolver& resolver_;
	IoJobEngine& engine_;
	_s64 memoryLimit_ = 256LL * 1024 * 1024;
	_s32 readUnit_ = 256 * 1024;
};

NS_END
