/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 9:13:03 AM
 * =====================
 *
 */

#include <jc/Core.h>
#include <jc/Memory.h>

#include <jc/FileSystem/Path.h>

NS_JC_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::FileName(const String& _path)
{
	return FileName(_path.Source(), _path.Length());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::FileName(const char* _pPath)
{
	return FileName(_pPath, (int)strlen(_pPath));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::FileNameWithoutExt(const String& _path)
{
	return FileNameWithoutExt(_path.Source());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::FileNameWithoutExt(const char* _pPath)
{
	const String fileName = FileName(_pPath);
	int lastPeriodIndex = fileName.FindReverse(".");

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
	String fileName;
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
		return _pPath;
	}

	return fileName;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::FileNameLevel(const String& _path, int _level)
{
	return FileNameLevel(_path.Source(), _path.Length(), _level);
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

	return fileName;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
String Path::Combine(const String& _lhs, const String& _rhs)
{
	String combined{ _lhs.Source(), _lhs.Length() + _rhs.Length() + 1 };

	int lastIndex = -1;
	int count = 0;

	for (int i = combined.Length() - 1; i >= 0; --i, ++count)
	{
		if (combined[i] != '/' && combined[i] != '\\')
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
		combined.Append('/');
	}

	count = 0;

	for (lastIndex = 0; lastIndex < _rhs.Length(); ++lastIndex, ++count)
	{
		if (_rhs.GetAt(lastIndex) != '/' && _rhs.GetAt(lastIndex) != '\\')
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
		if (_path.GetAt(i) == '.')
		{
			return { _path.Source() + i, count + 1 };
		}

		if (_path.GetAt(i) == '/' || _path.GetAt(i) == '\\')
		{
			return "";
		}
	}

	return "";
}

NS_END
