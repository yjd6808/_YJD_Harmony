/*
 * 작성자: 윤정도
 * 생성일: 8/23/2026
 * =====================
 * 로더 테스트 공용 헬퍼
 *  - 결정론적 패턴 데이터 생성/검증
 *  - Update 폴링 헬퍼
 *  - 느린 데이터 소스 테스트 더블 (취소/진행/한도 테스트의 결정론 확보)
 *  - 기록형 리스너 (통지 횟수/순서 검증)
 */

#pragma once

#include "jc/Primitives/Uri.h"
#include "jc/Container/MemoryStream.h"
#include "jc/FileSystem/FileStream.h"
#include "jc/FileSystem/Loader/FileLoader.h"
#include "jc/FileSystem/Loader/IFileDataSource.h"
#include "jc/FileSystem/Loader/IFileLoaderListener.h"

#include <chrono>

namespace loadertest
{
	// =====================================================================
	// 패턴 데이터 — byte[i] = i % 251 (결정론, 오프셋만 알면 기대값 계산 가능)
	// =====================================================================
	inline _byte Pattern(_s64 _i) { return (_byte)(_i % 251); }

	inline bool VerifyPatternBytes(const _byte* _p, _s64 _len, _s64 _startOffset = 0)
	{
		for (_s64 i = 0; i < _len; ++i)
			if (_p[i] != Pattern(_startOffset + i))
				return false;
		return true;
	}

	// 패턴 파일 생성 — 기존 FileStream 실제 API 사용 (생성자에서 예외 발생 가능)
	inline void CreatePatternFile(const char* _path, int _size)
	{
		CRuntime::FileDelete(_path);
		FileStream stream(_path, FileAccess::eWrite, FileMode::eCreate);
		_byte chunk[4096];
		int written = 0;
		while (written < _size)
		{
			const int n = Math::Min((int)sizeof(chunk), _size - written);
			for (int i = 0; i < n; ++i)
				chunk[i] = Pattern(written + i);
			stream.Write(chunk, 0, n);
			written += n;
		}
	}

	// 파일 존재 확인 — 검증된 FileStream API만 사용 (없으면 RuntimeException)
	inline bool FileExistsForTest(const char* _path)
	{
		try { FileStream stream(_path, FileAccess::eRead, FileMode::eOpen); return true; }
		catch (RuntimeException&) { return false; }
	}
	inline bool FileExistsForTest(const jc::String& _path) { return FileExistsForTest(_path.Source()); }

	// 파일 내용이 패턴과 일치하는지 검증
	inline bool VerifyPatternFile(const char* _path, int _expectedSize)
	{
		if (!FileExistsForTest(_path)) return false;
		FileStream stream(_path, FileAccess::eRead, FileMode::eOpen);
		if (stream.GetLength() != _expectedSize) return false;
		_byte chunk[4096];
		int readTotal = 0;
		for (;;)
		{
			const int n = stream.Read(chunk, 0, (int)sizeof(chunk));
			if (n <= 0) break;
			if (!VerifyPatternBytes(chunk, n, readTotal)) return false;
			readTotal += n;
		}
		return readTotal == _expectedSize;
	}

