/*
 * 작성자: 윤정도
 * 생성일: 2/22/2026
 * =====================
 *
 *
 * 메모리 레이아웃
 * | 접두 헤더 | 타겟 아이디(8) | 메시지 아이디(4) | 요소 개수(2) | 메모리 크기(4) | 요스 1 | 요소 2 | 요소 3 | ...
 *
 * 요소 레이아웃
 * 기본:				| 요소 타입(1) | 요소 데이터 |
 * 바이너리, 문자열	| 요소 타입(1) | 길이 (가변) | 바이너리 |
 *
 */

#pragma once

#include <jc/Primitives/RefCountObject.h>
#include <jc/Primitives/Span.h>

NS_JC_BEGIN

class CMessageContext;
class CMessageView;
class MemoryPoolAbstract;

template <typename T, typename TAllocator>
class Vector;

#pragma pack(push, 1) // 네트워크 스트림으로 전달도 가능하기 때문에 패킹은 1로 고정한다.
struct CMessageHeader
{
	object_id targetId_ = 0;
	_u32 msgId_ = 0;
	_u32 writeMemOffset_ = 0;	// 요소를 몇byte 만큼 Write 했는지 = 전체 메모리중 사용한 메모리 크기 = 스트림으로 전달될 메모리 크기 (접두 메모리 + CMessageHeader + 사용한 요소 메모리(용량이 아님))
	_u16 writeOffset_ = 0;		// 요소를 몇개 Write 했는지
};
#pragma pack(pop)

// 요소 타입별 메모리 사용량과 타입 코드를 정의한다.
template <typename T>
struct CMessage_VariantTraits
{
	static constexpr _u8 VARIANT_TYPE = 0; // 정의되지 않은 타입
	static constexpr _u8 MEM_SIZE = 77; // 계산할 수 없음
};

template <> struct CMessage_VariantTraits<_s8> { static constexpr _u8 MEM_SIZE = 1 + sizeof(_s8); static constexpr _u8 VARIANT_TYPE = 1; };
template <> struct CMessage_VariantTraits<_u8> { static constexpr _u8 MEM_SIZE = 1 + sizeof(_u8); static constexpr _u8 VARIANT_TYPE = 2; };
template <> struct CMessage_VariantTraits<_s16> { static constexpr _u8 MEM_SIZE = 1 + sizeof(_s16); static constexpr _u8 VARIANT_TYPE = 3; };
template <> struct CMessage_VariantTraits<_u16> { static constexpr _u8 MEM_SIZE = 1 + sizeof(_u16); static constexpr _u8 VARIANT_TYPE = 4; };
template <> struct CMessage_VariantTraits<_s32> { static constexpr _u8 MEM_SIZE = 1 + sizeof(_s32); static constexpr _u8 VARIANT_TYPE = 5; };
template <> struct CMessage_VariantTraits<_s32l> { static constexpr _u8 MEM_SIZE = 1 + sizeof(_s32l); static constexpr _u8 VARIANT_TYPE = 5; };
template <> struct CMessage_VariantTraits<_u32> { static constexpr _u8 MEM_SIZE = 1 + sizeof(_u32); static constexpr _u8 VARIANT_TYPE = 6; };
template <> struct CMessage_VariantTraits<_u32l> { static constexpr _u8 MEM_SIZE = 1 + sizeof(_u32l); static constexpr _u8 VARIANT_TYPE = 6; };
template <> struct CMessage_VariantTraits<_u64> { static constexpr _u8 MEM_SIZE = 1 + sizeof(_u64); static constexpr _u8 VARIANT_TYPE = 7; };
template <> struct CMessage_VariantTraits<_s64> { static constexpr _u8 MEM_SIZE = 1 + sizeof(_s64); static constexpr _u8 VARIANT_TYPE = 8; };
template <> struct CMessage_VariantTraits<_f32> { static constexpr _u8 MEM_SIZE = 1 + sizeof(_f32); static constexpr _u8 VARIANT_TYPE = 9; };
template <> struct CMessage_VariantTraits<_f64> { static constexpr _u8 MEM_SIZE = 1 + sizeof(_f64); static constexpr _u8 VARIANT_TYPE = 10; };
template <> struct CMessage_VariantTraits<void*> { static constexpr _u8 MEM_SIZE = 1 + sizeof(void*); static constexpr _u8 VARIANT_TYPE = 11; };
template <> struct CMessage_VariantTraits<bool> { static constexpr _u8 MEM_SIZE = 1 + sizeof(bool); static constexpr _u8 VARIANT_TYPE = 1; };

