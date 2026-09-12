/*
 * 작성자: 윤정도
 * 생성일: 9/6/2026
 * =====================
 * BasicString<CharT, Storage>용 inline SSO Storage (24B, S1 계열)
 *
 * - SSO_CAP 22 (char) / 10 (wchar_t). S1 벤치와 동일 용량이다.
 * - 24B union + 꼬리 마크 방식. sso_[23] 바이트가 0xFF면 힙 모드다.
 *   SSO 모드에서 len은 최대 22라 마크와 절대 겹치지 않는다.
 * - 힙 모드: pHeap_ + len_ + cap_ (16B) + 패딩 8B (꼬리 바이트에 마크 유지).
 * - 빈 문자열도 SSO 모드다 (풀 Storage의 nullptr 방식과 다름).
 * - 스레드 공유 상태가 없어서 cross-thread 추가 비용이 없다.
 */

#pragma once

#include <cstring>
#include <cstddef>

#include "jc/Define.h"
#include "jc/Macro.h"
#include "jc/Type.h"
#include "jc/Assert.h"
#include "jc/Memory.h"
#include "jc/Primitives/BasicString.h"

NS_JC_BEGIN

template <typename CharT>
class StringImpl_SSO
{
public:
	using value_type = CharT;

	static constexpr int kTotalBytes = 24;
	static constexpr int kSsoUnits = kTotalBytes / sizeof(CharT);	// char: 24, wchar_t: 12
	static constexpr int kSsoCapacity = kSsoUnits - 2;				// char: 22, wchar_t: 10 (내용. +NUL +len 1단위)
	static constexpr unsigned char kHeapMark = 0xFF;
	static constexpr int kDefaultBufferSize = 32;
	static constexpr int kExpandingFactor = 2;
	StringImpl_SSO();												// SSO 빈 문자열
	StringImpl_SSO(const StringImpl_SSO& _other);
	StringImpl_SSO(StringImpl_SSO&& _other) noexcept;
	~StringImpl_SSO();
	StringImpl_SSO& operator=(const StringImpl_SSO& _other);
	StringImpl_SSO& operator=(StringImpl_SSO&& _other) noexcept;

	const CharT* Data() const;
	CharT* Data();
	int Len() const;
	void SetLen(int _len);
	int Cap() const;					// NUL 제외. SSO 모드는 kSsoCapacity
	void Reserve(int _capacity);		// 축소 없음. 정확히 맞춤 (증가 정책은 BasicString 몫)
	void Assign(const CharT* _pStr, int _len);
	void AssignFresh(const CharT* _pStr, int _len);	// 막 만든 Storage 전용. 겹침 없음. SSO 범위 보장.
	void Clear();						// SSO 빈 문자열로 (힙 버퍼 있으면 해제)
	void Adopt(CharT* _pBuffer, int _len, int _capacity);

private:
	union U
	{
		struct
		{
			CharT*	pHeap_;
			int		len_;
			int		cap_;
		} heap_;
		CharT sso_[kSsoUnits];
	};

	bool IsHeap() const { return reinterpret_cast<const _u8*>(u_.sso_)[kTotalBytes - 1] == kHeapMark; }
	int SsoLen() const { return static_cast<int>(u_.sso_[kSsoUnits - 1]); }
	void SetSsoLen(int _len) { u_.sso_[kSsoUnits - 1] = static_cast<CharT>(_len); }
	void SetHeapMark() { reinterpret_cast<_u8*>(u_.sso_)[kTotalBytes - 1] = kHeapMark; }
	CharT* Buf() { return IsHeap() ? u_.heap_.pHeap_ : u_.sso_; }
	const CharT* Buf() const { return IsHeap() ? u_.heap_.pHeap_ : u_.sso_; }
	void FreeHeapBuf();
	void InitEmpty();

