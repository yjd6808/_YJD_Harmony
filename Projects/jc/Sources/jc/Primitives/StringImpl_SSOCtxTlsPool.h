/*
 * 작성자: 윤정도
 * 생성일: 9/6/2026
 * =====================
 * BasicString<CharT, Storage>용 Pooled SSO Storage (S8 정책)
 *
 * - 8B 핸들(StringImpl 객체가 들고 있는 슬롯 포인터 1개) + 64B 슬롯.
 * - 슬롯은 ObjectPoolTLSIndexed<StringSlot<CharT>>에서 Pop/Push한다.
 * - 빈 문자열은 슬롯을 갖지 않는다 (pCtx_ == nullptr). Data() const는 정적 EmptyStr을 반환.
 * - 쓰기 전에는 BasicString이 Reserve() (또는 non-const Data())를 부른다는 계약이다.
 * - 소멸은 2단계다. 힙 버퍼를 먼저 해제하고 슬롯을 풀에 돌려준다.
 *   순서가 바뀌면 풀 링크가 pHeap_ 자리를 덮어 힙 버퍼 주소가 사라진다 (릭).
 *
 * [슬롯 레이아웃: 64B]
 *  offset 0  : poolIdx_ (TlsPoolNode)  ← 풀 전용. 프리 상태에서도 보존
 *  offset 4  : len_                    ← 프리 상태에서 FreeNode::reserved_ 자리. 풀이 건드리지 않음
 *  offset 8  : pHeap_                  ← 프리 상태에서 FreeNode::pNext_가 덮어쓴다
 *  offset 16 : cap_                    ← 힙 모드 용량 (NUL 제외). SSO 모드는 pHeap_ == nullptr로 구별
 *  offset 20 : sso_[44]
 */

#pragma once

#include <cstring>
#include <cstddef>

#include "jc/Define.h"
#include "jc/Macro.h"
#include "jc/Type.h"
#include "jc/Assert.h"
#include "jc/Memory.h"
#include "jc/Pool/ObjectPoolTLSIndexed.h"
#include "jc/Primitives/BasicString.h"

NS_JC_BEGIN

template <typename CharT>
struct StringSlot : TlsPoolNode
{
	static constexpr int kHeapHeaderSize = sizeof(CharT*) + sizeof(int) * 2;							// 16
	static constexpr int kSsoBytes       = JC_CACHE_ALIGN_SIZE - sizeof(TlsPoolNode) - kHeapHeaderSize;	// 64 - 4 - 16 = 44
	static constexpr int kSsoCapacity    = kSsoBytes / sizeof(CharT) - 1;								// char: 43, wchar_t: 21 (NUL 제외)
	// offset 4 : len_
	int		len_;
	// offset 8 : pHeap_ (프리 상태에서 풀 링크가 덮어쓴다)
	CharT*	pHeap_;
	// offset 16 : cap_
	int		cap_;
	// offset 20 : sso_
	CharT	sso_[kSsoBytes / sizeof(CharT)];

	bool IsHeap() const { return pHeap_ != nullptr; }
	CharT* Buf() { return IsHeap() ? pHeap_ : sso_; }
	const CharT* Buf() const { return IsHeap() ? pHeap_ : sso_; }
	int Cap() const { return IsHeap() ? cap_ : kSsoCapacity; }
	void InitEmpty() { len_ = 0; pHeap_ = nullptr; cap_ = 0; sso_[0] = CharT(0); }
};
static_assert(sizeof(StringSlot<char>) == JC_CACHE_ALIGN_SIZE);
static_assert(sizeof(StringSlot<wchar_t>) == JC_CACHE_ALIGN_SIZE);
static_assert(StringSlot<char>::kSsoBytes == 44);
static_assert(StringSlot<char>::kSsoCapacity == 43);
static_assert(StringSlot<wchar_t>::kSsoCapacity == 21);
static_assert(offsetof(StringSlot<char>, pHeap_) == 8);
static_assert(offsetof(StringSlot<wchar_t>, pHeap_) == 8);

template <typename CharT>
JC_FORCEINLINE const CharT* EmptyStr()
{
	static constexpr CharT kEmpty[1] = { CharT(0) };
	return kEmpty;
}

template <typename CharT>
class StringImpl_SSOCtxTlsPool
{
public:
	using value_type = CharT;
	using TSlot = StringSlot<CharT>;
	using TPool = ObjectPoolTLSIndexed<TSlot>;
	static constexpr int kSsoCapacity = TSlot::kSsoCapacity;	// Storage 계약 상수 (BasicString SSO 게이트용)

	StringImpl_SSOCtxTlsPool() = default;									// pCtx_ == nullptr : 빈 문자열. 풀 안 건듦.
	StringImpl_SSOCtxTlsPool(const StringImpl_SSOCtxTlsPool& _other);		// _other가 비어있으면 풀 안 건듦
	StringImpl_SSOCtxTlsPool(StringImpl_SSOCtxTlsPool&& _other) noexcept;	// 슬롯 포인터만 넘김
	~StringImpl_SSOCtxTlsPool();
	StringImpl_SSOCtxTlsPool& operator=(const StringImpl_SSOCtxTlsPool& _other);
	StringImpl_SSOCtxTlsPool& operator=(StringImpl_SSOCtxTlsPool&& _other) noexcept;