//////////////////////////////////////////////////////////////////////////////////////////
class JC_DLL CMessage
{
public:
	CMessage();
	CMessage(_u32 _prefixMemCapacity, _u32 _elemMemCapacity, int _msgId = 0, object_id _targetId = 0);
	CMessage(const CMessage& _other);
	CMessage(CMessage&& _other) noexcept;
	~CMessage();

	CMessage& operator=(const CMessage& _other);
	CMessage& operator=(CMessage&& _other);

	enum VariantType
	{
		vt_none,
		vt_s8,
		vt_u8,
		vt_s16,
		vt_u16,
		vt_s32,
		vt_u32,
		vt_s64,
		vt_u64,
		vt_f32,
		vt_f64,
		vt_ptr,
		vt_bool,
		vt_string,
		vt_binary,
		vt_max,
	};

	static CMessage Null;
	static constexpr _u32 USAGE_NONE = 0;
	static constexpr _u32 DEFAULT_ELEM_MEM_CAPACITY = 256;
	static constexpr _u16 MESSAGE_HEADER_SIZE = sizeof(CMessageHeader);
	static constexpr _u8  MEM_SIZE_VARIANT = 77; // 요소 크기가 가변적인 타입의 메모리 크기를 지칭함
	static constexpr _u8  MEM_SIZE[vt_max]
	{
		0,
		CMessage_VariantTraits<_s8>::MEM_SIZE,
		CMessage_VariantTraits<_u8>::MEM_SIZE,
		CMessage_VariantTraits<_s16>::MEM_SIZE,
		CMessage_VariantTraits<_u16>::MEM_SIZE,
		CMessage_VariantTraits<_s32>::MEM_SIZE,
		CMessage_VariantTraits<_u32>::MEM_SIZE,
		CMessage_VariantTraits<_s64>::MEM_SIZE,
		CMessage_VariantTraits<_u64>::MEM_SIZE,
		CMessage_VariantTraits<_f32>::MEM_SIZE,
		CMessage_VariantTraits<_f64>::MEM_SIZE,
		CMessage_VariantTraits<void*>::MEM_SIZE,
		CMessage_VariantTraits<bool>::MEM_SIZE,
		MEM_SIZE_VARIANT,
		MEM_SIZE_VARIANT,
	};

	bool		IsNull() const;
	bool		IsEmpty() const;

	void		ReadyDefaultContext();

	void		ResetWriteOffset();	// write offset을 초기화한다. 그에 맞춰서 write mem offset도 초기화된다.
	void		ResetReadOffset();	// read offset을 초기화한다. 그에 맞춰서 read mem offset도 초기화된다.

	void		SetWriteOffset(_u16 _writeOffset); // write offset을 옮긴다. write mem offset은 write offset에 맞춰서 자동으로 계산한다. (내리는 것만 가능)
	void		SetReadOffset(_u16 _readOffset); // write offset과 write mem offset, read offset과 read mem offset을 모두 초기화한다.

	void		SetMsgId(_u32 _msgId);
	void		SetTargetId(object_id _targetId);
	void		SetUsage(_u32 _usage);
	void		SetContext(CMessageView* _pView);

	_u32		GetMsgId() const;
	object_id	GetTargetId() const;
	_u32		GetUsage() const;

	_u16		GetWriteOffset() const;
	_u16		GetReadOffset() const;

	// 좀더 안전하게 내부 포인터값을 얻을 수 있도록 함.
	_u8*		GetValue(_u32 _offset, OUT VariantType& _type, OUT _u32& _elemValueSize) const;
	VariantType GetVT(_u32 _offset);
	VariantType	GetCurrentVT() const;

