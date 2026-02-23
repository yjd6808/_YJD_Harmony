/*
 * 작성자: 윤정도
 * 생성일: 2/22/2026
 * =====================
 */


#include <jc/Core.h>
#include <jc/Container/CMessage.h>

NS_JC_BEGIN
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
		pContext_->AddRef();
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
void CMessage::WriteS8(_s8 _value)
{
	jc_assert(pContext_);
	pContext_->WriteValue<CMessage_VariantTraits, _s8>(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteU8(_u8 _value)
{
	jc_assert(pContext_);
	pContext_->WriteValue<CMessage_VariantTraits, _u8>(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteS16(_s16 _value)
{
	jc_assert(pContext_);
	pContext_->WriteValue<CMessage_VariantTraits, _s16>(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteU16(_u16 _value)
{
	jc_assert(pContext_);
	pContext_->WriteValue<CMessage_VariantTraits, _u16>(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteS32(_s32 _value)
{
	jc_assert(pContext_);
	pContext_->WriteValue<CMessage_VariantTraits, _s32>(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteS32L(_s32l _value)
{
	jc_assert(pContext_);
	pContext_->WriteValue<CMessage_VariantTraits, _s32l>(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteU32(_u32 _value)
{
	jc_assert(pContext_);
	pContext_->WriteValue<CMessage_VariantTraits, _u32>(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteU32L(_u32l _value)
{
	jc_assert(pContext_);
	pContext_->WriteValue<CMessage_VariantTraits, _u32l>(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteS64(_s64 _value)
{
	jc_assert(pContext_);
	pContext_->WriteValue<CMessage_VariantTraits, _s64>(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteU64(_u64 _value)
{
	jc_assert(pContext_);
	pContext_->WriteValue<CMessage_VariantTraits, _u64>(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteFloat(_f32 _value)
{
	jc_assert(pContext_);
	pContext_->WriteValue<CMessage_VariantTraits, _f32>(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteDouble(_f64 _value)
{
	jc_assert(pContext_);
	pContext_->WriteValue<CMessage_VariantTraits, _f64>(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WritePtr(void* _value)
{
	jc_assert(pContext_);
	pContext_->WriteValue<CMessage_VariantTraits, void*>(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteString(const String& _str)
{
	jc_assert(pContext_);
	pContext_->WriteString(_str);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessage::WriteBinary(const _u8* _pBytes, const _u32 _len)
{
	jc_assert(pContext_);
	pContext_->WriteBinary(_pBytes, _len);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s8	CMessage::ReadS8()
{
	jc_assert(pContext_);
	return pContext_->ReadValue<CMessage_VariantTraits, _s8>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_u8	CMessage::ReadU8()
{
	jc_assert(pContext_);
	return pContext_->ReadValue<CMessage_VariantTraits, _u8>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_s16 CMessage::ReadS16()
{
	jc_assert(pContext_);
	return pContext_->ReadValue<CMessage_VariantTraits, _s16>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_u16 CMessage::ReadU16()
{
	jc_assert(pContext_);
	return pContext_->ReadValue<CMessage_VariantTraits, _u16>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 CMessage::ReadS32()
{
	jc_assert(pContext_);
	return pContext_->ReadValue<CMessage_VariantTraits, _s32>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32l CMessage::ReadS32L()
{
	jc_assert(pContext_);
	return pContext_->ReadValue<CMessage_VariantTraits, _s32l>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 CMessage::ReadU32()
{
	jc_assert(pContext_);
	return pContext_->ReadValue<CMessage_VariantTraits, _u32>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32l CMessage::ReadU32L()
{
	jc_assert(pContext_);
	return pContext_->ReadValue<CMessage_VariantTraits, _u32l>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_s64 CMessage::ReadS64()
{
	jc_assert(pContext_);
	return pContext_->ReadValue<CMessage_VariantTraits, _s64>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 CMessage::ReadU64()
{
	jc_assert(pContext_);
	return pContext_->ReadValue<CMessage_VariantTraits, _u64>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_f32 CMessage::ReadFloat()
{
	jc_assert(pContext_);
	return pContext_->ReadValue<CMessage_VariantTraits, _f32>();
}

//////////////////////////////////////////////////////////////////////////////////////////
_f64 CMessage::ReadDouble()
{
	jc_assert(pContext_);
	return pContext_->ReadValue<CMessage_VariantTraits, _f64>();
}

//////////////////////////////////////////////////////////////////////////////////////////
void* CMessage::ReadPtr()
{
	jc_assert(pContext_);
	return pContext_->ReadValue<CMessage_VariantTraits, void*>();
}

//////////////////////////////////////////////////////////////////////////////////////////
String CMessage::ReadString()
{
	jc_assert(pContext_);
	return pContext_->ReadString();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::ReadBinary(Span<_u8> _buffer, _u32& _outLen)
{
	jc_assert(pContext_);
	return pContext_->ReadBinary(_buffer, _outLen);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::ReadBinary(_u8* _pBytes, _u32 _capacity, _u32& _outLen)
{
	jc_assert(pContext_);
	return pContext_->ReadBinary(_pBytes, _capacity, _outLen);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadS8(_s8& _value)
{
	jc_assert(pContext_);
	return pContext_->TryReadValue<CMessage_VariantTraits, _s8>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadU8(_u8& _value)
{
	jc_assert(pContext_);
	return pContext_->TryReadValue<CMessage_VariantTraits, _u8>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadS16(_s16& _value)
{
	jc_assert(pContext_);
	return pContext_->TryReadValue<CMessage_VariantTraits, _s16>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadU16(_u16& _value)
{
	jc_assert(pContext_);
	return pContext_->TryReadValue<CMessage_VariantTraits, _u16>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadS32(_s32& _value)
{
	jc_assert(pContext_);
	return pContext_->TryReadValue<CMessage_VariantTraits, _s32>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadS32(_s32l& _value)
{
	jc_assert(pContext_);
	return pContext_->TryReadValue<CMessage_VariantTraits, _s32l>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadU32(_u32& _value)
{
	jc_assert(pContext_);
	return pContext_->TryReadValue<CMessage_VariantTraits, _u32>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadU32(_u32l& _value)
{
	jc_assert(pContext_);
	return pContext_->TryReadValue<CMessage_VariantTraits, _u32l>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadS64(_s64& _value)
{
	jc_assert(pContext_);
	return pContext_->TryReadValue<CMessage_VariantTraits, _s64>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadU64(_u64& _value)
{
	jc_assert(pContext_);
	return pContext_->TryReadValue<CMessage_VariantTraits, _u64>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadFloat(_f32& _value)
{
	jc_assert(pContext_);
	return pContext_->TryReadValue<CMessage_VariantTraits, _f32>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadDouble(_f64& _value)
{
	jc_assert(pContext_);
	return pContext_->TryReadValue<CMessage_VariantTraits, _f64>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadPtr(void*& _value)
{
	jc_assert(pContext_);
	return pContext_->TryReadValue<CMessage_VariantTraits, void*>(_value) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadString(String& _value)
{
	jc_assert(pContext_);
	return pContext_->TryReadString(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadBinary(Span<_u8> _buffer, _u32& _outLen)
{
	jc_assert(pContext_);
	return pContext_->TryReadBinary(_buffer, _outLen);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessage::TryReadBinary(_u8* _pBytes, _u32 _capacity, _u32& _outLen)
{
	jc_assert(pContext_);
	return pContext_->TryReadBinary(_pBytes, _capacity, _outLen);
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessageContext::CMessageContext(_u32 _prefixMemCapacity, _u32 _elemMemCapacity, int _msgId, object_id _targetId)
{
	_u32 elemWriteOffset = _prefixMemCapacity + sizeof(CMessageHeader);
	prefixMemCapacity_ = _prefixMemCapacity;
	readMemOffset_ = elemWriteOffset; // 시작 위치
	readOffset_ = 0;
	memCapacity_ = elemWriteOffset + _elemMemCapacity;
	pBuf_ = Memory::Allocate<_u8*>(memCapacity_);

	CMessageHeader& header = GetMsgHeader();
	header.writeOffset_ = 0;
	header.writeMemOffset_ = elemWriteOffset;	// 사용한 요소 메모리가 없는 상태의 수치로 초기화
	header.msgId_ = _msgId;
	header.targetId_ = _targetId;
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessageHeader& CMessageContext::GetMsgHeader() const
{
	return *(CMessageHeader*)(pBuf_ + prefixMemCapacity_);
}

//////////////////////////////////////////////////////////////////////////////////////////
CMessageHeader* CMessageContext::GetMsgHeaderPtr() const
{
	return (CMessageHeader*)(pBuf_ + prefixMemCapacity_);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessageContext::EnsureCapacity(_u32 _requiredCapacity)
{
	if (_requiredCapacity <= memCapacity_)
		return false;

	_u32 writeMemOffset = GetWriteMemOffset(); // 버퍼가 날아가기전에 미리 저장해놓는다.
	_u32 newCapacity = memCapacity_ == 0 ? _requiredCapacity : memCapacity_; // 기존 용량을 2배씩 늘리면서, _requiredCapacity 를 초과할 때까지 증가
	while (newCapacity < _requiredCapacity)
	{
		newCapacity *= 2;
	}

	_u8* pNewBuf = Memory::Allocate<_u8*>(newCapacity);

	// 현재까지 사용 중인 영역만 복사 (prefix + header + used elements)
	Memory::CopyUnsafe(pNewBuf, pBuf_, writeMemOffset);
	Memory::Deallocate(pBuf_);
	pBuf_ = pNewBuf;
	memCapacity_ = newCapacity;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessageContext::WriteString(const String& _str)
{
	WriteBinaryImpl(CMessage::vt_string, (_u8*)_str.SafeSource(), static_cast<_u32>(_str.Length())); // null은 제외하고 전달
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessageContext::WriteBinary(const _u8* _pBytes, _u32 _len)
{
	WriteBinaryImpl(CMessage::vt_binary, _pBytes, _len);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMessageContext::WriteBinaryImpl(CMessage::VariantType _type, const _u8* _pBytes, _u32 _len)
{
	CMessageHeader& header = GetMsgHeader();

	_u32 lengthMemSize = Memory::CalcU32_LEB128(_len);
	_u16 elemSize = static_cast<_u16>(1 + lengthMemSize + _len);
	_u32 requiredMemCapacity = header.writeMemOffset_ + elemSize; // 타입 정보 + 길이 정보 + 데이터
	bool expandNeed = requiredMemCapacity > memCapacity_;
	if (expandNeed)
	{
		if (!EnsureCapacity(requiredMemCapacity))
		{
			jc_assert(false);
			return;
		}
	}

	if (expandNeed) header = GetMsgHeader(); // 확장되었으니 갱신
	_u8* pWrite = pBuf_ + header.writeMemOffset_;

	// 1) 타입 기록
	*pWrite = static_cast<_u8>(_type);
	pWrite++;

	// 2) 길이 LEB128 기록
	const _u32 writtenLenBytes = Memory::WriteU32_LEB128(pWrite, memCapacity_ - static_cast<_u32>(pWrite - pBuf_), _len);
	// 실패(0xffffffff) 시 방어
	if (writtenLenBytes == 0xffffffff)
	{
		jc_assert(false);
		return;
	}
	pWrite += writtenLenBytes;

	// 3) 실제 데이터 기록
	Memory::CopyUnsafe(pWrite, _pBytes, static_cast<int>(_len));

	// 헤더 갱신
	++header.writeOffset_;
	header.writeMemOffset_ += elemSize;
}

//////////////////////////////////////////////////////////////////////////////////////////
String CMessageContext::ReadString()
{
	_u8* pBuf = nullptr;
	_u32 length = 0;

	int result = TryReadBinaryImpl(CMessage::vt_string, &pBuf, 0, length);
	if (result != 0)
	{
		jc_assert_msg(false, "CMessageContext::ReadString - failed, error code: %d (%s)", result, GetBinaryReadErrorMessage(result));
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
bool CMessageContext::ReadBinary(Span<_u8> _buffer, OUT _u32& _outLen)
{
	if (_buffer.pArr_ == nullptr || _buffer.len_ == 0)
	{
		jc_assert_msg(false, "CMessageContext::ReadBinary - invalid buffer");
		return false;
	}

	int result = TryReadBinaryImpl(CMessage::vt_binary, &_buffer.pArr_, _buffer.len_, _outLen);
	if (result != 0)
	{
		jc_assert_msg(false, "CMessageContext::ReadBinary - failed, error code: %d (%s)", result, GetBinaryReadErrorMessage(result));
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessageContext::ReadBinary(_u8* _pBytes, _u32 _capacity, OUT _u32& _outLen)
{
	if (_pBytes == nullptr || _capacity == 0)
	{
		jc_assert_msg(false, "CMessageContext::ReadBinary - invalid buffer");
		return false;
	}

	int result = TryReadBinaryImpl(CMessage::vt_binary, &_pBytes, _capacity, _outLen);
	if (result != 0)
	{
		jc_assert_msg(false, "CMessageContext::ReadBinary - failed, error code: %d (%s)", result, GetBinaryReadErrorMessage(result));
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessageContext::TryReadString(OUT String& _value)
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
bool CMessageContext::TryReadBinary(Span<_u8> _buffer, OUT _u32& _outLen)
{
	if (_buffer.pArr_ == nullptr || _buffer.len_ == 0)
		return false;
	if (TryReadBinaryImpl(CMessage::vt_binary, &_buffer.pArr_, _buffer.len_, _outLen) == 0)
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CMessageContext::TryReadBinary(_u8* _pBytes, _u32 _capacity, OUT _u32& _outLen)
{
	if (_pBytes == nullptr || _capacity == 0)
		return false;
	if (TryReadBinaryImpl(CMessage::vt_binary, &_pBytes, _capacity, _outLen) != 0)
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
int CMessageContext::TryReadBinaryImpl(CMessage::VariantType _expectedType, _u8** _ppBuf, _u32 _capacity, OUT _u32& _outLen)
{
	if (_ppBuf == nullptr)
		return -8;
	CMessageHeader& header = GetMsgHeader();
	// 요소 갯수 기준으로 먼저 범위 체크
	if (readOffset_ >= header.writeOffset_)
		return -1;

	// 남은 데이터가 최소 타입 코드 + 값 크기보다 작은 경우 실패
	const _u32 remaining = readMemOffset_ >= header.writeMemOffset_ ? 0 : header.writeMemOffset_ - readMemOffset_;
	if (remaining == 0) // 타입길이 정보가 없는 경우
		return -2;

	_u8* pRead = pBuf_ + readMemOffset_;
	const _u8 typeCode = *pRead;
	if (typeCode != static_cast<_u8>(_expectedType)) // 예상한 타입이 아닌 경우
		return -3;
	if (remaining == 1)
		return -4; // LEB128 길이 정보가 없는 경우
	++pRead; // 타입 코드 읽었으니 다음으로 이동
	_u32 length = 0;
	_u32 readLenBytes = Memory::ReadU32_LEB128(pRead, remaining - 1, length);
	if (readLenBytes == 0xffffffff)
		return -5; // LEB128 길이 정보가 잘못된 경우
	_u32 totalSize = 1 + readLenBytes + length; // 타입 코드 + 길이 정보 + 실제 데이터
	if (remaining < totalSize)
		return -6; // 실제 데이터가 부족한 경우
	_outLen = length;
	_u8* pBuf = *_ppBuf;
	if (pBuf == nullptr)
	{
		if (length > 0)
		{
			_u32 allocSize = length;
			if (_expectedType == CMessage::vt_string)
			{
				// 문자열인 경우 null 문자까지 포함해서 할당
				allocSize += 1;
			}

			pBuf = Memory::Allocate<_u8*>(allocSize);
			pBuf[allocSize - 1] = '\0';
			*_ppBuf = pBuf;
		}
	}
	else
	{
		// 호출자가 넘겨준 버퍼를 사용하는 모드에서는 capacity 체크
		if (length > _capacity)
			return -7;
	}

	pRead += readLenBytes; // 길이 정보 읽었으니 다음으로 이동
	if (length > 0)
	{
		Memory::CopyUnsafe(pBuf, pRead, length);
	}

	readMemOffset_ += totalSize;
	++readOffset_;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
const char* CMessageContext::GetBinaryReadErrorMessage(int _errorCode)
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

NS_END