	const CharT* Data() const;			// 슬롯 없으면 EmptyStr<CharT>()
	CharT* Data();						// 쓰기 전에 Reserve하도록 BasicString이 보증. 슬롯 없으면 Ctx() 획득.
	int Len() const;
	void SetLen(int _len);				// Buf()[_len] = 0 포함
	int Cap() const;					// NUL 제외. 슬롯 없으면 0
	void Reserve(int _capacity);		// 축소 없음. 필요시 Ctx() 획득. SSO → 힙 승급. 정확히 맞춤 (증가 정책은 BasicString 몫)
	void Assign(const CharT* _pStr, int _len);	// 깊은 복사. self/overlap 안전
	void AssignFresh(const CharT* _pStr, int _len);	// 막 만든 Storage 전용. 겹침 없음. 슬롯 획득 포함.
	void Clear();						// 슬롯 유지, len_ = 0
	void Adopt(CharT* _pBuffer, int _len, int _capacity);	// 외부 힙 버퍼 소유권 인수 (Format 등)

private:
	TSlot* Ctx();						// pCtx_가 null이면 TPool::Pop() + InitEmpty()
	void FreeHeapBuf();					// pHeap_ 있으면 Memory::Deallocate, 없게
	void ReleaseCtx();					// FreeHeapBuf() 후 TPool::Push(pCtx_), pCtx_ = nullptr. 이 순서 필수.