	void		WriteS8(_s8 _value);
	void		WriteU8(_u8 _value);
	void		WriteS16(_s16 _value);
	void		WriteU16(_u16 _value);
	void		WriteS32(_s32 _value);
	void		WriteS32L(_s32l _value);
	void		WriteU32(_u32 _value);
	void		WriteU32L(_u32l _value);
	void		WriteS64(_s64 _value);
	void		WriteU64(_u64 _value);
	void		WriteFloat(_f32 _value);
	void		WriteDouble(_f64 _value);
	void		WritePtr(void* _value);
	void		WriteBool(bool _value);
	void		WriteString(const String& _str);
	void		WriteBinary(const _u8* _pBytes, _u32 _len);
	void		WriteBinaryDummy(_u32 _len);

	// 해당 값을 저장하고 저장된 값의 메모리 주소를 얻음. 밖에서도 조작가능하도록 하기 위함. (포인터는 당연히 눈치껏 CMessage 소멸되기전까지 사용)
	// 아래 함수는 내부 버퍼 확장이 발생할 경우 포인터가 무효화될 수 있음. (눈치껏 똑바로 쓸 것)
	// 위험함.
	void		WriteS8(_s8 _value, OUT _s8** _ppAddr);
	void		WriteU8(_u8 _value, OUT _u8** _ppAddr);
	void		WriteS16(_s16 _value, OUT _s16** _ppAddr);
	void		WriteU16(_u16 _value, OUT _u16** _ppAddr);
	void		WriteS32(_s32 _value, OUT _s32** _ppAddr);
	void		WriteS32L(_s32l _value, OUT _s32l** _ppAddr);
	void		WriteU32(_u32 _value, OUT _u32** _ppAddr);
	void		WriteU32L(_u32l _value, OUT _u32l** _ppAddr);
	void		WriteS64(_s64 _value, OUT _s64** _ppAddr);
	void		WriteU64(_u64 _value, OUT _u64** _ppAddr);
	void		WriteFloat(_f32 _value, OUT _f32** _ppAddr);
	void		WriteDouble(_f64 _value, OUT _f64** _ppAddr);

	VariantType	ReadAny();	// read offset을 하나 증가시키면서 해당 요소 타입을 반환한다. (read_offset을 증가 시키고 싶을 때)
	_s8			ReadS8();
	_u8			ReadU8();
	_s16		ReadS16();
	_u16		ReadU16();
	_s32		ReadS32();
	_s32l		ReadS32L();
	_u32		ReadU32();
	_u32l		ReadU32L();
	_s64		ReadS64();
	_u64		ReadU64();
	_f32		ReadFloat();
	_f64		ReadDouble();
	void*		ReadPtr();
	bool		ReadBool();
	String		ReadString();
	bool		ReadBinary(Span<_u8> _buffer, _u32& _outLen);
	bool		ReadBinary(_u8* _pBytes, _u32 _capacity, _u32& _outLen);
		
	bool		TryReadS8(_s8& _value);
	bool		TryReadU8(_u8& _value);
	bool		TryReadS16(_s16& _value);
	bool		TryReadU16(_u16& _value);
	bool		TryReadS32(_s32& _value);
	bool		TryReadS32(_s32l& _value);
	bool		TryReadU32(_u32& _value);
	bool		TryReadU32(_u32l& _value);
	bool		TryReadS64(_s64& _value);
	bool		TryReadU64(_u64& _value);
	bool		TryReadFloat(_f32& _value);
	bool		TryReadDouble(_f64& _value);
	bool		TryReadPtr(void*& _value);
	bool		TryReadBool(bool& _value);
	bool		TryReadString(String& _value);
	bool		TryReadBinary(Span<_u8> _buffer, _u32& _outLen);
	bool		TryReadBinary(_u8* _pBytes, _u32 _capacity, _u32& _outLen);

	String			Dump() const;
	static String	Dump(const CMessage& _other);
	static _u32		GetElemMemSize(_u8 _typeCode);
private:
	CMessageView* pContext_ = nullptr; // 참조 카운트 기반 공유 컨텍스트
};

//////////////////////////////////////////////////////////////////////////////////////////
class JC_DLL CMessageView : public RefCountObject
{
public:
	CMessageView(_u8* _pBuf, _u32 _prefixMemCapacity, _u32 _memCapacity, const jc::SharedPtr<MemoryPoolAbstract>& _pMemOwner = {});
	virtual ~CMessageView();