	U u_;
};
static_assert(sizeof(StringImpl_SSO<char>) == 24);
static_assert(sizeof(StringImpl_SSO<wchar_t>) == 24);
static_assert(StringImpl_SSO<char>::kSsoCapacity == 22);
static_assert(StringImpl_SSO<wchar_t>::kSsoCapacity == 10);

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline StringImpl_SSO<CharT>::StringImpl_SSO()
{
	InitEmpty();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline StringImpl_SSO<CharT>::StringImpl_SSO(const StringImpl_SSO& _other)
{
	const int len = _other.Len();
	if (_other.IsHeap())
	{
		// S1과 동일: 복사 시 len*2 (단 빈 문자열은 32).
		const int cap = len == 0 ? kDefaultBufferSize : (len * kExpandingFactor < kDefaultBufferSize ? kDefaultBufferSize : len * kExpandingFactor);
		CharT* pBuf = Memory::Allocate<CharT*>(static_cast<_u32>((cap + 1) * sizeof(CharT)));
		if (len > 0)
		{
			memcpy(pBuf, _other.u_.heap_.pHeap_, static_cast<size_t>(len + 1) * sizeof(CharT));
		}
		else
		{
			pBuf[0] = CharT(0);
		}
		u_.heap_.pHeap_ = pBuf;
		u_.heap_.len_ = len;
		u_.heap_.cap_ = cap;
		SetHeapMark();
	}
	else
	{
		memcpy(u_.sso_, _other.u_.sso_, static_cast<size_t>(len + 1) * sizeof(CharT));
		SetSsoLen(len);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline StringImpl_SSO<CharT>::StringImpl_SSO(StringImpl_SSO&& _other) noexcept
{
	// S1과 동일: heap은 steal, SSO는 len+1만 복사 (union 전체가 아님).
	if (_other.IsHeap())
	{
		u_.heap_.pHeap_ = _other.u_.heap_.pHeap_;
		u_.heap_.len_ = _other.u_.heap_.len_;
		u_.heap_.cap_ = _other.u_.heap_.cap_;
		SetHeapMark();
		_other.InitEmpty();
	}
	else
	{
		memcpy(u_.sso_, _other.u_.sso_, static_cast<size_t>(_other.Len() + 1) * sizeof(CharT));
		SetSsoLen(_other.Len());
		_other.InitEmpty();
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline StringImpl_SSO<CharT>::~StringImpl_SSO()
{
	FreeHeapBuf();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline StringImpl_SSO<CharT>& StringImpl_SSO<CharT>::operator=(const StringImpl_SSO& _other)
{
	// S1과 동일: self-check 없음 (자기 복사는 memcpy로 안전).
	// heap-src는 모드 유지 + 필요시만 확장, SSO-src는 힙 해제 후 SSO 복사.
	if (_other.IsHeap())
	{
		Reserve(_other.Len());
		memcpy(Buf(), _other.Data(), static_cast<size_t>(_other.Len() + 1) * sizeof(CharT));
		if (IsHeap())
		{
			u_.heap_.len_ = _other.Len();
		}
		else
		{
			SetSsoLen(_other.Len());
		}
	}
	else
	{
		FreeHeapBuf();
		memcpy(u_.sso_, _other.u_.sso_, static_cast<size_t>(_other.Len() + 1) * sizeof(CharT));
		SetSsoLen(_other.Len());
	}
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline StringImpl_SSO<CharT>& StringImpl_SSO<CharT>::operator=(StringImpl_SSO&& _other) noexcept
{
	// steal. 원본은 비운다. 복사는 len+1만 (union 전체가 아님).
	if (this == &_other)
	{
		return *this;
	}
	if (_other.IsHeap())
	{
		FreeHeapBuf();
		u_.heap_.pHeap_ = _other.u_.heap_.pHeap_;
		u_.heap_.len_ = _other.u_.heap_.len_;
		u_.heap_.cap_ = _other.u_.heap_.cap_;
		SetHeapMark();
		_other.InitEmpty();
	}
	else
	{
		FreeHeapBuf();
		memcpy(u_.sso_, _other.u_.sso_, static_cast<size_t>(_other.Len() + 1) * sizeof(CharT));
		SetSsoLen(_other.Len());
		_other.InitEmpty();
	}
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline const CharT* StringImpl_SSO<CharT>::Data() const
{
	return Buf();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline CharT* StringImpl_SSO<CharT>::Data()
{
	return Buf();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline int StringImpl_SSO<CharT>::Len() const
{
	return IsHeap() ? u_.heap_.len_ : SsoLen();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSO<CharT>::SetLen(int _len)
{
	// NUL 종결은 호출자 책임 (BasicString::CommitLen). SetLen은 길이만 둔다. S1과 동일.
	if (IsHeap())
	{
		jc_assert(_len >= 0 && _len <= u_.heap_.cap_);
		u_.heap_.len_ = _len;
	}
	else
	{
		jc_assert(_len >= 0 && _len <= kSsoCapacity);
		SetSsoLen(_len);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline int StringImpl_SSO<CharT>::Cap() const
{
	return IsHeap() ? u_.heap_.cap_ : kSsoCapacity;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSO<CharT>::Reserve(int _capacity)
{
	jc_assert(_capacity >= 0);
	if (_capacity <= Cap())
	{
		return;						// 축소 없음
	}

	const int oldLen = Len();
	CharT* pNew = Memory::Allocate<CharT*>(static_cast<_u32>((_capacity + 1) * sizeof(CharT)));
	memcpy(pNew, Buf(), static_cast<size_t>(oldLen + 1) * sizeof(CharT));
	FreeHeapBuf();
	u_.heap_.pHeap_ = pNew;
	u_.heap_.len_ = oldLen;
	u_.heap_.cap_ = _capacity;
	SetHeapMark();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSO<CharT>::Assign(const CharT* _pStr, int _len)
{
	if (_pStr == nullptr || _len <= 0)
	{
		Clear();
		return;
	}

	const CharT* pOldBuf = Buf();
	const bool overlapped = (_pStr >= pOldBuf && _pStr < pOldBuf + Cap() + 1);
	if (overlapped)
	{
		jc_assert(_pStr + _len <= pOldBuf + Len() + 1);
	}

	if (overlapped)
	{
		// 겹치면 Reserve가 기존 버퍼를 해제할 수 있으니 임시 버퍼를 거친다.
		// 임시 복사는 Reserve 전에 한다 (_pStr이 기존 버퍼 안이라 해제되면 댕글링).
		CharT* pTemp = Memory::Allocate<CharT*>(static_cast<_u32>((_len + 1) * sizeof(CharT)));
		memcpy(pTemp, _pStr, static_cast<size_t>(_len + 1) * sizeof(CharT));
		Reserve(_len);
		memcpy(Buf(), pTemp, static_cast<size_t>(_len + 1) * sizeof(CharT));
		Memory::Deallocate(pTemp);
	}
	else
	{
		Reserve(_len);
		memcpy(Buf(), _pStr, static_cast<size_t>(_len + 1) * sizeof(CharT));
	}

	if (IsHeap())
	{
		u_.heap_.len_ = _len;
	}
	else
	{
		SetSsoLen(_len);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSO<CharT>::Clear()
{
	FreeHeapBuf();
	InitEmpty();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSO<CharT>::AssignFresh(const CharT* _pStr, int _len)
{
	jc_assert(_pStr != nullptr && _len >= 0 && _len <= kSsoCapacity);
	memcpy(u_.sso_, _pStr, static_cast<size_t>(_len + 1) * sizeof(CharT));
	SetSsoLen(_len);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSO<CharT>::Adopt(CharT* _pBuffer, int _len, int _capacity)
{
	if (_pBuffer == nullptr)
	{
		Clear();
		return;
	}

	jc_assert(_len >= 0 && _capacity >= _len && _pBuffer[_len] == CharT(0));
	FreeHeapBuf();
	if (_capacity <= kSsoCapacity)
	{
		memcpy(u_.sso_, _pBuffer, static_cast<size_t>(_len + 1) * sizeof(CharT));
		SetSsoLen(_len);
		Memory::Deallocate(_pBuffer);
		return;
	}
	u_.heap_.pHeap_ = _pBuffer;
	u_.heap_.len_ = _len;
	u_.heap_.cap_ = _capacity;
	SetHeapMark();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSO<CharT>::FreeHeapBuf()
{
	if (IsHeap())
	{
		Memory::Deallocate(u_.heap_.pHeap_);
		u_.heap_.pHeap_ = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSO<CharT>::InitEmpty()
{
	u_.sso_[0] = CharT(0);
	SetSsoLen(0);
}

// 일상 별칭은 String.h 허브에 있다.
NS_END