	TSlot* pCtx_ = nullptr;
};

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline StringImpl_SSOCtxTlsPool<CharT>::StringImpl_SSOCtxTlsPool(const StringImpl_SSOCtxTlsPool& _other)
{
	if (_other.pCtx_ == nullptr)
	{
		return;
	}

	const TSlot* pSrc = _other.pCtx_;
	TSlot* pDst = Ctx();
	pDst->len_ = pSrc->len_;
	if (pSrc->IsHeap())
	{
		const int cap = pSrc->cap_;
		CharT* pBuf = Memory::Allocate<CharT*>(static_cast<_u32>((cap + 1) * sizeof(CharT)));
		memcpy(pBuf, pSrc->pHeap_, static_cast<size_t>(pSrc->len_ + 1) * sizeof(CharT));
		pDst->pHeap_ = pBuf;
		pDst->cap_ = cap;
	}
	else
	{
		memcpy(pDst->sso_, pSrc->sso_, static_cast<size_t>(pSrc->len_ + 1) * sizeof(CharT));
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline StringImpl_SSOCtxTlsPool<CharT>::StringImpl_SSOCtxTlsPool(StringImpl_SSOCtxTlsPool&& _other) noexcept
	: pCtx_(_other.pCtx_)
{
	_other.pCtx_ = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline StringImpl_SSOCtxTlsPool<CharT>::~StringImpl_SSOCtxTlsPool()
{
	ReleaseCtx();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline StringImpl_SSOCtxTlsPool<CharT>& StringImpl_SSOCtxTlsPool<CharT>::operator=(const StringImpl_SSOCtxTlsPool& _other)
{
	if (this == &_other)
	{
		return *this;
	}
	if (_other.pCtx_ == nullptr)
	{
		Clear();
		return *this;
	}
	Assign(_other.Data(), _other.Len());	// 다른 슬롯이라 겹침 없음. Reserve + 복사 재사용.
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline StringImpl_SSOCtxTlsPool<CharT>& StringImpl_SSOCtxTlsPool<CharT>::operator=(StringImpl_SSOCtxTlsPool&& _other) noexcept
{
	// swap. 풀 트래픽 0 (Sort 이동 폭풍 대응. §9.3 원결정 유지).
	// 내 옛 슬롯은 _other로 가서 소멸 시 반납된다. self-move여도 안전.
	// SSO steal과 의미가 다르다 (원본이 이전 값을 물려받음). P6 L240 carve-out 참조.
	TSlot* pTemp = pCtx_;
	pCtx_ = _other.pCtx_;
	_other.pCtx_ = pTemp;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline const CharT* StringImpl_SSOCtxTlsPool<CharT>::Data() const
{
	return pCtx_ != nullptr ? pCtx_->Buf() : EmptyStr<CharT>();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline CharT* StringImpl_SSOCtxTlsPool<CharT>::Data()
{
	return Ctx()->Buf();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline int StringImpl_SSOCtxTlsPool<CharT>::Len() const
{
	return pCtx_ != nullptr ? pCtx_->len_ : 0;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSOCtxTlsPool<CharT>::SetLen(int _len)
{
	// NUL 종결은 호출자 책임 (BasicString::CommitLen). SetLen은 길이만 둔다. S1과 동일.
	TSlot* pCtx = Ctx();
	jc_assert(_len >= 0 && _len <= pCtx->Cap());
	pCtx->len_ = _len;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline int StringImpl_SSOCtxTlsPool<CharT>::Cap() const
{
	return pCtx_ != nullptr ? pCtx_->Cap() : 0;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSOCtxTlsPool<CharT>::Reserve(int _capacity)
{
	jc_assert(_capacity >= 0);
	if (_capacity <= Cap())
	{
		return;						// 축소 없음
	}

	TSlot* pCtx = Ctx();
	if (_capacity <= TSlot::kSsoCapacity)
	{
		return;						// 갓 만든 슬롯은 SSO. (위에서 안 걸렸다는 건 이미 그 이상이라는 뜻)
	}

	CharT* pNew = Memory::Allocate<CharT*>(static_cast<_u32>((_capacity + 1) * sizeof(CharT)));
	memcpy(pNew, pCtx->Buf(), static_cast<size_t>(pCtx->len_ + 1) * sizeof(CharT));
	FreeHeapBuf();
	pCtx->pHeap_ = pNew;
	pCtx->cap_ = _capacity;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSOCtxTlsPool<CharT>::Assign(const CharT* _pStr, int _len)
{
	if (_pStr == nullptr || _len <= 0)
	{
		Clear();
		return;
	}

	// Reserve가 기존 버퍼를 해제할 수 있으니 겹침을 먼저 판정한다.
	bool overlapped = false;
	if (pCtx_ != nullptr)
	{
		const CharT* pOldBuf = pCtx_->Buf();
		overlapped = (_pStr >= pOldBuf && _pStr < pOldBuf + pCtx_->Cap() + 1);
		if (overlapped)
		{
			jc_assert(_pStr + _len <= pOldBuf + pCtx_->len_ + 1);	// 자기 내용+NUL 범위를 벗어난 길이는 호출자 버그
		}
	}

	if (overlapped)
	{
		CharT* pTemp = Memory::Allocate<CharT*>(static_cast<_u32>((_len + 1) * sizeof(CharT)));
		memcpy(pTemp, _pStr, static_cast<size_t>(_len + 1) * sizeof(CharT));
		Reserve(_len);
		memcpy(Ctx()->Buf(), pTemp, static_cast<size_t>(_len + 1) * sizeof(CharT));
		Memory::Deallocate(pTemp);
	}
	else
	{
		Reserve(_len);
		memcpy(Ctx()->Buf(), _pStr, static_cast<size_t>(_len + 1) * sizeof(CharT));
	}
	pCtx_->len_ = _len;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSOCtxTlsPool<CharT>::AssignFresh(const CharT* _pStr, int _len)
{
	jc_assert(_pStr != nullptr && _len >= 0);
	TSlot* pCtx = Ctx();
	memcpy(pCtx->Buf(), _pStr, static_cast<size_t>(_len + 1) * sizeof(CharT));
	pCtx->len_ = _len;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSOCtxTlsPool<CharT>::Clear()
{
	if (pCtx_ != nullptr)
	{
		pCtx_->len_ = 0;
		pCtx_->Buf()[0] = CharT(0);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSOCtxTlsPool<CharT>::Adopt(CharT* _pBuffer, int _len, int _capacity)
{
	if (_pBuffer == nullptr)
	{
		Clear();
		return;
	}

	jc_assert(_len >= 0 && _capacity >= _len && _pBuffer[_len] == CharT(0));
	TSlot* pCtx = Ctx();
	FreeHeapBuf();
	pCtx->pHeap_ = _pBuffer;
	pCtx->cap_ = _capacity;
	pCtx->len_ = _len;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
typename StringImpl_SSOCtxTlsPool<CharT>::TSlot* StringImpl_SSOCtxTlsPool<CharT>::Ctx()
{
	if (pCtx_ == nullptr)
	{
		pCtx_ = TPool::Pop();
		pCtx_->InitEmpty();
	}
	return pCtx_;
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSOCtxTlsPool<CharT>::FreeHeapBuf()
{
	if (pCtx_ != nullptr && pCtx_->pHeap_ != nullptr)
	{
		Memory::Deallocate(pCtx_->pHeap_);
		pCtx_->pHeap_ = nullptr;
		pCtx_->cap_ = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename CharT>
inline void StringImpl_SSOCtxTlsPool<CharT>::ReleaseCtx()
{
	if (pCtx_ == nullptr)
	{
		return;
	}
	FreeHeapBuf();
	jc_assert(pCtx_->pHeap_ == nullptr);		// §9.4: 링크가 덮기 전에 힙 버퍼가 먼저 나가야 한다.
	TPool::Push(pCtx_);
	pCtx_ = nullptr;
}

// 일상 별칭. using String = BasicString<...> 교체(step 6) 전까지 PooledString으로 쓴다.
using PooledString = BasicString<char, StringImpl_SSOCtxTlsPool<char>>;

NS_END