	CMessageHeader& GetMsgHeader() const;
	CMessageHeader* GetMsgHeaderPtr() const;

	_u32 GetCapacity() const { return memCapacity_; }
	_u32 GetCapacityElem() const { return memCapacity_ - prefixMemCapacity_ - sizeof(CMessageHeader); }
	_u32 GetCapacityPrefix() const { return prefixMemCapacity_; }

	_u32 GetMsgId() const { return GetMsgHeader().msgId_; }
	object_id GetTargetId() const { return GetMsgHeader().targetId_; }
	_u32 GetUsage() const { return usage_; }

	_u32 GetHeaderSize() const { return prefixMemCapacity_ + sizeof(CMessageHeader); }

	_u16 GetWriteOffset() const { return GetMsgHeader().writeOffset_; }
	_u32 GetWriteMemOffset() const { return GetMsgHeader().writeMemOffset_; }

	_u16 GetReadOffset() const { return readOffset_; }
	_u32 GetReadMemOffset() const { return readMemOffset_; }

	void InitHeader();

	virtual void ReleaseAction() override;
	virtual bool EnsureCapacity(_u32 _requiredCapacity);

	virtual bool IsView() const { return true; } // 기본적으로 읽기 전용 뷰로 간주. 파생 클래스에서 필요에 따라 재정의 가능.
	bool IsValid() const;
	bool IsReleased() const { return isReleased_; }

	void SetMsgId(_u32 _msgId);
	void SetTargetId(object_id _targetId);
	void SetUsage(_u32 _usage);
	void SetStackAllocatedContext(bool _v); // 스택에 할당된 컨텍스트로 설정. (메모리 풀에서 할당된 버퍼가 아님을 명시적으로 나타냄)

	void SetReadOffset(_u16 _readOffset);
	void SetWriteOffset(_u16 _writeOffset);

	void ResetReadOffset();
	void ResetWriteOffset();

	_u32 CalcMemOffset(_u16 _offset) const;

	CMessage::VariantType GetCurrentVT() const;
	CMessage::VariantType GetCurrentVT(OUT _u32* _pMemSize) const;

	template <template <typename> class TypeTraits, typename T>
	void WriteValue(T _value, T** _ppOut = nullptr)
	{
		static_assert(TypeTraits<T>::VARIANT_TYPE != 0, "Unsupported type for CMessageView::WriteValue");

		CMessageHeader* pHeader = GetMsgHeaderPtr();
		constexpr _u8 ELEM_SIZE = TypeTraits<T>::MEM_SIZE;
		const _u16 HEADER_SIZE = GetHeaderSize();

		_u32 requiredMemCapacity = HEADER_SIZE + pHeader->writeMemOffset_ + ELEM_SIZE;
		if (requiredMemCapacity > memCapacity_)
		{
			// 파생 클래스에서 확장을 시도할 수 있도록 가상 함수 호출
			if (!EnsureCapacity(requiredMemCapacity))
			{
				jc_assert(false);
				return;
			}
			// 확장에 성공한 경우, 헤더 포인터를 다시 얻는다.
			pHeader = GetMsgHeaderPtr();
		}

		_u8* pWrite = pBuf_ + HEADER_SIZE + pHeader->writeMemOffset_;
		*pWrite = static_cast<_u8>(TypeTraits<T>::VARIANT_TYPE);
		++pWrite;
		*reinterpret_cast<T*>(pWrite) = _value;

		if (_ppOut)
		{
			*_ppOut = reinterpret_cast<T*>(pWrite);
		}

		++pHeader->writeOffset_;
		pHeader->writeMemOffset_ += ELEM_SIZE;
	}

	template <template <typename> class TypeTraits, typename T>
	T ReadValue()
	{
		T value{};
		int result = TryReadValue<TypeTraits, T>(value);
		switch (result)
		{
		case 0:
			break;
		case -1:
			jc_assert_msg(false, "CMessageView::ReadValue - no more elements to read");
			break;
		case -2:
			jc_assert_msg(false, "CMessageView::ReadValue - not enough bytes for element");
			break;
		case -3:
			jc_assert_msg(false, "CMessageView::ReadValue - variant type mismatch");
			break;
		default:
			jc_assert_msg(false, "CMessageView::ReadValue - unknown read error");
			break;
		}
		return value;
	}

