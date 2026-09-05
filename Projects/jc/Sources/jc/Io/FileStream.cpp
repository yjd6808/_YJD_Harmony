/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 5:00:00 AM
 * =====================
 *
 */



#include "jc/Exception.h"
#include "jc/Primitives/String.h"

#include "jc/IO/FileStream.h"
#include "jc/IO/File.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
FileStream::FileStream(const String& _path, FileAccess _access, FileMode _mode)
	: m_eAccess(_access)
	, m_eMode(_mode)
	, m_hHandle(nullptr)
{
	const char* pPath = _path.Source();

	switch (_access)
	{
	case FileAccess::eRead:
	{
		m_hHandle = CRuntime::FileOpen(pPath, "rb");

		if (m_hHandle == nullptr)
		{
			throw RuntimeException("파일을 Read 모드로 여는데 실패하였습니다.");
		}

		if (CRuntime::FileSeekEnd(m_hHandle, 0))
		{
			m_eMode = FileMode::eOpen;
			m_bCanRead = true;
			m_iLength = CRuntime::FileTell(m_hHandle);
			CRuntime::FileSeekBegin(m_hHandle, 0);
		}
	}
	break;

	case FileAccess::eWrite:
	{
		switch (_mode)
		{
		case FileMode::eAppend:
			m_hHandle = CRuntime::FileOpen(pPath, "ab");
			if (m_hHandle == nullptr)
			{
				throw RuntimeException("파일을 Write/Append 모드로 여는데 실패하였습니다.");
			}
			m_iLength = CRuntime::FileTell(m_hHandle);
			m_iOffset = m_iLength;
			break;

		case FileMode::eOpen:
			if (!File::Exist(pPath))
			{
				throw RuntimeException("파일을 Write/Open 모드로 여는데 실패하였습니다. (파일이 없음)");
			}
			m_hHandle = CRuntime::FileOpen(pPath, "wb");
			if (m_hHandle == nullptr)
			{
				throw RuntimeException("파일을 Write/Open 모드로 여는데 실패하였습니다.");
			}
			m_iLength = CRuntime::FileTell(m_hHandle);
			break;

		case FileMode::eCreate:
			m_hHandle = CRuntime::FileOpen(pPath, "wb");
			if (m_hHandle == nullptr)
			{
				throw RuntimeException("파일을 Write/Create 모드로 여는데 실패하였습니다.");
			}
			break;
		}

		m_bCanWrite = true;
	}
	break;

	case FileAccess::eReadWrite:
	{
		switch (_mode)
		{
		case FileMode::eAppend:
			m_iLength = File::Size(pPath); // append/read 모드로 열면 한번이라도 write 하기전에는 옵셋이 0임, 따라서 위치를 수동으로 구해줘야한다.
			if (m_iLength == -1)
			{
				throw RuntimeException("파일을 ReadWrite/Append 모드로 여는데 실패하였습니다.");
			}
			m_hHandle = CRuntime::FileOpen(pPath, "ab+");
			if (m_hHandle == nullptr)
			{
				throw RuntimeException("파일을 ReadWrite/Append 모드로 여는데 실패하였습니다.");
			}
			m_iOffset = m_iLength;
			break;

		case FileMode::eOpen:
			m_iLength = File::Size(pPath); // read/write 모드로 열면 옵셋이 초기 0이므로 길이를 수동으로 구해줘야한다.
			if (m_iLength == -1)
			{
				throw RuntimeException("파일을 ReadWrite/Open 모드로 여는데 실패하였습니다.");
			}
			m_hHandle = CRuntime::FileOpen(pPath, "rb+");
			if (m_hHandle == nullptr)
			{
				throw RuntimeException("파일을 ReadWrite/Open 모드로 여는데 실패하였습니다.");
			}
			break;

		case FileMode::eCreate:
			m_hHandle = CRuntime::FileOpen(pPath, "wb+");
			if (m_hHandle == nullptr)
			{
				throw RuntimeException("파일을 ReadWrite/Create 모드로 여는데 실패하였습니다.");
			}
			break;
		}

		m_bCanWrite = true;
		m_bCanRead = true;
	}
	break;
	}

	m_bCanSeek = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
FileStream::~FileStream()
{
	FileStream::Close();
}

//////////////////////////////////////////////////////////////////////////////////////////
int FileStream::Read(OUT _u8* _pBytes, int _offset, int _len)
{
	jc_assert_msg(CanRead(), "읽기가 불가능한 스트림입니다.");
	jc_assert_msg(m_hHandle, "스트림이 닫혀 있습니다.");

	int readCount = (int)CRuntime::FileRead(_pBytes + _offset, 1, _len, m_hHandle);

	m_iOffset += readCount;

	if (m_iOffset > m_iLength)
	{
		m_iOffset = m_iLength;
	}

	jc_assert(CRuntime::FileTell(m_hHandle) == m_iOffset);
	return readCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
void FileStream::Write(const _u8* _pBytes, int _offset, int _len)
{
	jc_assert_msg(CanWrite(), "쓰기가 불가능한 스트림입니다.");
	jc_assert_msg(m_hHandle, "스트림이 닫혀 있습니다.");

	int writeCount = (int)CRuntime::FileWrite(_pBytes + _offset, 1, _len, m_hHandle);
	SetOffset(m_iOffset += writeCount);

	jc_assert(CRuntime::FileTell(m_hHandle) == m_iOffset);
}

//////////////////////////////////////////////////////////////////////////////////////////
void FileStream::Seek(int _offset, Origin _origin)
{
	jc_assert_msg(CanSeek(), "읽기가 불가능한 스트림입니다.");
	jc_assert_msg(m_hHandle, "스트림이 닫혀 있습니다.");

	switch (_origin)
	{
	case Origin::eBegin:
		if (_offset >= m_iLength)
		{
			CRuntime::FileSeekEnd(m_hHandle, 0);
			_offset = m_iLength;
		}
		else
		{
			CRuntime::FileSeekBegin(m_hHandle, _offset);
		}
		m_iOffset = _offset;
		break;

	case Origin::eCurrent:
		CRuntime::FileSeekCur(m_hHandle, _offset);
		m_iOffset += _offset;
		break;

	case Origin::eEnd:
		CRuntime::FileSeekEnd(m_hHandle, _offset);
		m_iOffset -= _offset;
		break;
	}

	jc_assert(CRuntime::FileTell(m_hHandle) == m_iOffset);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool FileStream::Flush()
{
	return CRuntime::FileFlush(m_hHandle);
}

//////////////////////////////////////////////////////////////////////////////////////////
void FileStream::Close()
{
	if (m_hHandle)
	{
		CRuntime::FileClose(m_hHandle);
		m_hHandle = nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool FileStream::IsClosed()
{
	return m_hHandle == nullptr;
}

NS_END