	// =====================================================================
	// Update 폴링 헬퍼 — 모든 비동기 대기는 이 함수로만 (sleep 동기화 금지)
	// =====================================================================
	template <typename TUpdatable, typename TPred>
	inline bool PollUntil(TUpdatable& _target, TPred&& _pred, int _timeoutMs = 5000)
	{
		const auto start = std::chrono::steady_clock::now();
		for (;;)
		{
			_target.Update();
			if (_pred()) return true;
			const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - start).count();
			if (elapsedMs >= _timeoutMs) return false;
			Thread::Sleep(1);
		}
	}

	// =====================================================================
	// 느린 패턴 소스 — 파일 없이 합성 데이터를 청크당 지연을 줌서 제공.
	// eHttp 스킴 자리에 등록해서 사용 (jc_gtest는 jnet 미링킹 — 스킴 자체는 레지스트리 검증용)
	// - totalBytes_ = -1 설정 시 크기 미상 소스 재현 (memoryLimit 중도 초과 테스트)
	// - openError_ 설정 시 Open 단계 실패 재현
	// =====================================================================
	struct SlowSourceSpec
	{
		_s64 totalBytes_ = 0;            // 실제 제공할 바이트 수
		bool reportUnknownSize_ = false; // true면 Open 시 totalBytes = -1 보고
		int  chunkSize_ = 4096;          // Read 1회당 최대 반환 바이트
		int  delayPerReadMs_ = 0;        // Read 1회당 지연 (취소 윈도우 확보)
		int  sourceStatus_ = 0;          // FileDataSourceInfo.sourceStatus_로 보고
		LoadError openError_ = LoadError::leNone; // eNone 아니면 Open 실패
	};

	class TestSlowDataSource final : public IFileDataSource
	{
	public:
		explicit TestSlowDataSource(const SlowSourceSpec& _spec) : spec_(_spec) {}

		bool Open(const Uri& _uri, OUT FileDataSourceInfo& _info) override
		{
			(void)_uri;
			_info.sourceStatus_ = spec_.sourceStatus_;
			if (spec_.openError_ != LoadError::leNone) { lastError_ = spec_.openError_; return false; }
			_info.totalBytes_ = spec_.reportUnknownSize_ ? -1 : spec_.totalBytes_;
			return true;
		}

		int Read(void* _pBuffer, int _len) override
		{
			if (spec_.delayPerReadMs_ > 0)
				Thread::Sleep(spec_.delayPerReadMs_);
			const _s64 remain = spec_.totalBytes_ - offset_;
			if (remain <= 0) return 0;                       // EOF
			const int n = (int)Math::Min<_s64>(Math::Min(_len, spec_.chunkSize_), remain);
			_byte* p = (_byte*)_pBuffer;
			for (int i = 0; i < n; ++i)
				p[i] = Pattern(offset_ + i);
			offset_ += n;
			return n;
		}

		void Close() override { closed_ = true; }
		LoadError GetLastError() const override { return lastError_; }

	private:
		SlowSourceSpec spec_;
		_s64 offset_ = 0;
		bool closed_ = false;
		LoadError lastError_ = LoadError::leNone;
	};

	// 등록 헬퍼 — eHttp 자리에 느린 소스 설치
	inline void InstallSlowSource(FileLoader& _loader, const SlowSourceSpec& _spec)
	{
		_loader.RegisterFileDataSource(UriScheme::usHttp,
			[_spec] { return MakeShared<TestSlowDataSource>(_spec); });
	}

	// =====================================================================
	// 기록형 리스너 — 통지 횟수/마지막 값/호출 순서 기록 (단일 스레드 Update에서만 호출되므로 락 불필요)
	// =====================================================================
	class RecordingLoadListener final : public ILoadListener
	{
	public:
		void OnLoadProgress(const LoadProgress& _progress) override
		{
			++progressCount_;
			EXPECT_GE(_progress.loadedBytes_, lastLoadedBytes_);   // 단조 증가 불변식
			lastLoadedBytes_ = _progress.loadedBytes_;
			lastProgressState_ = _progress.state_;
		}
		void OnLoadCompleted(LoadResult& _result) override { ++completedCount_; order_.PushBack(eListener); lastHandle_ = _result.GetHandle(); }
		void OnLoadCancelled(const LoadResult& _result) override { ++cancelledCount_; lastError_ = _result.GetError(); }
		void OnLoadFailed(const LoadResult& _result) override { ++failedCount_; lastError_ = _result.GetError(); }

		enum NotifyOrder { eCallback = 1, eListener = 2 };

		int progressCount_ = 0, completedCount_ = 0, cancelledCount_ = 0, failedCount_ = 0;
		_s64 lastLoadedBytes_ = 0;
		LoadState lastProgressState_ = LoadState::lsNone;
		LoadError lastError_ = LoadError::leNone;
		LoadHandle lastHandle_ = InvalidLoadHandle;
		Vector<int> order_;   // 콜백(1) → 리스너(2) 순서 검증용 — 테스트에서 콜백이 eCallback을 PushBack
	};
}
