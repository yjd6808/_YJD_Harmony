#include "jc/Io/PathResolver.h"
#include "jc/FileSystem/File.h"
#include "jc/FileSystem/Path.h"
#include "jc/Env.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
PathResolver::PathResolver()
{
	spSnapshot_ = MakeShared<Snapshot>();
}

// ─────────────────────────────── 스냅샷 (스레드 안전의 심장)

//////////////////////////////////////////////////////////////////////////////////////////
PathResolver::SnapshotPtr PathResolver::GetSnapshot() const
{
	LockGuard<NormalLock> guard(lock_);		// 찰나: 참조 복사만
	return spSnapshot_;
}

template <typename TMutator>
bool PathResolver::MutateSnapshot(TMutator&& _fn)
{
	LockGuard<NormalLock> guard(lock_);
	Snapshot* pNew = dbg_new Snapshot(*spSnapshot_);	// 통째 복제 (마운트 수 ≤ 십수 개)
	if (!_fn(*pNew))
	{
		delete pNew;
		return false;
	}
	spSnapshot_ = SnapshotPtr(pNew);			// 원자적 교체 — 진행 중 해석은 옛 스냅샷을 계속 사용
	return true;
}

// ─────────────────────────────── 마운트 / 체인 관리

//////////////////////////////////////////////////////////////////////////////////////////
bool PathResolver::Mount(const String& _alias, const String& _dir)
{
	if (_alias.Length() < 2)		// 한 글자 별칭 금지: "C:/" 드라이브 문자와 충돌
		return false;

	return MutateSnapshot([&](Snapshot& _snap)
	{
		String abs = ExpandTokens(_snap, _dir);
		if (!IsAbsolutePath(abs))
			abs = Path::Combine(Env::CurrentDirectory(), abs);

		AliasEntry* pExist = _snap.aliases_.Find(_alias);
		if (pExist != nullptr)
		{
			// 재바인딩: 주 디렉토리(priority 0)만 교체, 추가 서치 디렉토리는 유지
			pExist->primaryDir_ = abs;
			for (int i = 0; i < pExist->searchDirs_.Size(); ++i)
			{
				if (pExist->searchDirs_[i].priority_ == 0)
				{
					pExist->searchDirs_[i].dir_ = abs;
					return true;
				}
			}
			InsertSorted(pExist->searchDirs_, SearchDir{ abs, 0 });
			return true;
		}

		AliasEntry entry;
		entry.primaryDir_ = abs;
		entry.searchDirs_.PushBack(SearchDir{ abs, 0 });
		_snap.aliases_.Insert(_alias, entry);
		_snap.aliasOrder_.PushBack(_alias);
		return true;
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PathResolver::Unmount(const String& _alias)
{
	return MutateSnapshot([&](Snapshot& _snap)
	{
		if (_snap.aliases_.Find(_alias) == nullptr)
			return false;
		_snap.aliases_.Remove(_alias);
		for (int i = 0; i < _snap.aliasOrder_.Size(); ++i)
		{
			if (_snap.aliasOrder_[i] == _alias)
			{
				_snap.aliasOrder_.RemoveAt(i);
				break;
			}
		}
		if (_snap.defaultAlias_ == _alias)
			_snap.defaultAlias_ = String();
		return true;
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PathResolver::SetDefaultAlias(const String& _alias)
{
	return MutateSnapshot([&](Snapshot& _snap)
	{
		if (_snap.aliases_.Find(_alias) == nullptr)
			return false;					// 미등록 별칭은 거부
		_snap.defaultAlias_ = _alias;
		return true;
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PathResolver::AddSearchDir(const String& _alias, const String& _dir, int _priority)
{
	return MutateSnapshot([&](Snapshot& _snap)
	{
		AliasEntry* pEntry = _snap.aliases_.Find(_alias);
		if (pEntry == nullptr)
			return false;
		String abs = ExpandTokens(_snap, _dir);
		if (!IsAbsolutePath(abs))
			abs = Path::Combine(Env::CurrentDirectory(), abs);
		InsertSorted(pEntry->searchDirs_, SearchDir{ abs, _priority });
		return true;
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PathResolver::RemoveSearchDir(const String& _alias, const String& _dir)
{
	return MutateSnapshot([&](Snapshot& _snap)
	{
		AliasEntry* pEntry = _snap.aliases_.Find(_alias);
		if (pEntry == nullptr)
			return false;
		String abs = ExpandTokens(_snap, _dir);
		for (int i = 0; i < pEntry->searchDirs_.Size(); ++i)
		{
			// 주 디렉토리(priority 0)는 Unmount로만 제거
			if (pEntry->searchDirs_[i].dir_ == abs && pEntry->searchDirs_[i].priority_ != 0)
			{
				pEntry->searchDirs_.RemoveAt(i);
				return true;
			}
		}
		return false;
	});
}

// ─────────────────────────────── 해석

//////////////////////////////////////////////////////////////////////////////////////////
ResolveResult PathResolver::Resolve(const String& _path) const
{
	return ResolveInternal(_path, nullptr, true);
}

//////////////////////////////////////////////////////////////////////////////////////////
ResolveResult PathResolver::Resolve(const String& _path, const String& _aliasOverride) const
{
	return ResolveInternal(_path, &_aliasOverride, true);
}

//////////////////////////////////////////////////////////////////////////////////////////
ResolveResult PathResolver::ResolveWritable(const String& _path) const
{
	return ResolveInternal(_path, nullptr, false);
}

//////////////////////////////////////////////////////////////////////////////////////////
ResolveResult PathResolver::ResolveInternal(const String& _path, const String* _pAliasOverride, bool _checkExist) const
{
	ResolveResult result;

	if (_path.IsEmpty())
	{
		result.error_ = preEmptyPath;
		return result;
	}

	// 1. 절대경로는 무조건 통과 (오버라이드보다 우선, R11.6)
	if (IsAbsolutePath(_path))
	{
		result.fullPath_ = _path;
		return result;
	}

	SnapshotPtr spSnap = GetSnapshot();		// 이 시점 이후 락프리 — 도중 Mount 변경과 무관하게 일관

	// 2. 별칭 결정: 경로 내 명시("res:/...") > 요청 오버라이드 > 디폴트
	String alias;
	String rel;
	if (!TryParseAlias(_path, OUT alias, OUT rel))
	{
		alias = (_pAliasOverride != nullptr && !_pAliasOverride->IsEmpty())
			? *_pAliasOverride
			: spSnap->defaultAlias_;
		rel = _path;
	}

	const AliasEntry* pEntry = spSnap->aliases_.Find(alias);
	if (pEntry == nullptr)
	{
		result.alias_ = alias;
		result.error_ = preUnknownAlias;
		return result;
	}
	result.alias_ = alias;

	// 3. 쓰기 대상: 존재검사 없이 주 디렉토리로 확정 (다운로드 목적지 등)
	if (!_checkExist)
	{
		result.matchedDir_ = pEntry->primaryDir_;
		result.fullPath_ = Path::Combine(pEntry->primaryDir_, rel);
		return result;
	}

	// 4. 읽기: 체인 순회 — 첫 존재 파일 채택 (핫픽스 → 원본 순)
	for (int i = 0; i < pEntry->searchDirs_.Size(); ++i)
	{
		const SearchDir& sd = pEntry->searchDirs_[i];
		String candidate = Path::Combine(sd.dir_, rel);
		if (File::Exist(candidate))
		{
			result.matchedDir_ = sd.dir_;
			result.fullPath_ = candidate;
			return result;
		}
	}

	// 5. 미스: 에러 + 주 후보 경로 반환 → 호출부가 로그/생성 판단에 활용
	result.error_ = preNotFound;
	result.matchedDir_ = pEntry->primaryDir_;
	result.fullPath_ = Path::Combine(pEntry->primaryDir_, rel);
	return result;
}

// ─────────────────────────────── 파싱/유틸

//////////////////////////////////////////////////////////////////////////////////////////
bool PathResolver::IsAbsolutePath(const String& _path)
{
	if (_path.IsEmpty())
		return false;
	if (_path[0] == '/' || _path[0] == '\\')
		return true;
	// "C:" 드라이브: [영문자][':']
	if (_path.Length() >= 2 && _path[1] == ':')
	{
		const char c = _path[0];
		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
			return true;
	}
	return false;
}

// "res:/theme/dark.json" → alias="res", rel="theme/dark.json"
// 규칙: ':' 위치가 2 이상(두 글자 이상 별칭)이고 다음 문자는 경로 구분자.
//       ':' 위치 1이면 드라이브 문자 → 별칭 아님(IsAbsolutePath가 먼저 걸러줌)
//////////////////////////////////////////////////////////////////////////////////////////
bool PathResolver::TryParseAlias(const String& _path, OUT String& _alias, OUT String& _rel)
{
	const int colon = _path.Find(":");
	if (colon < 2)
		return false;
	if (colon + 1 >= _path.Length())
		return false;
	if (_path[colon + 1] != '/' && _path[colon + 1] != '\\')
		return false;

	for (int i = 0; i < colon; ++i)		// 별칭부에 경로 문자가 섞이면 별칭이 아님
	{
		if (_path[i] == '/' || _path[i] == '\\' || _path[i] == '.')
			return false;
	}

	_alias = _path.SubStr(0, colon);
	int relStart = colon + 1;
	while (relStart < _path.Length() && (_path[relStart] == '/' || _path[relStart] == '\\'))
		++relStart;						// "res://a" 같은 중복 구분자 허용
	_rel = _path.SubStr(relStart, _path.Length() - relStart);
	return true;
}

// "${asset}/res_data" → 등록된 별칭의 주 디렉토리 치환. 내장 토큰: ${exe} ${cwd}
//////////////////////////////////////////////////////////////////////////////////////////
String PathResolver::ExpandTokens(const Snapshot& _snap, const String& _dir)
{
	String result = _dir;
	for (int guardCount = 0; guardCount < 8; ++guardCount)		// 순환 참조 안전장치
	{
		const int start = result.Find("${");
		if (start == -1)
			break;
		const int end = result.Find(start + 2, "}");
		if (end == -1)
			break;

		const String token = result.SubStr(start + 2, end - start - 2);
		String value;
		if (token == "exe")
			value = ExeDirectory();
		else if (token == "cwd")
			value = Env::CurrentDirectory();
		else if (const AliasEntry* pEntry = _snap.aliases_.Find(token))
			value = pEntry->primaryDir_;
		// 미등록 토큰은 빈 값 치환

		result = result.SubStr(0, start) + value + result.SubStr(end + 1, result.Length() - end - 1);
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PathResolver::InsertSorted(Vector<SearchDir>& _dirs, const SearchDir& _sd)
{
	_dirs.PushBack(_sd);
	for (int i = _dirs.Size() - 1; i > 0; --i)		// 안정 삽입: 동순위는 등록 순 유지
	{
		if (_dirs[i - 1].priority_ <= _dirs[i].priority_)
			break;
		SearchDir temp = _dirs[i - 1];
		_dirs[i - 1] = _dirs[i];
		_dirs[i] = temp;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
String PathResolver::ExeDirectory()
{
	const String& modulePath = Env::ModulePath();	// a/b/c/d.exe
	String fileName = Path::FileName(modulePath);
	String dir = modulePath.SubStr(0, modulePath.Length() - fileName.Length());
	while (dir.Length() > 0 && (dir.Last() == '/' || dir.Last() == '\\'))
		dir = dir.SubStr(0, dir.Length() - 1);
	return dir;
}

//////////////////////////////////////////////////////////////////////////////////////////
String PathResolver::Dump() const
{
	SnapshotPtr spSnap = GetSnapshot();
	String out;
	out += "default: ";
	out += spSnap->defaultAlias_;
	out += "\n";
	for (int i = 0; i < spSnap->aliasOrder_.Size(); ++i)
	{
		const AliasEntry* pEntry = spSnap->aliases_.Find(spSnap->aliasOrder_[i]);
		if (pEntry == nullptr)
			continue;
		out += "[";
		out += spSnap->aliasOrder_[i];
		out += "] -> ";
		out += pEntry->primaryDir_;
		out += "\n";
		for (int j = 0; j < pEntry->searchDirs_.Size(); ++j)
		{
			out += "    (";
			char buf[16] = {};
			_snprintf_s(buf, sizeof(buf), "%d", pEntry->searchDirs_[j].priority_);
			out += buf;
			out += ") ";
			out += pEntry->searchDirs_[j].dir_;
			out += "\n";
		}
	}
	return out;
}

NS_END
