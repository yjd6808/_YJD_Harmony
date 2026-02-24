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
	static constexpr _u8 MEM_SIZE = 0; // 계산할 수 없음
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

//////////////////////////////////////////////////////////////////////////////////////////
class JC_DLL CMessage
{
public:
	static constexpr _u16 MESSAGE_HEADER_SIZE = sizeof(CMessageHeader);

	CMessage(_u32 _prefixMemCapacity, _u32 _elemMemCapacity, int _msgId = 0, object_id _targetId = 0);
	CMessage(const CMessage& _other);
	CMessage(CMessage&& _other) noexcept;
	~CMessage();

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
		vt_string,
		vt_binary,
	};

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
	void		WriteString(const String& _str);
	void		WriteBinary(const _u8* _pBytes, _u32 _len);

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
	bool		TryReadString(String& _value);
	bool		TryReadBinary(Span<_u8> _buffer, _u32& _outLen);
	bool		TryReadBinary(_u8* _pBytes, _u32 _capacity, _u32& _outLen);

	VariantType	GetCurrentVT() const;

	String		Dump() const { return Dump(*this);}
	static String	Dump(const CMessage& _other);
	static _u32		GetElemSize(_u8 _typeCode);
private:
	CMessageContext* pContext_ = nullptr; // 참조 카운트 기반 공유 컨텍스트
};

//////////////////////////////////////////////////////////////////////////////////////////
class JC_DLL CMessageContext : public RefCountObject
{
public:
	CMessageContext(_u32 _prefixMemCapacity, _u32 _elemMemCapacity, int _msgId = 0, object_id _targetId = 0);

	CMessageHeader& GetMsgHeader() const;
	CMessageHeader* GetMsgHeaderPtr() const;

	_u32 GetCapacity() const { return memCapacity_; }
	_u32 GetCapacityElem() const { return memCapacity_ - prefixMemCapacity_ - sizeof(CMessageHeader); }
	_u32 GetCapacityPrefix() const { return prefixMemCapacity_; }

	_u32 GetHeaderSize() const { return prefixMemCapacity_ + sizeof(CMessageHeader); }

	_u16 GetWriteOffset() const { return GetMsgHeader().writeOffset_; }
	_u32 GetWriteMemOffset() const { return GetMsgHeader().writeMemOffset_; }

	_u16 GetReadOffset() const { return readOffset_; }
	_u32 GetReadMemOffset() const { return readMemOffset_; }

	CMessage::VariantType GetCurrentVT() const;

	bool EnsureCapacity(_u32 _requiredCapacity);

	template <template <typename> class TypeTraits, typename T>
	void WriteValue(T _value)
	{
		// 지원 안되는 타입이면 컴파일 타임 에러
		static_assert(TypeTraits<T>::VARIANT_TYPE != 0, "Unsupported type for CMessageContext::WriteValue");

		CMessageHeader* pHeader = GetMsgHeaderPtr();
		constexpr _u8 ELEM_SIZE = TypeTraits<T>::MEM_SIZE;	// 타입 정보(1) + 실제 값 크기
		const _u16 HEADER_SIZE = GetHeaderSize();

		_u32 requiredMemCapacity = HEADER_SIZE + pHeader->writeMemOffset_ + ELEM_SIZE;
		bool expandNeed = requiredMemCapacity > memCapacity_;
		if (expandNeed)
		{
			if (!EnsureCapacity(requiredMemCapacity))
			{
				jc_assert(false);
				return;
			}
		}

		if (expandNeed)
		{
			pHeader = GetMsgHeaderPtr(); // 버퍼 확장되었으니 헤더 위치 갱신
		}
		_u8* pWrite = pBuf_ + HEADER_SIZE + pHeader->writeMemOffset_;

		// 1) 타입 코드 기록
		*pWrite = static_cast<_u8>(TypeTraits<T>::VARIANT_TYPE);
		++pWrite;

		// 2) 실제 값 기록
		*reinterpret_cast<T*>(pWrite) = _value;

		++pHeader->writeOffset_;
		pHeader->writeMemOffset_ += ELEM_SIZE;
	}

	// 기본 Read: 타입 불일치 또는 범위 초과 시 assert 후 false 반환
	template <template <typename> class TypeTraits, typename T>
	T ReadValue()
	{
		T value{};
		int result = TryReadValue<TypeTraits, T>(value);
		switch (result)
		{
		case 0:
			// ok
			break;
		case -1:
			jc_assert_msg(false, "CMessageContext::ReadValue - no more elements to read");
			break;
		case -2:
			jc_assert_msg(false, "CMessageContext::ReadValue - not enough bytes for element");
			break;
		case -3:
			jc_assert_msg(false, "CMessageContext::ReadValue - variant type mismatch");
			break;
		default:
			jc_assert_msg(false, "CMessageContext::ReadValue - unknown read error");
			break;
		}
		return value;
	}

	// 안전한 TryRead: 타입/범위 체크 후 실패 시 false 반환
	template <template <typename> class TypeTraits, typename T>
	int TryReadValue(T& _outValue)
	{
		static_assert(TypeTraits<T>::VARIANT_TYPE != 0, "Unsupported type for CMessageContext::TryReadValue");
		const CMessageHeader& header = GetMsgHeader();
		const _u16 HEADER_SIZE = GetHeaderSize();

		// 요소 갯수 기준으로 먼저 범위 체크
		if (readOffset_ >= header.writeOffset_) 
			return -1;

		// 남은 데이터가 최소 타입 코드 + 값 크기보다 작은 경우 실패
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

	void	WriteString(const String& _str);
	void	WriteBinary(const _u8* _pBytes, _u32 _len);
	void	WriteBinaryImpl(CMessage::VariantType _type, const _u8* _pBytes, _u32 _len);

	String	ReadString();
	bool	ReadBinary(Span<_u8> _buffer, OUT _u32& _outLen);
	bool 	ReadBinary(_u8* _pBytes, _u32 _capacity, OUT _u32& _outLen);

	bool	TryReadString(OUT String& _value);
	bool	TryReadBinary(Span<_u8> _buffer, OUT _u32& _outLen);
	bool	TryReadBinary(_u8* _pBytes, _u32 _capacity, OUT _u32& _outLen);

	int		TryReadBinaryImpl(CMessage::VariantType _expectedType, _u8** _ppBuf, _u32 _capacity, OUT _u32& _outLen);
	const char* GetBinaryReadErrorMessage(int _errorCode);

	jc::String Dump() const;

	_u32	prefixMemCapacity_ = 0;			// 접두 메모리 크기
	_u32	memCapacity_ = 0;				// 전체 메모리 용량
	_u32	readMemOffset_ = 0;				// 요소 메모리를 얼만큼 읽었는지
	_u16	readOffset_ = 0;				// 요소 메모리를 몇개나 읽었는지
	_u8*	pBuf_ = nullptr;

	friend class CMessage;
};

NS_END
