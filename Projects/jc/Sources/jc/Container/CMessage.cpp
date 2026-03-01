/*
 * 작성자: 윤정도
 * 생성일: 2/22/2026
 * =====================
 */


#include <jc/Core.h>
#include <jc/Container/CMessage.h>
#include <jc/Primitives/StringUtil.h>

USING_NS_JC;

CMessage CMessage::Null;

//////////////////////////////////////////////////////////////////////////////////////////
CMessage::CMessage()
{
	pContext_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessage::CMessage(_u32 _prefixMemCapacity, _u32 _elemMemCapacity, int _msgId /*= 0*/, object_id _targetId /*= 0*/)
{
	pContext_ = dbg_new CMessageContext(_prefixMemCapacity, _elemMemCapacity, _msgId, _targetId);
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessage::CMessage(const CMessage& _other) : pContext_(_other.pContext_)
{
	// 공유 컨텍스트의 참조 카운트를 증가시켜 안전하게 공유한다.
	if (pContext_)
	{
		pContext_->AddRef();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessage::CMessage(CMessage&& _other) noexcept : pContext_(_other.pContext_)
{
	_other.pContext_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessage::~CMessage()
{
	JC_RELEASE_SAFE(pContext_);
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessage& CMessage::operator=(const CMessage& _other)
{
	if (this == &_other)
	{
		return *this;
	}

	JC_RELEASE_SAFE(pContext_);

	if (pContext_ = _other.pContext_)
	{
		pContext_->AddRef();
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessage& CMessage::operator=(CMessage&& _other)
{
	if (this == &_other)
	{
		return *this;
	}

	JC_RELEASE_SAFE(pContext_);

	pContext_ = _other.pContext_;
	_other.pContext_ = nullptr;

	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::IsNull() const
{ 
	return pContext_ == nullptr; 
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::IsEmpty() const
{
	return pContext_ == nullptr || pContext_->GetWriteOffset() == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::ReadyDefaultContext()
{
	if (pContext_ == nullptr)
	{
		pContext_ = dbg_new CMessageContext(0, DEFAULT_ELEM_MEM_CAPACITY);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::ResetWriteOffset()
{
	ReadyDefaultContext();
	return pContext_->ResetWriteOffset();
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::ResetReadOffset()
{
	ReadyDefaultContext();
	return pContext_->ResetReadOffset();
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::SetWriteOffset(_u16 _writeOffset)
{
	ReadyDefaultContext();
	return pContext_->SetWriteOffset(_writeOffset);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::SetReadOffset(_u16 _readOffset)
{
	ReadyDefaultContext();
	return pContext_->SetReadOffset(_readOffset);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::SetMsgId(_u32 _msgId)
{
	ReadyDefaultContext();
	pContext_->GetMsgHeader().msgId_ = _msgId;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::SetTargetId(object_id _targetId)
{
	ReadyDefaultContext();
	pContext_->GetMsgHeader().targetId_ = _targetId;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::SetContext(CMessageView* _pView)
{
	if (pContext_ == _pView)
		return;
	if (_pView == nullptr)
		return;
	if (!_pView->IsValid())
	{
		jc_assert_msg(false, "CMessage::SetContext - invalid CMessageView provided");
		return;
	}

	JC_RELEASE_SAFE(pContext_);
	pContext_ = _pView;
}

//////////////////////////////////////////////////////////////////////////////////////////
_u16 CMessage::GetWriteOffset() const
{
	if (IsNull())
	{
		return 0;
	}
	return pContext_->GetWriteOffset();
}

//////////////////////////////////////////////////////////////////////////////////////////
_u16 CMessage::GetReadOffset() const
{
	if (IsNull())
	{
		return 0;
	}
	return pContext_->GetReadOffset();
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteS8(_s8 _value) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _s8>(_value); }
void CMessage::WriteU8(_u8 _value) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _u8>(_value); }
void CMessage::WriteS16(_s16 _value) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _s16>(_value); }
void CMessage::WriteU16(_u16 _value) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _u16>(_value); }
void CMessage::WriteS32(_s32 _value) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _s32>(_value); }
void CMessage::WriteS32L(_s32l _value) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _s32l>(_value); }
void CMessage::WriteU32(_u32 _value) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _u32>(_value); }
void CMessage::WriteU32L(_u32l _value) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _u32l>(_value); }
void CMessage::WriteS64(_s64 _value) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _s64>(_value); }
void CMessage::WriteU64(_u64 _value) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _u64>(_value); }
void CMessage::WriteFloat(_f32 _value) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _f32>(_value); }
void CMessage::WriteDouble(_f64 _value) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _f64>(_value); }
void CMessage::WritePtr(void* _value) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, void*>(_value); }

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteString(const String& _str)
{
	ReadyDefaultContext();
	pContext_->WriteString(_str);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteBinary(const _u8* _pBytes, const _u32 _len)
{
	ReadyDefaultContext();
	pContext_->WriteBinary(_pBytes, _len);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteBinaryDummy(_u32 _len)
{
	static _u8 dummy[256]{};
	if (_len > sizeof(dummy))
	{
		WriteBinary(dummy, _len);
	}
	else
	{
		_u8* pDummy = dbg_new _u8[_len]{};
		WriteBinary(pDummy, _len);
		delete[] pDummy;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteS8(_s8 _value, OUT _s8** _ppAddr) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _s8>(_value, _ppAddr); }
void CMessage::WriteU8(_u8 _value, OUT _u8** _ppAddr) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _u8>(_value, _ppAddr); }
void CMessage::WriteS16(_s16 _value, OUT _s16** _ppAddr) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _s16>(_value, _ppAddr); }
void CMessage::WriteU16(_u16 _value, OUT _u16** _ppAddr) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _u16>(_value, _ppAddr); }
void CMessage::WriteS32(_s32 _value, OUT _s32** _ppAddr) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _s32>(_value, _ppAddr); }
void CMessage::WriteS32L(_s32l _value, OUT _s32l** _ppAddr) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _s32l>(_value, _ppAddr); }
void CMessage::WriteU32(_u32 _value, OUT _u32** _ppAddr) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _u32>(_value, _ppAddr); }
void CMessage::WriteU32L(_u32l _value, OUT _u32l** _ppAddr) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _u32l>(_value, _ppAddr); }
void CMessage::WriteS64(_s64 _value, OUT _s64** _ppAddr) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _s64>(_value, _ppAddr); }
void CMessage::WriteU64(_u64 _value, OUT _u64** _ppAddr) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _u64>(_value, _ppAddr); }
void CMessage::WriteFloat(_f32 _value, OUT _f32** _ppAddr) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _f32>(_value, _ppAddr); }
void CMessage::WriteDouble(_f64 _value, OUT _f64** _ppAddr) { ReadyDefaultContext(); pContext_->WriteValue<CMessage_VariantTraits, _f64>(_value, _ppAddr); }

