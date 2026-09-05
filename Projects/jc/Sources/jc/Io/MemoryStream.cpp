#include "jc/IO/MemoryStream.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
int MemoryStream::Read(OUT _u8* _pBytes, int _offset, int _len)
{
	jc_assert_msg(CanRead(), "읽기가 불가능한 스트림입니다.");
	jc_assert_msg(!closed_, "스트림이 닫혀 있습니다.");
	if (_pBytes == nullptr || _len <= 0)
		return 0;
	if (readOffset_ >= writeOffset_)
		return 0;

	_u32 readable = writeOffset_ - readOffset_;
	if ((_u32)_len < readable)
		readable = (_u32)_len;
	Memory::CopyUnsafe(_pBytes + _offset, pBytes_ + readOffset_, (int)readable);
	readOffset_ += readable;
	m_iOffset = (int)readOffset_;
	return (int)readable;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MemoryStream::Write(const _u8* _pBytes, int _offset, int _len)
{
	jc_assert_msg(CanWrite(), "쓰기가 불가능한 스트림입니다.");
	jc_assert_msg(!closed_, "스트림이 닫혀 있습니다.");
	if (_pBytes == nullptr || _len <= 0)
		return;
	ExpandAllocateIfNeeded(writeOffset_ + (_u32)_len);
	Memory::CopyUnsafe(pBytes_ + writeOffset_, _pBytes + _offset, _len);
	writeOffset_ += (_u32)_len;
	m_iLength = (int)writeOffset_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MemoryStream::Seek(int _offset, Origin _origin)
{
	jc_assert_msg(CanSeek(), "탐색이 불가능한 스트림입니다.");
	jc_assert_msg(!closed_, "스트림이 닫혀 있습니다.");

	int next = 0;
	switch (_origin)
	{
	case Origin::eBegin:
		next = _offset;
		break;
	case Origin::eCurrent:
		next = (int)readOffset_ + _offset;
		break;
	case Origin::eEnd:
		next = (int)writeOffset_ + _offset;
		break;
	}

	if (next < 0)
		next = 0;
	if (next > (int)writeOffset_)
		next = (int)writeOffset_;
	readOffset_ = (_u32)next;
	m_iOffset = next;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MemoryStream::Flush()
{
	return !closed_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MemoryStream::Close()
{
	closed_ = true;
}

NS_END