	template <template <typename> class TypeTraits, typename T>
	int TryReadValue(T& _outValue)
	{
		static_assert(TypeTraits<T>::VARIANT_TYPE != 0, "Unsupported type for CMessageView::TryReadValue");
		const CMessageHeader& header = GetMsgHeader();
		const _u16 HEADER_SIZE = GetHeaderSize();

		if (readOffset_ >= header.writeOffset_)
			return -1;

		const _u32 remaining = readMemOffset_ >= header.writeMemOffset_ ? 0 : header.writeMemOffset_ - readMemOffset_;
		constexpr _u8 ELEM_SIZE = TypeTraits<T>::MEM_SIZE;
		if (remaining < ELEM_SIZE)
			return -2;

		_u8* pRead = pBuf_ + HEADER_SIZE + readMemOffset_;
		const _u8 typeCode = *pRead;
		++pRead;

		if (typeCode != static_cast<_u8>(TypeTraits<T>::VARIANT_TYPE))
			return -3;

		_outValue = *reinterpret_cast<T*>(pRead);
		readMemOffset_ += ELEM_SIZE;
		++readOffset_;
		return 0;
	}

	void		WriteString(const String& _str);
	void		WriteBinary(const _u8* _pBytes, _u32 _len);
	void		WriteBinaryImpl(CMessage::VariantType _type, const _u8* _pBytes, _u32 _len);

	CMessage::VariantType	ReadAny(OUT _u32* _pMemSize);
	String					ReadString();
	bool					ReadBinary(Span<_u8> _buffer, OUT _u32& _outLen);
	bool 					ReadBinary(_u8* _pBytes, _u32 _capacity, OUT _u32& _outLen);

	bool					TryReadString(OUT String& _value);
	bool					TryReadBinary(Span<_u8> _buffer, OUT _u32& _outLen);
	bool					TryReadBinary(_u8* _pBytes, _u32 _capacity, OUT _u32& _outLen);

	int						TryReadBinaryImpl(CMessage::VariantType _expectedType, _u8** _ppBuf, _u32 _capacity, OUT _u32& _outLen);
	const char*				GetBinaryReadErrorMessage(int _errorCode);

	_u8*					GetValue(_u16 _offset, OUT CMessage::VariantType& _type, OUT _u32& _elemValueSize);

	jc::String				Dump() const;

	static CMessage::VariantType PeekVT(_u8* _pBuf, _u32 _capacity, OUT _u32* _pMemSize = nullptr, OUT _u32* _pVTSize = nullptr);

	jc::SharedPtr<MemoryPoolAbstract> pMemOwner_ = nullptr; // pBuf_의 소유권자 (pBuf_는 이 메모리풀에서 할당받은 메모리 청크에서 일부분을 나타낸다.)
	_u32	usage_ = CMessage::USAGE_NONE;	// 이 메시지의 사용처
	_u32	prefixMemCapacity_ = 0;			// 접두 메모리 크기
	_u32	memCapacity_ = 0;				// 전체 메모리 용량
	_u32	readMemOffset_ = 0;				// 요소 메모리를 얼만큼 읽었는지
	_u16	readOffset_ = 0;				// 요소 메모리를 몇개나 읽었는지
	bool	isStackAllocatedContext_ = false; // 스택에 할당된 컨텍스트인지
	bool	isReleased_ = false;			// Release된 개체인지.
	_u8*	pBuf_ = nullptr;
};

//////////////////////////////////////////////////////////////////////////////////////////
class JC_DLL CMessageContext : public CMessageView
{
public:
	CMessageContext(_u32 _prefixMemCapacity, _u32 _elemMemCapacity, int _msgId = 0, object_id _targetId = 0);
	virtual ~CMessageContext();

	virtual bool EnsureCapacity(_u32 _requiredCapacity) override;
	virtual bool IsView() const override { return false; }
};

NS_END
