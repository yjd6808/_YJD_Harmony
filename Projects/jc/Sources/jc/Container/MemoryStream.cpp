/*
 * 작성자: 윤정도
 * 생성일: 5/19/2024 9:18:17 AM
 * =====================
 *
 */

#include <jc/Core.h>
#include "MemoryStream.h"

//MemoryStream MemoryStream::Null;
//
////////////////////////////////////////////////////////////////////////////////////////////
//MemoryStream::MemoryStream()
//: readOffset_(0)
//, writeOffset_(0)
//, len_(0)
//, pBytes_(nullptr)
//, owner_(true)
//{}
//
////////////////////////////////////////////////////////////////////////////////////////////
//MemoryStream::MemoryStream(_byte* _pBytes, _u32 _len, bool _owner /* = true */)
//: readOffset_(0)
//, writeOffset_(0)
//, len_(_len)
//, pBytes_(_pBytes)
//, owner_(_owner)
//{
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//MemoryStream::MemoryStream(_ptr _pAlloc, _u32 _len, Process _memOwner, bool _owner /*= true*/)
//: readOffset_(0)
//, writeOffset_(0)
//, len_(_len)
//, pBytes_((_byte*)_pAlloc)
//, owner_(_owner)
//, process_(std::make_shared<Process>(_memOwner.GetId()))
//{
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//MemoryStream::MemoryStream(SpanByte _span, bool _owner)
//: readOffset_(0)
//, writeOffset_(0)
//, len_(_span.len_)
//, pBytes_(_span.pArr_)
//, owner_(_owner)
//{
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//MemoryStream::MemoryStream(const MemoryStream& _other)
//: MemoryStream()
//{
//	this->operator=(_other);
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//MemoryStream::MemoryStream(MemoryStream&& _other) noexcept
// : MemoryStream()
//{
//	this->operator=(std::move(_other));
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//MemoryStream::~MemoryStream()
//{
//	Free();
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//MemoryStream& MemoryStream::operator=(const MemoryStream& _other)
//{
//	len_ = _other.len_;
//	readOffset_ = _other.readOffset_;
//	writeOffset_ = _other.writeOffset_;
//	process_ = _other.process_;
//	if (process_)
//	{
//		owner_ = false; // 일반적으로 프로세스 ownership 이전은 안하는게 맞지	
//	}
//	else
//	{
//		pBytes_ = new _byte[len_];
//		std::memcpy(pBytes_, _other.pBytes_, len_);
//	}
//	return *this;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//MemoryStream& MemoryStream::operator=(MemoryStream&& _other) noexcept
//{
//	Free();
//
//	len_ = _other.len_;
//	readOffset_ = _other.readOffset_;
//	writeOffset_ = _other.writeOffset_;
//	process_ = _other.process_;
//	owner_ = _other.owner_; 
//	pBytes_ = _other.pBytes_;
//
//	_other.len_ = 0;
//	_other.readOffset_ = 0;
//	_other.writeOffset_ = 0;
//	_other.owner_ = true;
//	_other.process_ = nullptr;
//	_other.pBytes_ = nullptr;
//	return *this;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_byte& MemoryStream::operator[](_u32 _offset)
//{
//	RIC_ASSERT(_offset < len_);
//	return pBytes_[_offset];
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::Free()
//{
//	if (process_)
//	{
//		if (owner_)
//		{
//			process_->MemFree((_ptr)pBytes_);
//		}
//	}
//	else
//	{
//		if (owner_)
//		{
//			RIC_ASSERT(process_ == nullptr);
//			RIC_DELETE_ARRAY(pBytes_);
//		}
//	}
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::ExpandAllocateIfNeeded(_u32 _minimumSize /*= 0*/)
//{
//	if (process_)
//		return;
//
//	if (!IsOwner())
//		return;
//
//	if (IsNull())
//	{
//		int bufSize = _minimumSize > RIC_DEFAULT_BUFF_SIZE ? _minimumSize : RIC_DEFAULT_BUFF_SIZE;
//		pBytes_ = new _byte[bufSize];
//		len_ = bufSize;
//	}
//	else if (GetWriteableSize() < (int)_minimumSize)
//	{
//		_u32 newLen = len_ << 1;
//		RIC_ASSERT(newLen > len_);
//		_byte* pNew = new _byte[newLen];
//		Memory::CopyUnsafe(pNew, pBytes_, (int)len_);
//		RIC_DELETE_ARRAY(pBytes_);
//		pBytes_ = pNew;
//		len_ = newLen;
//	}
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::SetData(_byte* _pBytes, _u32 _len)
//{
//	*this = { _pBytes, _len };
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::SetWriteOffset(int _offset, Origin _origin)
//{
//	switch (_origin)
//	{
//	case moBegin:
//		{
//			writeOffset_ = _offset;
//
//			if (readOffset_ > writeOffset_)
//			{
//				readOffset_ = writeOffset_;
//			}
//			break;
//		}
//	default:
//		// 딴건 귀찮아서 구현안함. 필요할일 생기면 구현
//		RIC_ASSERT(false);
//	}
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::SetReadOffset(int _offset, Origin _origin)
//{
//	int tmp = readOffset_;
//
//	switch (_origin)
//	{
//	case moBegin:
//	{
//		readOffset_ = _offset;
//
//		if (readOffset_ > writeOffset_)
//		{
//			RIC_ASSERT(false);
//			readOffset_ = tmp;
//		}
//		break;
//	}
//	default:
//		// 딴건 귀찮아서 구현안함. 필요할일 생기면 구현
//		RIC_ASSERT(false);
//	}
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::Reset()
//{
//	readOffset_ = 0;
//	writeOffset_ = 0;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::PopReads()
//{
//	Memory::Copy(pBytes_, (int)len_, pBytes_ + readOffset_, writeOffset_ - readOffset_);
//	writeOffset_ -= readOffset_;
//	readOffset_ = 0;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_byte MemoryStream::GetAt(_u32 _offset) const
//{
//	RIC_ASSERT(_offset < len_);
//	return pBytes_[_offset];
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_u32 MemoryStream::GetNetworkMessageId(bool _includeStreamLen)
//{
//	int expectedReadableSize = 4;
//
//	if (_includeStreamLen)
//	{
//		expectedReadableSize = 8;
//	}
//
//	if (GetReadaleSize() >= expectedReadableSize)
//	{
//		if (_includeStreamLen)
//		{
//			ReadU32();
//		}
//		_u32 msgId = ReadU32();
//		SetReadOffset(0);
//		return msgId;
//	}
//	return 0;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//bool MemoryStream::CanReadNetworkMessage()
//{
//	int readableSize = GetReadaleSize();
//
//	// 메시지 길이 만큼 읽을 수 있는지 검사
//	if (readableSize > (int)sizeof(_s32) && readableSize >= PeekS32())
//	{
//		return true;
//	}
//
//	return false;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//int MemoryStream::PeekS32()
//{
//	_s32 value;
//	Peek((_byte*)&value, 0, sizeof(_s32));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_u32 MemoryStream::PeekU32()
//{
//	_u32 value;
//	Peek((_byte*)&value, 0, sizeof(_u32));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//int& MemoryStream::PeekS32Ref()
//{
//	int* value;
//	PeekRef((_byte**)&value, 0, sizeof(_s32));
//	return *value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_u32& MemoryStream::PeekU32Ref()
//{
//	_u32* value;
//	PeekRef((_byte**)&value, 0, sizeof(_u32));
//	return *value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::Peek(OUT _byte* _pBytes, _u32 _offset, _u32 _len)
//{
//	if (IsNull())
//	{
//		RIC_ASSERT(false);
//		return;
//	}
//
//	if (readOffset_ + (int)_len > writeOffset_)
//	{
//		RIC_ASSERT(false);
//		return;
//	}
//
//	if (process_)
//	{
//		process_->MemRead((_ptr)pBytes_ + readOffset_, _pBytes + _offset, _len);
//	}
//	else
//	{
//		std::memcpy(_pBytes + _offset, pBytes_ + readOffset_, _len);
//	}
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::PeekRef(OUT _byte** _ppBytes, _u32 _offset, _u32 _len)
//{
//	if (IsNull())
//	{
//		RIC_ASSERT(false);
//		return;
//	}
//
//	if (readOffset_ + (int)_len > writeOffset_)
//	{
//		RIC_ASSERT(false);
//		return;
//	}
//
//	if (process_)
//	{
//		RIC_THROW_MSG(false, _T("프로세스 모드에서는 지원하지 않는 기능입니다."));
//	}
//	else
//	{
//		*_ppBytes = pBytes_ + readOffset_;
//	}
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_string MemoryStream::ReadString()
//{
//	if (IsNull())
//	{
//		RIC_ASSERT(false);
//		return _T("");
//	}
//
//	_u32 len = ReadU32();
//	_char* tmp = new _char[len + 1];
//	Read((_byte*)tmp, 0, len * sizeof(_string::value_type));
//	tmp[len] = NULL;
//	_string s = tmp;
//	RIC_DELETE_ARRAY(tmp);
//	return s;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::ReadStringArray(OUT _char* _pBuf, int _capacity)
//{
//	_u32 len = ReadU32();
//	RIC_THROW_MSG(_capacity >= (int)len + 1, _T("스트림에서 읽고자하는 문자열의 버퍼 크기가 작습니다."));
//	Read((_byte*)_pBuf, 0, len * sizeof(_string::value_type));
//	_pBuf[len] = NULL;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_byte MemoryStream::ReadByte()
//{
//	if (IsNull())
//	{
//		RIC_ASSERT(false);
//		return 0;
//	}
//
//	_byte value;
//	Read((_byte*)&value, 0, sizeof(_byte));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::ReadBytes(byte* _pBuf, int _count)
//{
//	Read(_pBuf, 0, _count);
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//bool MemoryStream::ReadBool()
//{
//	if (IsNull())
//	{
//		RIC_ASSERT(false);
//		return 0;
//	}
//
//	bool value;
//	Read((_byte*)&value, 0, sizeof(bool));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_char MemoryStream::ReadChar()
//{
//	if (IsNull())
//	{
//		RIC_ASSERT(false);
//		return 0;
//	}
//
//	_char value;
//	Read((_byte*)&value, 0, sizeof(_char));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//wchar_t MemoryStream::ReadWChar()
//{
//	if (IsNull())
//	{
//		RIC_ASSERT(false);
//		return 0;
//	}
//
//	wchar_t value;
//	Read((_byte*)&value, 0, sizeof(wchar_t));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//char MemoryStream::ReadMChar()
//{
//	if (IsNull())
//	{
//		RIC_ASSERT(false);
//		return 0;
//	}
//
//	char value;
//	Read((_byte*)&value, 0, sizeof(char));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_ptr MemoryStream::ReadPtr()
//{
//	_ptr value;
//	Read((_byte*)&value, 0, sizeof(_ptr));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//float MemoryStream::ReadFloat()
//{
//	float value;
//	Read((_byte*)&value, 0, sizeof(float));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_s8 MemoryStream::ReadS8()
//{
//	_s8 value;
//	Read((_byte*)&value, 0, sizeof(_s8));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_s16 MemoryStream::ReadS16()
//{
//	_s16 value;
//	Read((_byte*)&value, 0, sizeof(_s16));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//int MemoryStream::ReadS32()
//{
//	int value;
//	Read((_byte*)&value, 0, sizeof(int));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_s64 MemoryStream::ReadS64()
//{
//	_s64 value;
//	Read((_byte*)&value, 0, sizeof(_s64));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_u8 MemoryStream::ReadU8()
//{
//	_u8 value;
//	Read((_byte*)&value, 0, sizeof(_u8));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_u16 MemoryStream::ReadU16()
//{
//	_u16 value;
//	Read((_byte*)&value, 0, sizeof(_u16));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_u32 MemoryStream::ReadU32(bool _pop /*= false*/)
//{
//	if (IsNull())
//	{
//		RIC_ASSERT(false);
//		return 0;
//	}
//
//	_u32 value;
//	Read((_byte*)&value, 0, sizeof(_u32));
//
//	if (_pop)
//	{
//		PopReads();
//	}
//
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_u64 MemoryStream::ReadU64()
//{
//	if (IsNull())
//	{
//		RIC_ASSERT(false);
//		return 0;
//	}
//
//	_u64 value;
//	Read((_byte*)&value, 0, sizeof(_u64));
//	return value;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::Read(OUT _byte* _pBytes, _u32 _offset, _u32 _len)
//{
//	Peek(_pBytes, _offset, _len);
//	readOffset_ += (int)_len;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::ReadStream(OUT MemoryStream& _stream, int _len)
//{
//	if (_stream.IsTargetProcess())
//	{
//		RIC_ASSERT(false);	// not implemented
//		return;
//	}
//
//	bool isAllocated = false;
//	_byte* pBuf = nullptr;
//	if (_stream.IsNull())
//	{
//		pBuf = new _byte[_len];
//		isAllocated = true;
//		_stream.SetData(pBuf, _len);
//	}
//	else
//	{
//		pBuf = _stream.DataFromWriteOffset();
//	}
//
//	int writeableSize = _stream.GetWriteableSize();
//	if (writeableSize < _len)
//	{
//		if (isAllocated)
//		{
//			RIC_DELETE_ARRAY(pBuf);
//			_stream = {};
//		}
//		RIC_ASSERT(false);
//		return;
//	}
//	
//	Read(pBuf, 0, _len);
//	_stream.SetWriteOffset(_len);
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//bool MemoryStream::ReadNetworkMessage(OUT MemoryStream& _stream, OUT int& _readBytes, bool _readOnly)
//{
//	if (IsNull())
//	{
//		RIC_ASSERT(false);
//		return false;
//	}
//
//	int streamLength = ReadS32();
//	if (_readOnly)
//	{
//		_stream.SetData(DataFromReadOffset(), streamLength);
//		_stream.SetWriteOffset(streamLength);
//		_stream.SetOwner(false);
//		SetReadOffset(readOffset_ + streamLength);
//	}
//	else
//	{
//		ReadStream(_stream, streamLength);
//	}
//	_readBytes = streamLength;
//	return true;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::Write(const MemoryStream& _value)
//{
//	int len = _value.GetReadaleSize();
//	
//	if (_value.process_)
//	{
//		_byte* pBuf = new _byte[len];
//		MemoryStream& stream = const_cast<MemoryStream&>(_value);
//		int prevReadOffset = stream.GetReadOffset();
//		stream.Read(pBuf, 0, len);
//		stream.SetReadOffset(prevReadOffset);
//		WriteInternal(_value.DataFromReadOffset(), len);
//		delete[] pBuf;
//	}
//	else
//	{
//		ExpandAllocateIfNeeded(len);
//		WriteInternal(_value.DataFromReadOffset(), len);
//	}
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::WriteString(const _string& _value)
//{
//	ExpandAllocateIfNeeded((_u32)_value.length() * sizeof(_string::value_type));
//	WriteU32((_u32)_value.length());
//	WriteInternal((_byte*)_value.data(), (_u32)_value.length() * sizeof(_string::value_type));
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::WriteString(const _char* _value)
//{
//	if (_value == nullptr) return;
//	int len = StringUtil::Length(_value);
//	if (len <= 0) return;
//	WriteString(_value, len);
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::WriteString(const _char* _value, int _len)
//{
//	ExpandAllocateIfNeeded((_u32)_len * sizeof(_string::value_type));
//	WriteU32((_u32)_len);
//	WriteInternal((_byte*)_value, (_u32)_len * sizeof(_string::value_type));
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::WriteByte(_byte _value)
//{
//	ExpandAllocateIfNeeded(sizeof(_byte));
//	WriteInternal((_byte*)&_value, sizeof(_byte));
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::WriteBool(bool _value)
//{
//	ExpandAllocateIfNeeded(sizeof(bool));
//	WriteInternal((_byte*)&_value, sizeof(bool));
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::WriteChar(_char _value)
//{
//	ExpandAllocateIfNeeded(sizeof(_char));
//	WriteInternal((_byte*)&_value, sizeof(_char));
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::WriteU32(_u32 _value)
//{
//	ExpandAllocateIfNeeded(sizeof(_u32));
//	WriteInternal((_byte*)&_value, sizeof(_u32));
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::WriteU32(_u32 _value, OUT _u32** _ptr)
//{
//	ExpandAllocateIfNeeded(sizeof(_u32));
//	if (WriteInternal((_byte*)&_value, sizeof(_u32)))
//	{
//		*_ptr = (_u32*)(pBytes_ + writeOffset_ - sizeof(_u32));
//	}
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::WriteS32(int _value)
//{
//	ExpandAllocateIfNeeded(sizeof(int));
//	WriteInternal((_byte*)&_value, sizeof(int));
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::WriteS64(_s64 _value)
//{
//	ExpandAllocateIfNeeded(sizeof(_s64));
//	WriteInternal((_byte*)&_value, sizeof(_s64));
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::WriteU64(_u64 _value)
//{
//	ExpandAllocateIfNeeded(sizeof(_u64));
//	WriteInternal((_byte*)&_value, sizeof(_u64));
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::WritePtr(_ptr _value)
//{
//	ExpandAllocateIfNeeded(sizeof(_ptr));
//	WriteInternal((_byte*)&_value, sizeof(_ptr));
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::Write(const _byte* _pBytes, _u32 _len)
//{
//	ExpandAllocateIfNeeded(_len);
//	WriteInternal(_pBytes, _len);
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//bool MemoryStream::WriteInternal(const _byte* _pBytes, _u32 _len)
//{
//	if (writeOffset_ + _len > len_)
//	{
//		RIC_ASSERT(false);
//		return false;
//	}
//
//	bool ret = true;
//	if (process_)
//	{
//		ret = process_->MemWrite((_ptr)pBytes_ + writeOffset_, _pBytes, _len);
//		RIC_ASSERT(ret); // 프로세스 쓰기 실패
//	}
//	else
//	{
//		std::memcpy(pBytes_ + writeOffset_, _pBytes, (size_t)_len);
//	}
//	
//	writeOffset_ += (int)_len;
//	return ret;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::PlugNetworkMessage(_u32 _cmdId)
//{
//	int msgSize = GetReadaleSize();
//	RIC_THROW_MSG(msgSize >= 8, _T("올바르지 않은 네트워크 메시지 입니다."));
//	int writeOffset = GetWriteOffset();
//	SetWriteOffset(0);
//	SetReadOffset(0);
//	WriteS32(msgSize - (int)sizeof(_s32));
//	WriteU32(_cmdId);
//	SetWriteOffset(writeOffset);
//
//	//SetReadOffset(0);
//	//PeekS32Ref() = msgSize - (int)sizeof(_s32);	// 메시지 사이즈 정보 크기는 뺌
//	//SetReadOffset(sizeof(_s32));
//	//PeekU32Ref() = _cmdId;
//	//SetReadOffset(0);
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//_string MemoryStream::DumpHexText(
//	_u32 _byteGroupSize /*= 1*/, 
//	_u32 _byteGroupCountInSingleLine /*= 8*/, 
//	_string _join /*= _string()*/, 
//	bool _0x /*= false */)
//{
//	static _char HEX[16] {
//		_T('0'), _T('1'), _T('2'), _T('3'),
//		_T('4'), _T('5'), _T('6'), _T('7'),
//		_T('8'), _T('9'), _T('A'), _T('B'),
//		_T('C'), _T('D'), _T('E'), _T('F')
//	};
//	int msgSize = GetReadaleSize();
//	if (msgSize <= 0)
//	{
//		return _string();
//	}
//
//	_u32 byteGroupCount = 0;
//	int byteGroupSize = RIC_CLAMP(_byteGroupSize, 1, 8);
//	_byte* bin = new _byte[msgSize];
//	Peek(bin, 0, msgSize);
//
//	_string str;
//	str.reserve(((int)_join.size() + 2 + byteGroupSize) * msgSize + 5);
//	for (int i = 0; i < msgSize; i += byteGroupSize, byteGroupCount++)
//	{
//		if (i > 0)
//		{
//			str.append(_join);
//		}
//
//		if (_0x)
//		{
//			str.append(_T("0x"));
//		}
//
//		for (int j = 0; j < byteGroupSize && (i + j) < msgSize; ++j)
//		{
//			_byte byteValue = bin[i + j];
//			str.push_back(HEX[(byteValue >> 4) & 0xF]); // 상위 4비트
//			str.push_back(HEX[byteValue & 0xF]);       // 하위 4비트
//		}
//
//		if (byteGroupCount != 0 && (byteGroupCount % _byteGroupCountInSingleLine) == 0)
//		{
//			str.append(_T("\n"));
//		}
//	}
//
//	delete[] bin;
//	return str;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//std::shared_ptr<MemoryStream> MemoryStream::CreateNetworkMessage()
//{
//	MemoryStreamPtr pStream = std::make_shared<MemoryStream>();
//	pStream->WriteS32(0);		// cmd length
//	pStream->WriteS32(0);		// cmd id
//	return pStream;
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::WriteJmpRip(_ptr _instructionAddr, _ptr _dstAddr)
//{
//#if _WIN64
//	_s64 src = (_s64)_instructionAddr;
//	_s64 dst = (_s64)_dstAddr;
//
//	ExpandAllocateIfNeeded(6);
//	WriteByte(0xFF);
//	WriteByte(0x25);
//	WriteS32(static_cast<int>(dst - src));
//#else
//	static_assert(false, "come on...");
//#endif
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::WriteJmpR15()
//{
//#if _WIN64
//	ExpandAllocateIfNeeded(3);
//	WriteByte(0x41);
//	WriteByte(0xFF);
//	WriteByte(0x17);
//#else
//	static_assert(false, "come on...");
//#endif
//}
//
////////////////////////////////////////////////////////////////////////////////////////////
//void MemoryStream::WriteCallRip(_ptr _instructionAddr, _ptr _dstAddr)
//{
//#if _WIN64
//	_s64 src = (_s64)_instructionAddr;
//	_s64 dst = (_s64)_dstAddr;
//
//	ExpandAllocateIfNeeded(6);
//	WriteByte(0xFF);
//	WriteByte(0x15);
//	WriteS32(static_cast<int>(dst - src - 6));
//#else
//	static_assert(false, "come on...");
//#endif
//}