//////////////////////////////////////////////////////////////////////////////////////////
CMessage::VariantType CMessage::ReadAny()
{
	ReadyDefaultContext();
	return pContext_->ReadAny(nullptr);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s8	CMessage::ReadS8()
{
	ReadyDefaultContext();
	return pContext_->ReadValue<CMessage_VariantTraits, _s8>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_u8	CMessage::ReadU8()
{
	ReadyDefaultContext();
	return pContext_->ReadValue<CMessage_VariantTraits, _u8>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_s16 CMessage::ReadS16()
{
	ReadyDefaultContext();
	return pContext_->ReadValue<CMessage_VariantTraits, _s16>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_u16 CMessage::ReadU16()
{
	ReadyDefaultContext();
	return pContext_->ReadValue<CMessage_VariantTraits, _u16>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 CMessage::ReadS32()
{
	ReadyDefaultContext();
	return pContext_->ReadValue<CMessage_VariantTraits, _s32>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32l CMessage::ReadS32L()
{
	ReadyDefaultContext();
	return pContext_->ReadValue<CMessage_VariantTraits, _s32l>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 CMessage::ReadU32()
{
	ReadyDefaultContext();
	return pContext_->ReadValue<CMessage_VariantTraits, _u32>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32l CMessage::ReadU32L()
{
	ReadyDefaultContext();
	return pContext_->ReadValue<CMessage_VariantTraits, _u32l>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_s64 CMessage::ReadS64()
{
	ReadyDefaultContext();
	return pContext_->ReadValue<CMessage_VariantTraits, _s64>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 CMessage::ReadU64()
{
	ReadyDefaultContext();
	return pContext_->ReadValue<CMessage_VariantTraits, _u64>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_f32 CMessage::ReadFloat()
{
	ReadyDefaultContext();
	return pContext_->ReadValue<CMessage_VariantTraits, _f32>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_f64 CMessage::ReadDouble()
{
	ReadyDefaultContext();
	return pContext_->ReadValue<CMessage_VariantTraits, _f64>();
}

//////////////////////////////////////////////////////////////////////////////////////////
void* CMessage::ReadPtr()
{
	ReadyDefaultContext();
	return pContext_->ReadValue<CMessage_VariantTraits, void*>();
}

//////////////////////////////////////////////////////////////////////////////////////////
String CMessage::ReadString()
{
	ReadyDefaultContext();
	return pContext_->ReadString();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::ReadBinary(Span<_u8> _buffer, _u32& _outLen)
{
	ReadyDefaultContext();
	return pContext_->ReadBinary(_buffer, _outLen);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::ReadBinary(_u8* _pBytes, _u32 _capacity, _u32& _outLen)
{
	ReadyDefaultContext();
	return pContext_->ReadBinary(_pBytes, _capacity, _outLen);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadS8(_s8& _value)
{
	ReadyDefaultContext();
	return pContext_->TryReadValue<CMessage_VariantTraits, _s8>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadU8(_u8& _value)
{
	ReadyDefaultContext();
	return pContext_->TryReadValue<CMessage_VariantTraits, _u8>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadS16(_s16& _value)
{
	ReadyDefaultContext();
	return pContext_->TryReadValue<CMessage_VariantTraits, _s16>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadU16(_u16& _value)
{
	ReadyDefaultContext();
	return pContext_->TryReadValue<CMessage_VariantTraits, _u16>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadS32(_s32& _value)
{
	ReadyDefaultContext();
	return pContext_->TryReadValue<CMessage_VariantTraits, _s32>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadS32(_s32l& _value)
{
	ReadyDefaultContext();
	return pContext_->TryReadValue<CMessage_VariantTraits, _s32l>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadU32(_u32& _value)
{
	ReadyDefaultContext();
	return pContext_->TryReadValue<CMessage_VariantTraits, _u32>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadU32(_u32l& _value)
{
	ReadyDefaultContext();
	return pContext_->TryReadValue<CMessage_VariantTraits, _u32l>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadS64(_s64& _value)
{
	ReadyDefaultContext();
	return pContext_->TryReadValue<CMessage_VariantTraits, _s64>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadU64(_u64& _value)
{
	ReadyDefaultContext();
	return pContext_->TryReadValue<CMessage_VariantTraits, _u64>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadFloat(_f32& _value)
{
	ReadyDefaultContext();
	return pContext_->TryReadValue<CMessage_VariantTraits, _f32>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadDouble(_f64& _value)
{
	ReadyDefaultContext();
	return pContext_->TryReadValue<CMessage_VariantTraits, _f64>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadPtr(void*& _value)
{
	ReadyDefaultContext();
	return pContext_->TryReadValue<CMessage_VariantTraits, void*>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadString(String& _value)
{
	ReadyDefaultContext();
	return pContext_->TryReadString(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadBinary(Span<_u8> _buffer, _u32& _outLen)
{
	ReadyDefaultContext();
	return pContext_->TryReadBinary(_buffer, _outLen);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadBinary(_u8* _pBytes, _u32 _capacity, _u32& _outLen)
{
	ReadyDefaultContext();
	return pContext_->TryReadBinary(_pBytes, _capacity, _outLen);
}

//////////////////////////////////////////////////////////////////////////////////////////
String CMessage::Dump() const
{
	return Dump(*this);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u8* CMessage::GetValue(_u32 _offset, OUT VariantType& _type, OUT _u32& _elemValueSize) const
{
	if (IsNull())
		return nullptr;
	return pContext_->GetValue((_u16)_offset, _type, _elemValueSize);
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessage::VariantType CMessage::GetVT(_u32 _offset)
{
	ReadyDefaultContext();
	VariantType type;
	_u32 elemValueSize = 0;
	if (pContext_->GetValue((_u16)_offset, type, elemValueSize) != nullptr)
		return type;
	return vt_none;
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessage::VariantType CMessage::GetCurrentVT() const
{
	if (IsNull())
		return vt_none;
	return pContext_->GetCurrentVT();
}

//////////////////////////////////////////////////////////////////////////////////////////
String CMessage::Dump(const CMessage& _other)
{
	if (_other.IsNull())
		return String::Empty;
	return _other.pContext_->Dump();
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 CMessage::GetElemMemSize(_u8 _typeCode)
{
	return _typeCode < vt_max ? MEM_SIZE[_typeCode] : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessageView::CMessageView(_u8* _pBuf, _u32 _prefixMemCapacity, _u32 _memCapacity, const jc::SharedPtr<MemoryPoolAbstract>& _pMemOwner /*= {}*/)
: pMemOwner_(_pMemOwner)
, prefixMemCapacity_(_prefixMemCapacity)
, memCapacity_(_memCapacity)
, pBuf_(_pBuf) // _pBuf가 속한 메모리풀 (레퍼런스 유지용도)
{

	if (pBuf_)
	{
		if (memCapacity_ < prefixMemCapacity_ + sizeof(CMessageHeader))
		{
			jc_assert_msg(false, "CMessageView constructor - buffer too small for header");
			return;
		}

		CMessageHeader& header = GetMsgHeader();
		header.writeOffset_ = 0;
		header.writeMemOffset_ = 0;
		header.msgId_ = 0;
		header.targetId_ = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessageView::~CMessageView()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// CMessageView 구현
//////////////////////////////////////////////////////////////////////////////////////////
CMessageHeader& CMessageView::GetMsgHeader() const
{
	return *(CMessageHeader*)(pBuf_ + prefixMemCapacity_);
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessageHeader* CMessageView::GetMsgHeaderPtr() const
{
	return (CMessageHeader*)(pBuf_ + prefixMemCapacity_);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessageView::IsValid() const
{
	if (pBuf_ == nullptr || memCapacity_ < GetHeaderSize())
		return false;
	const CMessageHeader& header = GetMsgHeader();
	if (header.writeMemOffset_ > GetCapacityElem())
		return false;
	if (header.writeOffset_ > 0 && header.writeMemOffset_ == 0)
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessageView::SetReadOffset(_u16 _readOffset)
{
	_u16 writeOffset = GetWriteOffset();
	_u16 readOffset = Math::Min(_readOffset, writeOffset);
	if (readOffset == readOffset_)
		return;

	if (readOffset == 0)
	{
		ResetReadOffset();
		return;
	}

	readMemOffset_ = CalcMemOffset(readOffset);
	readOffset_ = readOffset;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessageView::SetWriteOffset(_u16 _writeOffset)
{
	auto& header = GetMsgHeader();
	_u16 srcWriteOffset = header.writeOffset_;
	_u16 dstWriteOffset = Math::Min(_writeOffset, srcWriteOffset);
	if (dstWriteOffset == srcWriteOffset)
		return;

	if (dstWriteOffset == 0)
	{
		ResetWriteOffset();
		return;
	}

	header.writeMemOffset_ = CalcMemOffset(dstWriteOffset);
	header.writeOffset_ = dstWriteOffset;

	if (readOffset_ > dstWriteOffset)
	{
		readMemOffset_ = header.writeMemOffset_;
		readOffset_ = dstWriteOffset;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessageView::ResetReadOffset()
{
	readOffset_ = 0;
	readMemOffset_ = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessageView::ResetWriteOffset()
{
	auto& header = GetMsgHeader();
	header.writeOffset_ = 0;
	header.writeMemOffset_ = 0;
	ResetReadOffset();
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 CMessageView::CalcMemOffset(_u16 _offset) const
{
	CMessageHeader& header = const_cast<CMessageView*>(this)->GetMsgHeader();
	const _u16 HEADER_SIZE = GetHeaderSize();

	if (_offset > header.writeOffset_)
	{
		_offset = header.writeOffset_;
	}

	if (_offset > readOffset_)
	{
		_u32 remaining = header.writeMemOffset_ - readMemOffset_;
		_u32 memOffset = readMemOffset_;
		_u8* pRead = pBuf_ + HEADER_SIZE + readMemOffset_;

		for (_u16 i = readOffset_; i < _offset; ++i)
		{
			_u32 memSize = 0;
			CMessage::VariantType typeCode = PeekVT(pRead, remaining, &memSize);
			if (typeCode == CMessage::vt_none)
				return Memory::INVALID_OFFSET;
			if (remaining < memSize)
				return Memory::INVALID_OFFSET;

			remaining -= memSize;
			memOffset += memSize;
			pRead += memSize;
		}
		return memOffset;
	}

	_u32 remaining = header.writeMemOffset_;
	_u32 memOffset = 0;
	_u8* pRead = pBuf_ + HEADER_SIZE;

	for (_u16 i = 0; i < _offset; ++i)
	{
		if (remaining == 0)
		{
			jc_assert(false);
			return memOffset;
		}

		_u32 memSize = 0;
		CMessage::VariantType typeCode = PeekVT(pRead, remaining, &memSize);
		if (typeCode == CMessage::vt_none)
		{
			return memOffset;
		}

		if (remaining < memSize)
		{
			jc_assert(false);
			return memOffset;
		}

		remaining -= memSize;
		memOffset += memSize;
		pRead += memSize;
	}

	return memOffset;
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessage::VariantType CMessageView::GetCurrentVT() const
{
	return GetCurrentVT(nullptr);
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessage::VariantType CMessageView::GetCurrentVT(OUT _u32* _pMemSize) const
{
	const CMessageHeader& header = const_cast<CMessageView*>(this)->GetMsgHeader();

	if (readOffset_ >= header.writeOffset_ || header.writeMemOffset_ == 0)
	{
		return CMessage::vt_none;
	}

	const _u32 remaining = header.writeMemOffset_ - readMemOffset_;
	if (remaining == 0)
	{
		return CMessage::vt_none;
	}

	_u8* pRead = pBuf_ + GetHeaderSize() + readMemOffset_;
	return PeekVT(pRead, remaining, _pMemSize);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessageView::WriteString(const String& _str)
{
	WriteBinaryImpl(CMessage::vt_string, (_u8*)_str.SafeSource(), static_cast<_u32>(_str.Length()));
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessageView::WriteBinary(const _u8* _pBytes, _u32 _len)
{
	WriteBinaryImpl(CMessage::vt_binary, _pBytes, _len);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessageView::WriteBinaryImpl(CMessage::VariantType _type, const _u8* _pBytes, _u32 _len)
{
	CMessageHeader* pHeader = GetMsgHeaderPtr();
	const _u16 HEADER_SIZE = GetHeaderSize();

	_u32 lengthMemSize = Memory::CalcU32_LEB128(_len);
	_u16 elemSize = static_cast<_u16>(1 + lengthMemSize + _len);
	_u32 requiredMemCapacity = HEADER_SIZE + pHeader->writeMemOffset_ + elemSize;
	if (requiredMemCapacity > memCapacity_)
	{
		// 가상 EnsureCapacity 호출: 컨텍스트에서는 확장, 순수 뷰는 assert 후 실패
		if (!EnsureCapacity(requiredMemCapacity))
		{
			jc_assert(false);
			return;
		}
		pHeader = GetMsgHeaderPtr();
	}

	_u8* pWrite = pBuf_ + HEADER_SIZE + pHeader->writeMemOffset_;

	*pWrite = static_cast<_u8>(_type);
	pWrite++;

	const _u32 writtenLenBytes = Memory::WriteU32_LEB128(pWrite, memCapacity_ - static_cast<_u32>(pWrite - pBuf_), _len);
	if (writtenLenBytes == Memory::INVALID_OFFSET)
	{
		jc_assert(false);
		return;
	}
	pWrite += writtenLenBytes;

	Memory::CopyUnsafe(pWrite, _pBytes, static_cast<int>(_len));

	++pHeader->writeOffset_;
	pHeader->writeMemOffset_ += elemSize;
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessage::VariantType CMessageView::ReadAny(OUT _u32* _pMemSize /*= nullptr*/)
{
	CMessageHeader& header = GetMsgHeader();

	if (readOffset_ >= header.writeOffset_)
	{
		jc_assert_msg(false, "CMessageView::ReadAny - no more elements to read");
		return CMessage::vt_none;
	}

	_u32 memSize = 0;
	auto vt = GetCurrentVT(&memSize);
	if (vt != CMessage::vt_none)
	{
		if (_pMemSize)
		{
			*_pMemSize = memSize;
		}

		readOffset_++;
		readMemOffset_ += memSize;
	}
	return vt;
}

//////////////////////////////////////////////////////////////////////////////////////////
String CMessageView::ReadString()
{
	_u8* pBuf = nullptr;
	_u32 length = 0;

	int result = TryReadBinaryImpl(CMessage::vt_string, &pBuf, 0, length);
	if (result != 0)
	{
		jc_assert_msg(false, "CMessageView::ReadString - failed, error code: %d (%s)", result, GetBinaryReadErrorMessage(result));
		return String::Null;
	}

	String str(0);
	if (length > 0)
	{
		str.ExchangeSource((char*)pBuf, length);
	}
	return str;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessageView::ReadBinary(Span<_u8> _buffer, OUT _u32& _outLen)
{
	if (_buffer.pArr_ == nullptr || _buffer.len_ == 0)
	{
		jc_assert_msg(false, "CMessageView::ReadBinary - invalid buffer");
		return false;
	}

	int result = TryReadBinaryImpl(CMessage::vt_binary, &_buffer.pArr_, _buffer.len_, _outLen);
	if (result != 0)
	{
		jc_assert_msg(false, "CMessageView::ReadBinary - failed, error code: %d (%s)", result, GetBinaryReadErrorMessage(result));
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessageView::ReadBinary(_u8* _pBytes, _u32 _capacity, OUT _u32& _outLen)
{
	if (_pBytes == nullptr || _capacity == 0)
	{
		jc_assert_msg(false, "CMessageView::ReadBinary - invalid buffer");
		return false;
	}

	int result = TryReadBinaryImpl(CMessage::vt_binary, &_pBytes, _capacity, _outLen);
	if (result != 0)
	{
		jc_assert_msg(false, "CMessageView::ReadBinary - failed, error code: %d (%s)", result, GetBinaryReadErrorMessage(result));
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessageView::TryReadString(OUT String& _value)
{
	_u8* pBuf = (_u8*)_value.Source();
	_u8* pBufOrigin = pBuf;
	_u32 length = 0;

	int result = TryReadBinaryImpl(CMessage::vt_string, &pBuf, pBuf == nullptr ? 0 : _value.Capacity(), length);
	if (result != 0)
		return false;

	if (length == 0)
	{
		_value.Clear();
	}
	else
	{
		if (pBufOrigin == nullptr)
		{
			_value.ExchangeSource((char*)pBuf, length);
		}
		else
		{
			_value.SetLength((int)length);
		}
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessageView::TryReadBinary(Span<_u8> _buffer, OUT _u32& _outLen)
{
	if (_buffer.pArr_ == nullptr || _buffer.len_ == 0)
		return false;
	if (TryReadBinaryImpl(CMessage::vt_binary, &_buffer.pArr_, _buffer.len_, _outLen) == 0)
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessageView::TryReadBinary(_u8* _pBytes, _u32 _capacity, OUT _u32& _outLen)
{
	if (_pBytes == nullptr || _capacity == 0)
		return false;
	if (TryReadBinaryImpl(CMessage::vt_binary, &_pBytes, _capacity, _outLen) != 0)
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
int CMessageView::TryReadBinaryImpl(CMessage::VariantType _expectedType, _u8** _ppBuf, _u32 _capacity, OUT _u32& _outLen)
{
	if (_ppBuf == nullptr)
		return -8;

	const CMessageHeader& header = GetMsgHeader();
	const _u16 HEADER_SIZE = GetHeaderSize();

	if (readOffset_ >= header.writeOffset_)
		return -1;

	const _u32 remaining = readMemOffset_ >= header.writeMemOffset_ ? 0 : header.writeMemOffset_ - readMemOffset_;
	if (remaining == 0)
		return -2;

	_u8* pRead = pBuf_ + HEADER_SIZE + readMemOffset_;
	const _u8 typeCode = *pRead;
	if (typeCode != static_cast<_u8>(_expectedType))
		return -3;
	if (remaining == 1)
		return -4;
	++pRead;
	_u32 length = 0;
	_u32 readLenBytes = Memory::ReadU32_LEB128(pRead, remaining - 1, length);
	if (readLenBytes == Memory::INVALID_OFFSET)
		return -5;
	_u32 totalSize = 1 + readLenBytes + length;
	if (remaining < totalSize)
		return -6;
	_outLen = length;
	_u8* pBuf = *_ppBuf;
	if (pBuf == nullptr)
	{
		if (length > 0)
		{
			_u32 allocSize = length;
			if (_expectedType == CMessage::vt_string)
			{
				allocSize += 1;
			}

			pBuf = Memory::Allocate<_u8*>(allocSize);
			pBuf[allocSize - 1] = '\0';
			*_ppBuf = pBuf;
		}
	}
	else
	{
		if (length > _capacity)
			return -7;
	}

	pRead += readLenBytes;
	if (length > 0)
	{
		Memory::CopyUnsafe(pBuf, pRead, length);
	}

	readMemOffset_ += totalSize;
	++readOffset_;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
const char* CMessageView::GetBinaryReadErrorMessage(int _errorCode)
{
	switch (_errorCode)
	{
	case -1: return "no more elements to read";
	case -2: return "not enough bytes for element";
	case -3: return "variant type mismatch";
	case -4: return "missing LEB128 length info";
	case -5: return "invalid LEB128 length info";
	case -6: return "not enough bytes for binary data";
	case -7: return "provided buffer capacity too small";
	case -8: return "_ppBuf is nullptr";
	default: return "unknown";
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
_u8* CMessageView::GetValue(_u16 _offset, OUT CMessage::VariantType& _type, OUT _u32& _elemValueSize)
{
	CMessage::VariantType type = CMessage::vt_none;
	_u32 memSize = 0;
	_u32 vtSize = 0;

	CMessageHeader& header = GetMsgHeader();

	if (_offset == 0)
	{
		_u8* pRead = pBuf_ + GetHeaderSize();
		type = PeekVT(pRead, header.writeMemOffset_, &memSize, &vtSize);
		if (type == CMessage::vt_none)
			return nullptr;
		_type = type;
		_elemValueSize = memSize - 1 - vtSize;
		return pRead + 1 + vtSize;
	}

	if (_offset > header.writeOffset_)
		return nullptr;
	_u32 memOffset = CalcMemOffset(_offset - 1);
	if (memOffset == Memory::INVALID_OFFSET)
		return nullptr;
	if (memOffset >= header.writeMemOffset_)
		return nullptr;
	_u8* pRead = pBuf_ + GetHeaderSize() + memOffset;
	type = PeekVT(pRead, header.writeMemOffset_ - memOffset, &memSize, &vtSize);
	_type = type;
	_elemValueSize = memSize - 1 - vtSize;
	return pRead + 1 + vtSize;
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String CMessageView::Dump() const
{
	jc::String str(1024);
	const CMessageHeader& header = const_cast<CMessageView*>(this)->GetMsgHeader();
	const _u16 HEADER_SIZE = GetHeaderSize();

	_u8*	pRead = pBuf_ + HEADER_SIZE;
	_u32	remaining = header.writeMemOffset_;
	_u8	typeCode = 0;

	while (true)
	{
		if (remaining == 0)
			break;

		typeCode = *pRead;
		--remaining;
		if (remaining == 0)
		{
			str += "  <invalid element: missing type code>\n";
			break;
		}
		++pRead;

		switch (typeCode)
		{
		case CMessage::vt_s8:
		case CMessage::vt_u8:
			{
				if (remaining < 1)
				{
					str += "  <invalid element length for s8>\n";
					break;
				}

				if (typeCode == CMessage::vt_s8)
				{
					_s32 value = *reinterpret_cast<_s8*>(pRead);
					str += StringUtil::Format("  s8: %d", value);
				}
				else
				{
					_s32 value = *reinterpret_cast<_u8*>(pRead);
					str += StringUtil::Format("  u8: %d", value);
				}
				
				remaining -= 1;
				pRead += 1;
			}
			break;
		case CMessage::vt_s16:
		case CMessage::vt_u16:
			{
				if (remaining < 2)
				{
					str += "  <invalid element length for s16>\n";
					break;
				}

				if (typeCode == CMessage::vt_s16)
				{
					_s32 value = *reinterpret_cast<_s16*>(pRead);
					str += StringUtil::Format("  s16: %d", value);
				}
				else
				{
					_s32 value = *reinterpret_cast<_u16*>(pRead);
					str += StringUtil::Format("  u16: %d", value);
				}
				remaining -= 2;
				pRead += 2;
			}
			break;
		case CMessage::vt_s32:
		case CMessage::vt_u32:
			{
				if (remaining < 4)
				{
					str += "  <invalid element length for s32>\n";
					break;
				}

				if (typeCode == CMessage::vt_s32)
				{
					_s64 value = *reinterpret_cast<_s32*>(pRead);
					str += StringUtil::Format("  s32: %lld", value);
				}
				else
				{
					_s64 value = *reinterpret_cast<_u32*>(pRead);
					str += StringUtil::Format("  u32: %lld", value);
				}
				remaining -= 4;
				pRead += 4;
			}
			break;
		case CMessage::vt_s64:
		case CMessage::vt_u64:
			{
				if (remaining < 8)
				{
					str += "  <invalid element length for s64>\n";
					break;
				}

				if (typeCode == CMessage::vt_s64)
				{
					_s64 value = *reinterpret_cast<_s64*>(pRead);
					str += StringUtil::Format("  s64: %lld", value);
				}
				else
				{
					_u64 value = *reinterpret_cast<_u64*>(pRead);
					str += StringUtil::Format("  u64: %llu", value);
				}
				
				remaining -= 8;
				pRead += 8;
			}
			break;
		case CMessage::vt_f32:
			{
				if (remaining < 4)
				{
					str += "  <invalid element length for f32>\n";
					break;
				}

				_f32 value = *reinterpret_cast<_f32*>(pRead);
				str += StringUtil::Format("  f32: %.3f", value);
				remaining -= 4;
				pRead += 4;
			}
			break;
		case CMessage::vt_f64:
			{
				if (remaining < 8)
				{
					str += "  <invalid element length for f64>\n";
					break;
				}

				_f64 value = *reinterpret_cast<_f64*>(pRead);
				str += StringUtil::Format("  f64: %.3lf", value);
				remaining -= 8;
				pRead += 8;
			}
			break;
		case CMessage::vt_ptr:
			{
				if (remaining < sizeof(void*))
				{
					str += "  <invalid element length for pointer>\n";
					break;
				}

				_ptr value = *reinterpret_cast<_ptr*>(pRead);
				str += StringUtil::Format("  ptr: 0x%p", value);
				remaining -= sizeof(_ptr);
				pRead += sizeof(_ptr);
			}
			break;
		case CMessage::vt_binary:
		case CMessage::vt_string:
			{
				_u32 length = 0;
				_u32 readLenBytes = Memory::ReadU32_LEB128(pRead, remaining, length);
				if (readLenBytes == Memory::INVALID_OFFSET)
				{
					str += "  <invalid LEB128 length info>\n";
					break;
				}

				remaining -= readLenBytes;
				pRead += readLenBytes;

				if (remaining < length)
				{
					str += "  <invalid element length for binary/string data>\n";
				}

				if (length == 0)
				{
					str += typeCode == CMessage::vt_string ? "  String: \"\"" : "  Binary: <empty>";
				}
				else if (typeCode == CMessage::vt_string)
				{
					_u8* pBuf = pRead;
					_u8 backup = pBuf[length];
					pBuf[length] = '\0'; // null terminator for safe dumping as string
					str += "  String: \"";
					str += (char*)pBuf;
					pBuf[length] = backup;
					pRead += length;
					remaining -= length;
				}
				else
				{
					str += StringUtil::Format("  Binary: %d bytes", length);
					pRead += length;
					remaining -= length;
				}
			}
			break;
		}
		str += "\n";
	}

	return str;
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessage::VariantType CMessageView::PeekVT(_u8* _pBuf, _u32 _capacity, OUT _u32* _pMemSize /*= nullptr*/, OUT _u32* _pVTSize /*= nullptr*/)
{
	if (_pBuf == nullptr || _capacity == 0)
	{
		jc_assert(false);
		return CMessage::vt_none;
	}

	const _u8 typeCode = *_pBuf;
	const _u32 elemMemSize = CMessage::GetElemMemSize(typeCode);

	if (_pMemSize)
	{
		if (elemMemSize == CMessage::MEM_SIZE_VARIANT)
		{
			if (_capacity < 2)
			{
				jc_assert(false);
				return CMessage::vt_none;
			}

			_u32 length = 0;
			_u32 readLenBytes = Memory::ReadU32_LEB128(_pBuf + 1, _capacity - 1, length);
			if (readLenBytes == 0xffffffff)
			{
				jc_assert(false);
				return CMessage::vt_none;
			}

			const _u32 variantElemMemSize = 1 + readLenBytes + length;
			if (_capacity < variantElemMemSize)
			{
				jc_assert(false);
				return CMessage::vt_none;
			}

			if (_pVTSize)
			{
				*_pVTSize = readLenBytes;
			}
			*_pMemSize = variantElemMemSize;
		}
		else if (elemMemSize > 0)
		{
			if (_capacity < elemMemSize)
			{
				jc_assert(false);
				return CMessage::vt_none;
			}

			if (_pVTSize)
			{
				*_pVTSize = 0;
			}

			*_pMemSize = elemMemSize;
		}
		else
		{
			jc_assert(false);
			return CMessage::vt_none;
		}
	}
	return static_cast<CMessage::VariantType>(typeCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
// CMessageContext
//////////////////////////////////////////////////////////////////////////////////////////
CMessageContext::CMessageContext(_u32 _prefixMemCapacity, _u32 _elemMemCapacity, int _msgId, object_id _targetId)
: CMessageView(nullptr, _prefixMemCapacity, 0)
{
	prefixMemCapacity_ = _prefixMemCapacity;
	readMemOffset_ = 0;
	readOffset_ = 0;
	memCapacity_ = _prefixMemCapacity + sizeof(CMessageHeader) + _elemMemCapacity;
	pBuf_ = Memory::Allocate<_u8*>(memCapacity_);

	CMessageHeader& header = GetMsgHeader();
	header.writeOffset_ = 0;
	header.writeMemOffset_ = 0;
	header.msgId_ = _msgId;
	header.targetId_ = _targetId;
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessageContext::~CMessageContext()
{
	Memory::Deallocate(pBuf_);
	pBuf_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessageContext::EnsureCapacity(_u32 _requiredCapacity)
{
	if (_requiredCapacity <= memCapacity_)
		return false;

	_u32 writeMemOffset = GetWriteMemOffset();
	_u32 newCapacity = memCapacity_ == 0 ? _requiredCapacity : memCapacity_;
	while (newCapacity < _requiredCapacity)
	{
		newCapacity *= 2;
	}

	newCapacity += 1;
	_u8* pNewBuf = Memory::Allocate<_u8*>(newCapacity);

	Memory::CopyUnsafe(pNewBuf, pBuf_, GetHeaderSize() + writeMemOffset);
	Memory::Deallocate(pBuf_);
	pBuf_ = pNewBuf;
	memCapacity_ = newCapacity;
	return true;
}
