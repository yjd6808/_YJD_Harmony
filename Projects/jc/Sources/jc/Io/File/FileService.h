#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * FileService — 파일 도메인 창구
 *   Load/LoadAsync : 파일 → 호출자 버퍼 (_srcOffset/_srcCount, 0 = 끝까지)
 *   Save/SaveAsync : 호출자 버퍼 → 파일 (.part 기록 후 원자적 커밋, 실패 시 원본 보존)
 *   [계약] raw 버퍼 오버로드는 동기 전용 / 비동기는 완료 콜백 전까지 버퍼 접근 금지 (잡이 배타 소유)
 *
 * 사용 예:
 *   MemoryStreamPtr spOut = MakeShared<MemoryStream>(0u);
 *   IOResult r = daemon.Files().Load("save:/slot1.dat", spOut);
 *   if (r.IsOk())
 *   {
 *       Parse(spOut->GetData(), spOut->GetSize());
 *   }
 */

#include "jc/Namespace.h"
#include "jc/Pattern/NonCopyableh.h"
#include "jc/IO/MemoryStream.h"
#include "jc/IO/Engine/IOTypes.h"
#include "jc/IO/Engine/IOEngine.h"

NS_JC_BEGIN

class PathResolver;

class FileService final : public NonCopyableNonMovable
{
public:
	// 동기 — 파일 → 호출자 스트림. 쓴 양은 bytesTransferred_로 확인
	//   사용 예) IOResult r = daemon.Files().Load("res:/pack.bin", spOut, 1024, 4096);	// 1KB 지점부터 4KB
	IOResult Load(const String& _path, const MemoryStreamPtr& _spOut, _s64 _srcOffset = 0, _s64 _srcCount = 0, const LoadOptions& _options = LoadOptions());

	// 동기 전용 — 파일 → raw 버퍼. 용량 부족 시 ieBufferTooSmall + bytesTransferred_에 기록된 양
	//   사용 예) _byte header[64];
	//            IOResult r = daemon.Files().Load("res:/pack.bin", header, sizeof(header), 0, 64);
	IOResult Load(const String& _path, _byte* _pOut, _u32 _outCapacity, _s64 _srcOffset = 0, _s64 _srcCount = 0, const LoadOptions& _options = LoadOptions());

	// 비동기 — [계약] 완료 콜백 전까지 _spOut 접근 금지
	//   사용 예) IOHandle h = daemon.Files().LoadAsync("save:/slot1.dat", spOut,
	//                [spOut](const IOResult& _r)
	//                {
	//                    if (_r.IsOk()) Parse(spOut->GetData(), spOut->GetSize());
	//                });
	//            // 필요 시 daemon.Cancel(h);
	IOHandle LoadAsync(const String& _path, const MemoryStreamPtr& _spOut, const IOCallback& _callback = nullptr, _s64 _srcOffset = 0, _s64 _srcCount = 0, const LoadOptions& _options = LoadOptions());

	// 동기 — 호출자 버퍼 → 파일 (.part → Commit)
	//   사용 예) IOResult r = daemon.Files().Save("save:/slot1.dat", spBuffer);
	IOResult Save(const String& _path, const MemoryStreamPtr& _spData);
	IOResult Save(const String& _path, const _byte* _pData, _u32 _len);		// raw 버전 — 동기 전용

	// 비동기 — [계약] 완료 콜백 전까지 _spData 수정 금지
	//   사용 예) daemon.Files().SaveAsync("save:/slot1.dat", spBuffer,
	//                [](const IOResult& _r) { if (!_r.IsOk()) _LogWarn_("save fail: %d", _r.error_); });
	IOHandle SaveAsync(const String& _path, const MemoryStreamPtr& _spData, const IOCallback& _callback = nullptr);

	// 텍스트 편의 래퍼 — 내부에서 Load/Save 재사용
	String LoadText(const String& _path, const LoadOptions& _options = LoadOptions());
	IOResult SaveText(const String& _path, const String& _text);

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
