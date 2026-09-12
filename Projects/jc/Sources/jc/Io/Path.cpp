/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 9:13:03 AM
 * =====================
 *
 */

#include "jc/Memory.h"

#include "jc/IO/Path.h"
#include "jc/Primitives/StringConvert.h"

NS_JC_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::FileName(const String& _path)
{
	int slash = _path.FindReverse(_T("/"));
	int bslash = _path.FindReverse(_T("\\"));
	int pos = slash > bslash ? slash : bslash;
	if (pos < 0)
		return _path;
	return _path.SubStr(pos + 1, _path.Length() - pos - 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::FileName(const char* _pPath)
{
	return FileName(_pPath, (int)strlen(_pPath));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::FileNameWithoutExt(const String& _path)
{
	const String fileName = FileName(_path);
	int lastPeriodIndex = fileName.FindReverse(_T("."));

	if (lastPeriodIndex == -1)
	{
		return fileName;
	}

	return fileName.SubStr(0, lastPeriodIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::FileNameWithoutExt(const char* _pPath)
{
	const String fileName = FileName(_pPath);
	int lastPeriodIndex = fileName.FindReverse(_T("."));

	// 점이 없으면 전체 파일 이름 반환
	if (lastPeriodIndex == -1)
	{
		return fileName;
	}

	return fileName.SubStr(0, lastPeriodIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::FileName(const char* _pPath, int _length)
{
	AString fileName;
	bool slashFound = false;
	char* pPath = (char*)_pPath;

	for (int i = _length - 1; i >= 0; --i)
	{
		char& ch = pPath[i];

		if (ch == '\\' || ch == '/')
		{
			fileName += pPath + i + 1;
			slashFound = true;
			break;
		}
	}

	if (slashFound == false)
	{
		return StringConvert::FromAnsi(_pPath, _length);
	}

	return StringConvert::FromAnsi(fileName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::FileNameLevel(const String& _path, int _level)
{
	const int len = _path.Length();
	const _char* src = _path.Source();
	int currentLevel = 0;
	bool broke = false;
	int i = len - 1;
	for (; i >= 0; --i)
	{
		_char ch = src[i];
		if (ch == _T('\\') || ch == _T('/'))
		{
			if (currentLevel == _level)
			{
				broke = true;
				break;
			}
			int j = i - 1;
			while (j >= 0 && (src[j] == _T('\\') || src[j] == _T('/')))
			{
				--j;
			}
			i = j + 1;
			++currentLevel;
		}
	}
	int start = i + 1;
	if (start >= len)
		return String();
	// narrow판(FileNameLevel(char*))과 동일하게 슬래시 런을 '/' 하나로 정규화. '\\'도 '/'로 통일
	String ret;
	bool lastWasSlash = false;
	for (int k = start; k < len; ++k)
	{
		const _char ch = src[k];
		if (ch == _T('\\') || ch == _T('/'))
		{
			if (!lastWasSlash)
				ret += _T('/');
			lastWasSlash = true;
		}
		else
		{
			ret += ch;
			lastWasSlash = false;
		}
	}
	if (!broke && !ret.IsEmpty() && ret[0] == _T('/'))
	{
		if (ret.Length() <= 1)
			return String();
		ret = ret.SubStr(1, ret.Length() - 1);
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::FileNameLevel(const char* _pPath, int _level)
{
	return FileNameLevel(_pPath, (int)strlen(_pPath), _level);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void Path::FileNameLevel(char* _pBuf, int _bufCapacity, const char* _pPath, int _pathLen, int _level)
{
	int lastIndex = _bufCapacity - 1;

	char* pPath = (char*)_pPath;
	int currentLevel = 0;

	for (int i = _pathLen - 1; i >= 0; --i, --lastIndex)
	{
		char& ch = pPath[i];

		if (ch == '\\' || ch == '/')
		{
			_pBuf[lastIndex] = '/';

			if (currentLevel == _level)
			{
				break;
			}

			// 슬래쉬가 아닌 문자열을 만날때까지 체크
			int j = i - 1;
			while (j >= 0 && (pPath[j] == '\\' || pPath[j] == '/'))
			{
				--j;
			}

			i = j + 1;
			++currentLevel;
		}
		else
		{
			_pBuf[lastIndex] = ch;
		}
	}

	//  x x x x x a b c
	//          |
	//        iLast 다 읽고나면 빈 곳에 있기때문

	//  x x x x x a b c
	//            |
	//          iLast 이렇게 옮겨 줘야함

	lastIndex += 1; // 문자열이 있는 위치로 다시 옮겨 놓는다.

	// 레벨을 높게 잡아버린 경우 마지막 슬래쉬가 포함될 수가 있다.
	// //a///b//c/d에 레벨 10을 전달하면 /a/b/c/d 이렇게 됨 
	if (_pBuf[lastIndex] == '/')
	{
		++lastIndex;
	}

	int fileNameLength = _bufCapacity - lastIndex;
	Memory::Copy(_pBuf, _bufCapacity, _pBuf + lastIndex, fileNameLength);
	_pBuf[fileNameLength] = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::FileNameLevel(const char* _pPath, int _length, int _level)
{
	static constexpr int BUF_SIZE = 512;
	jc_assert_msg(_length < BUF_SIZE, "경로 길이는 버퍼 사이즈보다 작아야합니다.");

	char fileName[BUF_SIZE]{};
	FileNameLevel(fileName, BUF_SIZE, _pPath, _length, _level);

	return StringConvert::FromAnsi(fileName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::Combine(const String& _lhs, const String& _rhs)
{
	String combined{ _lhs.Source(), _lhs.Length() + _rhs.Length() + 1 };

	int lastIndex = -1;
	int count = 0;

	for (int i = combined.Length() - 1; i >= 0; --i, ++count)
	{
		if (combined[i] != _T('/') && combined[i] != _T('\\'))
		{
			lastIndex = i;
			break;
		}
	}

	if (lastIndex != -1)
	{
		combined.Clear(lastIndex + 1, count);
	}

	if (combined.Length() != 0)
	{
		combined.Append(_T('/'));
	}

	count = 0;

	for (lastIndex = 0; lastIndex < _rhs.Length(); ++lastIndex, ++count)
	{
		if (_rhs.GetAt(lastIndex) != _T('/') && _rhs.GetAt(lastIndex) != _T('\\'))
		{
			break;
		}
	}

	combined.Append(_rhs.Source() + count);
	return combined;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::Combine(const String& _lhs, const String& _rhs, const String& _khs)
{
	return Combine(_lhs, Combine(_rhs, _khs));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::Extension(const String& _path)
{
	int count = 0;

	for (int i = _path.Length() - 1; i >= 0; --i, ++count)
	{
		if (_path.GetAt(i) == _T('.'))
		{
			return { _path.Source() + i, count + 1 };
		}

		if (_path.GetAt(i) == _T('/') || _path.GetAt(i) == _T('\\'))
		{
			return _T("");
		}
	}

	return _T("");
}

NS_END
