#pragma once

/*
 * 작성자: 윤정도
 * =====================
 * 경로 해석기 (05 문서 정본 이식) — Io를 모르는 순수 클래스, 데몬 직속 멤버
 *   "res:/theme/dark.json" → 마운트 별칭   |   "theme/dark.json" → 디폴트 별칭
 *   "C:/tmp/a.bin" → 절대경로 통과         |   별칭당 폴백 체인(핫픽스 오버레이)
 *
 * 스레드 정책: 불변 스냅샷 교체(COW) — 읽기는 락 찰나(sp 복사), 해석은 락프리.
 * 재바인딩 중에도 진행 중 해석은 옛 스냅샷을 안전하게 계속 사용한다 (R11.5).
 */

#include "jc/Namespace.h"
#include "jc/Primitives/String.h"
#include "jc/Primitives/SmartPtr.h"
#include "jc/Primitives/Atomic.h"
#include "jc/Sync/NormalLock.h"
#include "jc/Container/HashMap.h"
#include "jc/Container/Vector.h"

NS_JC_BEGIN

enum PathResolveError
{
	preNone = 0,
	preEmptyPath,
	preUnknownAlias,
	preNotFound		// fullPath_에 주 후보 경로를 채워 반환 (로그/생성 판단용)
};

struct ResolveResult
{
	String fullPath_;			// 최종 절대경로
	String matchedDir_;			// 매칭된 베이스 (디버깅/로그용)
	String alias_;				// 사용된 별칭 (절대경로면 빈 값)
	PathResolveError error_ = preNone;

	bool IsOk() const { return error_ == preNone; }
};

class PathResolver
{
public:
	PathResolver();

	// ── 마운트 (별칭 이름공간) ─ _dir에 ${exe} ${cwd} ${다른별칭} 토큰 허용. 재호출 = 재바인딩(모드 전환)
	bool Mount(const String& _alias, const String& _dir);
	bool Unmount(const String& _alias);
	bool SetDefaultAlias(const String& _alias);

	// ── 폴백 체인 ─ priority 작을수록 먼저 검사. 주 디렉토리는 priority 0으로 자동 포함
	bool AddSearchDir(const String& _alias, const String& _dir, int _priority = 100);
	bool RemoveSearchDir(const String& _alias, const String& _dir);

	// ── 해석 (접수 시점 1회) ─ 요청 단위 오버라이드 오버로드 제공
	ResolveResult Resolve(const String& _path) const;
	ResolveResult Resolve(const String& _path, const String& _aliasOverride) const;
	ResolveResult ResolveWritable(const String& _path) const;	// 쓰기 대상: 존재검사 없이 주 디렉토리 확정

	String Dump() const;

	static bool IsAbsolutePath(const String& _path);

private:
	struct SearchDir
	{
		String dir_;
		int priority_ = 0;
	};
	struct AliasEntry
	{
		String primaryDir_;					// Mount 주 디렉토리 (쓰기 대상)
		Vector<SearchDir> searchDirs_;		// priority 오름차순 (주 디렉토리 포함)
	};
	struct Snapshot
	{
		HashMap<String, AliasEntry> aliases_;
		Vector<String> aliasOrder_;			// Dump 출력 순서 보존용
		String defaultAlias_;
	};
	using SnapshotPtr = SharedPtr<const Snapshot>;

	SnapshotPtr GetSnapshot() const;
	template <typename TMutator>
	bool MutateSnapshot(TMutator&& _fn);	// 복제 → 수정 → 원자적 교체

	static bool TryParseAlias(const String& _path, OUT String& _alias, OUT String& _rel);
	static String ExpandTokens(const Snapshot& _snap, const String& _dir);
	static void InsertSorted(Vector<SearchDir>& _dirs, const SearchDir& _sd);
	static String ExeDirectory();

	ResolveResult ResolveInternal(const String& _path, const String* _pAliasOverride, bool _checkExist) const;

private:
	mutable NormalLock lock_;
	SnapshotPtr spSnapshot_;
};

NS_END
