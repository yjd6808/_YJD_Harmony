/*
 * StrForBench_SImpl.inl: S5/S6/S7 공용 클래스 본문.
 * 헤더 가드 없음. 네임스페이스 블록을 열지 않는다.
 * 호출자가 `namespace jc_gbench { namespace STRFORBENCH_NS {` 안에서 include하고,
 * StrBenchImpl(pHeap_/len_/cap_/owner_/sso_[44]) 구조체와
 * PoolNewImpl() / PoolReleaseImpl(StrBenchImpl*) 헬퍼를 먼저 정의해야 한다.
 * SSO_CAP은 43이다.
 */

class StrForBench final
{
	inline static constexpr int DEFAULT_BUFFER_SIZE = 32;
	inline static constexpr int EXPANDING_FACTOR = 2;
	static constexpr int SSO_CAP = 43;

	inline static const char* EmptySource = "";
public:
	static const StrForBench Empty;
	static const StrForBench Null;

	StrForBench();
	explicit StrForBench(int _capacity);
	StrForBench(const char* _pStr, int _capacity);
	StrForBench(const char* _pStr);
	StrForBench(char _ch, int _count);
	StrForBench(const std::string& _str);
	StrForBench(const StrForBench& _str);
	StrForBench(StrForBench&& _str) noexcept;
	~StrForBench();
public:
	char* Source() const { return Data(); }
	const char* SafeSource() const { return Data() != nullptr ? Data() : EmptySource; }
	int Capacity() const { return Cap(); }
	int Length() const { return Len(); }
	int LengthWithNull() const { return Len() + 1; }
	void SetLength(int _length) { if (pCtx_) pCtx_->len_ = _length; }
	void ExchangeSource(char* _pSrc, int _len);
	bool IsEmpty() const { return Len() == 0; }
	bool IsNull() const { return pCtx_ == nullptr; }
	bool IsValidIndex(const int _idx) const { return _idx >= 0 && _idx < Len(); }
	bool IsValidIndexRange(const int _startIdx, const int _endIdx) const
	{
		return _startIdx <= _endIdx && _startIdx >= 0 && _endIdx < Len();
	}

	template <typename T>
	void Append(const T& _str)
	{
		std::string s = std::to_string(_str);
		Append(s.c_str(), (int)s.length());
	}
	void Append(char _ch);
	void Append(const char* _pStr);
	void Append(const char* _pStr, int _len);
	void Append(char* _pStr);
	void Append(const std::string& _str);
	void Append(const StrForBench& _str);
	void Append(const StringView& _str);
	void Append(StrForBench&& _str);

	void Insert(int _idx, const char* _pStr);
	void Insert(int _idx, const StrForBench& _str);

	void Resize(int _capacity);
	void ResizeIfNeeded(int _length);

	int Compare(const StrForBench& _str) const;
	int Compare(const char* _pStr, int _strLen = -1) const;
	Vector<int, CDefaultAllocator> FindAll(int _startIdx, int _endIdx, const char* _pStr, bool _caseSensitive = true) const;
	Vector<int, CDefaultAllocator> FindAll(const char* _pStr, bool _caseSensitive = true) const;
	Vector<int, CDefaultAllocator> FindAll(const StrForBench& _str, bool _caseSensitive = true) const;
	int Find(int _startIdx, int _endIdx, const char* _pStr, bool _caseSensitive = true) const;
	int Find(int _startIdx, const char* _pStr, bool _caseSensitive = true) const;
	int Find(int _startIdx, const StrForBench& _str, bool _caseSensitive = true) const;
	int Find(const char* _pStr, bool _caseSensitive = true) const;
	int Find(const StrForBench& _str, bool _caseSensitive = true) const;
	int FindReverse(int _startIdx, int _endIdx, const char* _pStr, bool _caseSensitive = true) const;
	int FindReverse(const StrForBench& _str, bool _caseSensitive = true) const;
	int FindReverse(const char* _pStr, bool _caseSensitive = true) const;

	bool EndWith(const StrForBench& _str, bool _caseSensitive = true) const { return FindReverse(_str.Source(), _caseSensitive) == Len() - _str.Length(); }
	bool StartWith(const StrForBench& _str, bool _caseSensitive = true) const { return Find(_str, _caseSensitive) == 0; }

	char Last() const { return GetAt(Len() - 1); }
	char First() const { return GetAt(0); }

	void Clear();
	void Clear(int _offset, int _length);

	int Count(const char* _pStr, bool _caseSensitive = true) const;
	int Count(const StrForBench& _value, bool _caseSensitive = true) const;
	int Count(int _startIdx, int _endIdx, const char* _pStr, bool _caseSensitive = true) const;
	int Count(int _startIdx, int _endIdx, const StrForBench& _value, bool _caseSensitive = true) const;

	int Replace(const char* _pFrom, const StrForBench& _to, bool _caseSensitive = true);
	int Replace(const StrForBench& _from, const StrForBench& _to, bool _caseSensitive = true);
	int Replace(int _offset, int _length, const StrForBench& _to);
	int Replace(int _offset, const char* _pFrom, const StrForBench& _to, bool _caseSensitive = true);
	int Replace(int _offset, const StrForBench& _from, const StrForBench& _to, bool _caseSensitive = true);
	void ReplaceAll(const char* _pFrom, const char* _pTo, bool _caseSensitive = true);

	bool Contain(const char* _pStr) const;
	bool Contain(const StrForBench& _str) const;
	void Format(const char* _format, ...);

	void SetAt(int _idx, char _ch);
	void SetAtForce(int _idx, char _ch);
	char GetAt(int _idx) const;
	StrForBench GetRange(int _startIdx, int _endIdx) const;
	StrForBench SubStr(int _startIdx, int _count) const;

	Tuple<char*, int, int> GetRangeUnsafe(int _startIdx, int _endIdx) const;
	Vector<StrForBench, CDefaultAllocator> Split(const char* _delimiter, bool _includeEmpty = false) const;
	Vector<StrForBench, CDefaultAllocator> Split(char _delimiter, bool _includeEmpty = false) const;

	void Initialize(int _capacity = DEFAULT_BUFFER_SIZE);

	StrForBench ToLowerCase() const;
	StrForBench ToUpperCase() const;
	int LeadingZeroCount() const;

	std::string ToStd();

	_s8 ToInt8(bool _ignoreLeadingZero = true) const;
	_u8 ToUInt8(bool _ignoreLeadingZero = true) const;
	_s16 ToInt16(bool _ignoreLeadingZero = true) const;
	_u16 ToUInt16(bool _ignoreLeadingZero = true) const;
	_s32 ToInt32(bool _ignoreLeadingZero = true) const;
	_u32 ToUInt32(bool _ignoreLeadingZero = true) const;
	_s64 ToInt64(bool _ignoreLeadingZero = true) const;
	_u64 ToUInt64(bool _ignoreLeadingZero = true) const;
	_f32 ToFloat(bool _ignoreLeadingZero = true) const;
	_f64 ToDouble(bool _ignoreLeadingZero = true) const;

	bool TryToInt8(OUT _s8& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToUInt8(OUT _u8& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToInt16(OUT _s16& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToUInt16(OUT _u16& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToInt32(OUT _s32& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToUInt32(OUT _u32& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToInt64(OUT _s64& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToUInt64(OUT _u64& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToFloat(OUT _f32& _outValue, bool _ignoreLeadingZero = true) const;
	bool TryToDouble(OUT _f64& _outValue, bool _ignoreLeadingZero = true) const;

	char PopBack();

public:
	char& operator[](int _idx) const;

	template <typename T>
	StrForBench operator+(const T& _other) {
		StrForBench temp = *this;
		temp.Append(_other);
		return temp;
	}

	StrForBench operator+(const StrForBench& _other) const;
	StrForBench operator+(char _ch) const;
	StrForBench operator+(const char* _pStr) const;

	template <typename T>
	StrForBench& operator+=(const T& _other) { Append(_other); return *this; }
	StrForBench& operator+=(const StrForBench& _other);
	StrForBench& operator+=(char _ch);
	StrForBench& operator+=(char* _pStr);
	StrForBench& operator+=(const char* _pStr);

	template <typename T>
	StrForBench& operator=(const T& _other) {
		if (pCtx_ == nullptr)
			Initialize();

		Clear();
		Append(_other);
		return *this;
	}

	StrForBench& operator=(const StrForBench& _other);
	StrForBench& operator=(StrForBench&& _other) noexcept;
	StrForBench& operator=(const char* _pOther);
	StrForBench& operator=(std::nullptr_t _other);
	bool operator==(const StrForBench& _other) const;
	bool operator==(const char* _pOther) const;

	bool operator<(const StrForBench& _other) const;
	bool operator<(const char* _pOther) const;
	bool operator>(const StrForBench& _other) const;
	bool operator>(const char* _pOther) const;
	bool operator<=(const StrForBench& _other) const;
	bool operator<=(const char* _pOther) const;
	bool operator>=(const StrForBench& _other) const;
	bool operator>=(const char* _pOther) const;

	friend std::ostream& operator<<(std::ostream& _os, const StrForBench& _src);
private:
	bool IsHeap() const { return pCtx_ && pCtx_->pHeap_ != nullptr; }
	char* Data() const { if (!pCtx_) return nullptr; return pCtx_->pHeap_ ? pCtx_->pHeap_ : pCtx_->sso_; }
	int Len() const { return pCtx_ ? pCtx_->len_ : 0; }
	int Cap() const { if (!pCtx_) return 0; return pCtx_->pHeap_ ? pCtx_->cap_ : SSO_CAP; }
	void SetLen(int _len) { pCtx_->len_ = _len; }
	StrBenchImpl& Ctx() { if (!pCtx_) pCtx_ = PoolNewImpl(); return *pCtx_; }
	void FreeHeapBuf()
	{
		if (pCtx_ && pCtx_->pHeap_)
		{
			JC_DELETE_ARRAY_SAFE(pCtx_->pHeap_);
			pCtx_->pHeap_ = nullptr;
		}
	}
	void FreeCtx()
	{
		if (pCtx_)
		{
			FreeHeapBuf();
			PoolReleaseImpl(pCtx_);
			pCtx_ = nullptr;
		}
	}
	void ToSso(StrBenchImpl& _ctx, const char* _s, int _len)
	{
		FreeHeapBuf();
		memcpy(_ctx.sso_, _s, _len + 1);
		_ctx.len_ = _len;
	}
	void ThrowIfInvalidRangeIndex(int _startIdx, int _endIdx) const;
	void ThrowIfNotInitialized() const;
	void ThrowIfInvalidIndex(int _idx) const;
	void AttachSource(char* _pSrc, int _len, int _capacity);
private:
	StrBenchImpl* pCtx_{};
};

template <> void StrForBench::Append<_s32>(const _s32& _val);
template <> void StrForBench::Append<_u32>(const _u32& _val);
template <> void StrForBench::Append<_s32l>(const _s32l& _val);
template <> void StrForBench::Append<_u32l>(const _u32l& _val);
template <> void StrForBench::Append<_s64>(const _s64& _val);
template <> void StrForBench::Append<_u64>(const _u64& _val);
template <> void StrForBench::Append<_s16>(const _s16& _val);
template <> void StrForBench::Append<_u16>(const _u16& _val);
template <> void StrForBench::Append<_s8>(const _s8& _val);
template <> void StrForBench::Append<_u8>(const _u8& _val);
template <> void StrForBench::Append<_f32>(const _f32& _val);
template <> void StrForBench::Append<_f64>(const _f64& _val);
template <> void StrForBench::Append<_f64l>(const _f64l& _val);
template <> void StrForBench::Append<bool>(const bool& _val);

static_assert(sizeof(StrForBench) == 8, "S5~S7 StrForBench size must stay 8 bytes.");

inline StrForBench::StrForBench()
	: pCtx_(nullptr)
{
}

inline StrForBench::StrForBench(const int _capacity)
	: pCtx_(nullptr)
{
	if (_capacity != 0)
	{
		*this = StrForBench(EmptySource, _capacity);
	}
}

inline StrForBench::StrForBench(const char* _pStr, const int _capacity)
	: pCtx_(nullptr)
{
	if (_pStr == nullptr)
	{
		return;
	}

	const int length = StringUtil::Length(_pStr);
	if (length == 0)
	{
		Initialize(_capacity);
		return;
	}

	StrBenchImpl* pImpl = PoolNewImpl();
	if (length <= SSO_CAP)
	{
		ToSso(*pImpl, _pStr, length);
	}
	else
	{
		int expectedCapacity = int(length * EXPANDING_FACTOR);
		if (expectedCapacity < _capacity)
		{
			expectedCapacity = _capacity;
		}
		pImpl->pHeap_ = dbg_new char[expectedCapacity];
		pImpl->cap_ = expectedCapacity;
		pImpl->len_ = length;
		memcpy(pImpl->pHeap_, _pStr, length + 1);
	}
	pCtx_ = pImpl;
}

inline StrForBench::StrForBench(const char* _pStr)
	: StrForBench(_pStr, DEFAULT_BUFFER_SIZE)
{
}

inline StrForBench::StrForBench(char _ch, int _count)
	: pCtx_(nullptr)
{
	StrBenchImpl* pImpl = PoolNewImpl();
	if (_count <= SSO_CAP)
	{
		memset(pImpl->sso_, _ch, _count);
		pImpl->sso_[_count] = 0;
		pImpl->len_ = _count;
	}
	else
	{
		const int cap = _count + DEFAULT_BUFFER_SIZE;
		pImpl->pHeap_ = dbg_new char[cap];
		pImpl->cap_ = cap;
		pImpl->len_ = _count;
		memset(pImpl->pHeap_, _ch, _count);
		pImpl->pHeap_[_count] = 0;
	}
	pCtx_ = pImpl;
}

inline StrForBench::StrForBench(const std::string& _str)
	: StrForBench(_str.c_str())
{
}

inline StrForBench::StrForBench(const StrForBench& _str)
	: pCtx_(nullptr)
{
	if (!_str.pCtx_)
	{
		return;
	}

	StrBenchImpl* pImpl = PoolNewImpl();
	if (_str.pCtx_->pHeap_)
	{
		const int len = _str.Len();
		const int cap = len == 0 ? DEFAULT_BUFFER_SIZE : (len * EXPANDING_FACTOR < DEFAULT_BUFFER_SIZE ? DEFAULT_BUFFER_SIZE : len * EXPANDING_FACTOR);
		pImpl->pHeap_ = dbg_new char[cap];
		pImpl->cap_ = cap;
		pImpl->len_ = len;
		if (len > 0)
		{
			memcpy(pImpl->pHeap_, _str.Data(), len + 1);
		}
		else
		{
			pImpl->pHeap_[0] = 0;
		}
	}
	else
	{
		ToSso(*pImpl, _str.Data(), _str.Len());
	}
	pCtx_ = pImpl;
}

inline StrForBench::StrForBench(StrForBench&& _str) noexcept
	: pCtx_(_str.pCtx_)
{
	_str.pCtx_ = nullptr;
}

inline StrForBench::~StrForBench()
{
	FreeCtx();
}

inline void StrForBench::ExchangeSource(char* _pSrc, int _len)
{
	FreeCtx();
	if (_pSrc == nullptr)
	{
		return;
	}
	StrBenchImpl* pImpl = PoolNewImpl();
	if (_len <= SSO_CAP)
	{
		ToSso(*pImpl, _pSrc, _len);
		JC_DELETE_ARRAY_SAFE(_pSrc);
	}
	else
	{
		pImpl->pHeap_ = _pSrc;
		pImpl->len_ = _len;
		pImpl->cap_ = _len + 1;
	}
	pCtx_ = pImpl;
}

inline void StrForBench::AttachSource(char* _pSrc, int _len, int _capacity)
{
	FreeCtx();
	if (_pSrc == nullptr)
	{
		return;
	}
	StrBenchImpl* pImpl = PoolNewImpl();
	if (_len <= SSO_CAP)
	{
		ToSso(*pImpl, _pSrc, _len);
		JC_DELETE_ARRAY_SAFE(_pSrc);
	}
	else
	{
		pImpl->pHeap_ = _pSrc;
		pImpl->len_ = _len;
		pImpl->cap_ = _capacity;
	}
	pCtx_ = pImpl;
}

template <>
inline void StrForBench::Append<_s32>(const _s32& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%d", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

template <>
inline void StrForBench::Append<_u32>(const _u32& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%u", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

template <>
inline void StrForBench::Append<_s32l>(const _s32l& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%ld", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

template <>
inline void StrForBench::Append<_u32l>(const _u32l& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%lu", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

template <>
inline void StrForBench::Append<_s64>(const _s64& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%lld", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

template <>
inline void StrForBench::Append<_u64>(const _u64& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%llu", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

template <>
inline void StrForBench::Append<_s16>(const _s16& _val)
{
	char buffer[16];
	const int len = snprintf(buffer, sizeof(buffer), "%hd", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

template <>
inline void StrForBench::Append<_u16>(const _u16& _val)
{
	char buffer[16];
	const int len = snprintf(buffer, sizeof(buffer), "%hu", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

template <>
inline void StrForBench::Append<_s8>(const _s8& _val)
{
	char buffer[8];
	const int len = snprintf(buffer, sizeof(buffer), "%d", static_cast<int>(_val));
	if (len > 0)
	{
		Append(buffer, len);
	}
}

template <>
inline void StrForBench::Append<_u8>(const _u8& _val)
{
	char buffer[8];
	const int len = snprintf(buffer, sizeof(buffer), "%u", static_cast<unsigned int>(_val));
	if (len > 0)
	{
		Append(buffer, len);
	}
}

template <>
inline void StrForBench::Append<_f32>(const _f32& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%g", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

template <>
inline void StrForBench::Append<_f64>(const _f64& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%g", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

template <>
inline void StrForBench::Append<_f64l>(const _f64l& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%Lg", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

template <>
inline void StrForBench::Append<bool>(const bool& _val)
{
	int len = _val ? 4 : 5;
	Append(_val ? "true" : "false", len);
}

inline void StrForBench::Append(const char _ch)
{
	StrBenchImpl& ctx = Ctx();
	(void)ctx;
	const int iDstLen = Len() + 1;
	ResizeIfNeeded(iDstLen);

	char* pData = Data();
	pData[iDstLen - 1] = _ch;
	pData[iDstLen] = NULL;
	SetLen(iDstLen);
}

inline void StrForBench::Append(const char* _pStr)
{
	if (_pStr == nullptr)
	{
		throw NullPointerException("추가하고자 하는 문자열이 nullptr 입니다.");
	}

	Ctx();
	const int iStrLen = StringUtil::Length(_pStr);
	const int iDstLen = Len() + iStrLen;
	ResizeIfNeeded(iDstLen);

	char* pData = Data();
	memcpy(pData + Len(), _pStr, iStrLen + 1);
	SetLen(iDstLen);
}

inline void StrForBench::Append(const char* _pStr, int _len)
{
	if (_pStr == nullptr)
	{
		throw NullPointerException("추가하고자 하는 문자열이 nullptr 입니다.");
	}

	Ctx();
	const int iDstLen = Len() + _len;
	ResizeIfNeeded(iDstLen);

	char* pData = Data();
	memcpy(pData + Len(), _pStr, _len);
	pData[iDstLen] = NULL;
	SetLen(iDstLen);
}

inline void StrForBench::Append(char* _pStr)
{
	Append(const_cast<const char*>(_pStr));
}

inline void StrForBench::Append(const std::string& _str)
{
	if (_str.empty())
	{
		return;
	}

	Append(_str.c_str());
}

inline void StrForBench::Append(const StrForBench& _str)
{
	if (_str.Length() == 0)
	{
		return;
	}

	Append(_str.Data());
}

inline void StrForBench::Append(const StringView& _str)
{
	Append(_str.SafeSource(), (int)_str.Length());
}

inline void StrForBench::Append(StrForBench&& _str)
{
	if (_str.Length() == 0)
	{
		return;
	}

	Append(_str.Data());
}

inline void StrForBench::Insert(const int _idx, const char* _pStr)
{
	const int iLen = StringUtil::Length(_pStr);

	if (iLen == 0)
	{
		return;
	}

	StrBenchImpl& ctx = Ctx();
	(void)ctx;
	const int len = Len();
	if (_idx < 0 || _idx > len)
	{
		throw OutOfRangeException("인덱스가 범위를 벗어났습니다.");
	}

	ResizeIfNeeded(len + iLen);

	char* pData = Data();
	if (len - _idx > 0)
	{
		Memory::CopyReverse(
			pData + _idx + iLen,
			Cap() - _idx - iLen,
			pData + _idx,
			len - _idx);
	}

	for (int i = 0; i < iLen; ++i)
	{
		pData[i + _idx] = _pStr[i];
	}

	SetLen(len + iLen);
	pData[len + iLen] = NULL;
}

inline void StrForBench::Insert(const int _idx, const StrForBench& _str)
{
	Insert(_idx, _str.SafeSource());
}

inline void StrForBench::Resize(const int _capacity)
{
	StrBenchImpl& ctx = Ctx();
	if (_capacity - 1 <= SSO_CAP)
	{
		if (ctx.pHeap_ && ctx.len_ <= SSO_CAP)
		{
			const int len = ctx.len_;
			char temp[SSO_CAP + 1];
			memcpy(temp, ctx.pHeap_, len + 1);
			FreeHeapBuf();
			memcpy(ctx.sso_, temp, len + 1);
		}
		return;
	}

	const int len = Len();
	char* pNew = dbg_new char[_capacity];
	if (len > 0)
	{
		memcpy(pNew, Data(), len + 1);
	}
	else
	{
		pNew[0] = 0;
	}
	FreeHeapBuf();
	ctx.pHeap_ = pNew;
	ctx.cap_ = _capacity;
}

inline void StrForBench::ResizeIfNeeded(int _len)
{
	bool bNeedResize = false;
	if (_len >= Cap())
	{
		_len *= EXPANDING_FACTOR;
		bNeedResize = true;
	}

	if (bNeedResize)
		Resize(_len + 1);
}

inline int StrForBench::Compare(const StrForBench& _str) const
{
	return Compare(_str.SafeSource(), _str.Len());
}

inline int StrForBench::Compare(const char* _str, const int _strLen) const
{
	const int len = Len();
	const int iStrLen = _strLen == -1 ? StringUtil::Length(_str) : _strLen;
	const char* pSrc = SafeSource();

	while (*pSrc != NULL && *_str != NULL)
	{
		if (*pSrc > *_str)
			return 1;
		if (*pSrc < *_str)
			return -1;

		pSrc++;
		_str++;
	}

	if (len > iStrLen)
		return 1;
	if (len < iStrLen)
		return -1;

	return 0;
}

inline Vector<int, CDefaultAllocator> StrForBench::FindAll(int _startIdx, int _endIdx, const char* _pStr, bool _caseSensitive /*= true*/) const
{
	Vector<int, CDefaultAllocator> offsets;
	const int iStrLen = StringUtil::Length(_pStr);

	if (iStrLen == 0)
	{
		return offsets;
	}

	ThrowIfInvalidRangeIndex(_startIdx, _endIdx);

	for (int i = _startIdx; i <= _endIdx; )
	{
		int iFind = Find(i, _endIdx, _pStr, _caseSensitive);

		if (iFind == -1)
		{
			break;
		}

		offsets.PushBack(iFind);
		i = iFind + iStrLen;
	}

	return offsets;
}

inline Vector<int, CDefaultAllocator> StrForBench::FindAll(const char* _pStr, bool _caseSensitive /*= true*/) const
{
	return FindAll(0, Len() - 1, _pStr, _caseSensitive);
}

inline Vector<int, CDefaultAllocator> StrForBench::FindAll(const StrForBench& _str, bool _caseSensitive /*= true*/) const
{
	return FindAll(_str.Data(), _caseSensitive);
}

inline int StrForBench::Find(int _startIdx, int _endIdx, const char* _pStr, bool _caseSensitive /*= true*/) const
{
	return StringUtil::Find(Data(), Len(), _startIdx, _endIdx, _pStr, _caseSensitive);
}

inline int StrForBench::Find(int _startIdx, const char* _pStr, bool _caseSensitive /*= true*/) const
{
	return Find(_startIdx, Len() - 1, _pStr, _caseSensitive);
}

inline int StrForBench::Find(const char* _pStr, bool _caseSensitive /*= true*/) const
{
	return Find(0, Len() - 1, _pStr, _caseSensitive);
}

inline int StrForBench::Find(const StrForBench& _str, bool _caseSensitive /*= true*/) const
{
	return Find(_str.Data(), _caseSensitive);
}

inline int StrForBench::Find(int _startIdx, const StrForBench& _str, bool _caseSensitive /*= true*/) const
{
	return Find(_startIdx, Len() - 1, _str.SafeSource(), _caseSensitive);
}

inline int StrForBench::FindReverse(int _startIdx, int _endIdx, const char* _pStr, bool _caseSensitive /*= true*/) const
{
	const int iFindStrLen = StringUtil::Length(_pStr);
	const int iSrcLen = _endIdx - _startIdx + 1;

	if (iFindStrLen == 0)
	{
		return 0;
	}

	ThrowIfInvalidRangeIndex(_startIdx, _endIdx);

	if (iFindStrLen > iSrcLen)
	{
		return -1;
	}

	char* pData = Data();
	for (int i = _endIdx; i >= _startIdx; i--)
	{
		int iContinuous = 0;

		if (_caseSensitive)
		{
			while (iContinuous < iFindStrLen && pData[i + iContinuous] == _pStr[iContinuous])
			{
				iContinuous++;
			}
		}
		else
		{
			while (iContinuous < iFindStrLen && tolower(pData[i + iContinuous]) == tolower(_pStr[iContinuous]))
			{
				iContinuous++;
			}
		}

		if (iContinuous == iFindStrLen)
		{
			return i;
		}
	}

	return -1;
}

inline int StrForBench::FindReverse(const StrForBench& _str, bool _caseSensitive /*= true*/) const
{
	return FindReverse(0, Len() - 1, _str.Data(), _caseSensitive);
}

inline int StrForBench::FindReverse(const char* _pStr, bool _caseSensitive /*= true*/) const
{
	return FindReverse(0, Len() - 1, _pStr, _caseSensitive);
}

inline void StrForBench::Clear()
{
	if (!pCtx_)
		return;
	SetLen(0);
	Data()[0] = NULL;
}

inline void StrForBench::Clear(int _offset, int _length)
{
	if (!pCtx_)
		return;
	const int len = Len();
	if (_offset >= len)
		return;

	const int iRemoveLen = _offset + _length > len ? len - _offset : _length;
	const int iMoveCharCount = len - _offset - iRemoveLen;

	char* pData = Data();
	Memory::CopyUnsafe(
		pData + _offset,
		pData + _offset + iRemoveLen,
		iMoveCharCount);

	SetLen(len - iRemoveLen);
	pData[Len()] = NULL;
}

inline int StrForBench::Count(const char* _pStr, bool _caseSensitive /*= true*/) const
{
	return Count(0, Len() - 1, _pStr, _caseSensitive);
}

inline int StrForBench::Count(const StrForBench& _value, bool _caseSensitive /*= true*/) const
{
	return Count(0, Len() - 1, _value.SafeSource(), _caseSensitive);
}

inline int StrForBench::Count(const int _startIdx, const int _endIdx, const char* _pStr, bool _caseSensitive /*= true*/) const
{
	ThrowIfNotInitialized();
	ThrowIfInvalidRangeIndex(_startIdx, _endIdx);

	const int iStrLen = StringUtil::Length(_pStr);

	int iOffset = _startIdx;
	int iCount = 0;

	while (iOffset <= _endIdx && (iOffset = Find(iOffset, _endIdx, _pStr, _caseSensitive)) != -1)
	{
		iCount++;
		iOffset += iStrLen;
	}
	return iCount;
}

inline int StrForBench::Count(const int _startIdx, const int _endIdx, const StrForBench& _value, bool _caseSensitive /*= true*/) const
{
	return Count(_startIdx, _endIdx, _value.SafeSource(), _caseSensitive);
}

inline int StrForBench::Replace(const char* _pFrom, const StrForBench& _to, bool _caseSensitive /*= true*/)
{
	return Replace(Find(_pFrom, _caseSensitive), StringUtil::Length(_pFrom), _to);
}

inline int StrForBench::Replace(const StrForBench& _from, const StrForBench& _to, bool _caseSensitive /*= true*/)
{
	return Replace(Find(_from.SafeSource(), _caseSensitive), _from.Length(), _to);
}

inline int StrForBench::Replace(int _offset, int _length, const StrForBench& _to)
{
	if (_offset == -1)
		return -1;

	ThrowIfInvalidIndex(_offset);
	const int len = Len();
	const int iLen = _offset + _length > len ? len - _offset : _length;
	const int iToLen = _to.Length();

	if (iLen < iToLen)
	{
		ResizeIfNeeded(len + iToLen - iLen);

		char* pData = Data();
		Memory::CopyUnsafeReverse(
			pData + _offset + iToLen,
			pData + _offset + iLen,
			len - _offset - iLen
		);

		for (int i = 0; i < iToLen; ++i)
		{
			pData[_offset + i] = _to[i];
		}

		SetLen(len + iToLen - iLen);
	}
	else
	{
		char* pData = Data();
		Memory::CopyUnsafe(
			pData + _offset + iToLen,
			pData + _offset + iLen,
			len - _offset - iToLen
		);

		for (int i = 0; i < iToLen; ++i)
		{
			pData[_offset + i] = _to[i];
		}

		SetLen(len - (iLen - iToLen));
	}

	char* pData = Data();
	pData[Len()] = NULL;
	const int iNextOffset = _offset + iToLen;
	return iNextOffset >= Len() ? -1 : iNextOffset;
}

inline int StrForBench::Replace(int _offset, const char* _pFrom, const StrForBench& _to, bool _caseSensitive /*= true*/)
{
	return Replace(Find(_offset, _pFrom, _caseSensitive), StringUtil::Length(_pFrom), _to);
}

inline int StrForBench::Replace(int _offset, const StrForBench& _from, const StrForBench& _to, bool _caseSensitive /*= true*/)
{
	return Replace(Find(_offset, _from, _caseSensitive), _from.Length(), _to);
}

inline bool StrForBench::Contain(const char* _pStr) const
{
	return Find(_pStr) != -1;
}

inline bool StrForBench::Contain(const StrForBench& _str) const
{
	return Find(_str.Data()) != -1;
}

inline void StrForBench::Format(const char* _format, ...)
{
	va_list args;
	va_start(args, _format);

	const int iExpectedLen = vsnprintf(nullptr, 0, _format, args);

	if (iExpectedLen <= 0)
	{
		throw RuntimeException("문자열 포맷 수행중 오류가 발생하였습니다.");
	}

	StrBenchImpl& ctx = Ctx();
	(void)ctx;
	if (Cap() < iExpectedLen + 1)
	{
		Resize(iExpectedLen + DEFAULT_BUFFER_SIZE);
	}

	char* pData = Data();
	vsnprintf(pData, Cap(), _format, args);
	pData[iExpectedLen] = NULL;
	SetLen(iExpectedLen);

	va_end(args);
}

inline void StrForBench::ReplaceAll(const char* _pFrom, const char* _pTo, bool _caseSensitive /*= true*/)
{
	const int iFromLen = StringUtil::Length(_pFrom);

	if (iFromLen == 0)
	{
		*this = _pTo;
		return;
	}

	int iReplaceOffset = 0;
	while ((iReplaceOffset = Replace(iReplaceOffset, _pFrom, _pTo, _caseSensitive)) != -1)
	{
	}
}

inline void StrForBench::SetAt(const int _idx, const char _ch)
{
	ThrowIfInvalidIndex(_idx);
	Data()[_idx] = _ch;
}

inline void StrForBench::SetAtForce(int _idx, char _ch)
{
	if (_idx < 0 || _idx >= Cap())
		return;
	Data()[_idx] = _ch;
}

inline char StrForBench::GetAt(const int _idx) const
{
	return Data()[_idx];
}

inline StrForBench StrForBench::GetRange(const int _startIdx, const int _endIdx) const
{
	auto [pBuffer, iLen, iCapacity] = GetRangeUnsafe(_startIdx, _endIdx);

	StrForBench subString{ 0 };
	subString.AttachSource(pBuffer, iLen, iCapacity);
	return subString;
}

inline StrForBench StrForBench::SubStr(int _startIdx, int _count) const
{
	ThrowIfInvalidIndex(_startIdx);
	if (_count < 0)
	{
		throw InvalidArgumentException("부분 문자열의 길이는 0 이상이어야 합니다.");
	}
	if (_startIdx + _count > Len())
	{
		_count = Len() - _startIdx;
	}
	return GetRange(_startIdx, _startIdx + _count - 1);
}

inline Tuple<char*, int, int> StrForBench::GetRangeUnsafe(const int _startIdx, const int _endIdx) const
{
	return StringUtil::GetRangeUnsafe(Data(), Len(), _startIdx, _endIdx);
}

inline Vector<StrForBench> StrForBench::Split(const char* _delimiter, const bool _includeEmpty) const
{
	Vector<StrForBench> vecTokens;
	int iOffset = Find(_delimiter);

	if (iOffset == -1)
	{
		vecTokens.EmplaceBack(SafeSource());
		return vecTokens;
	}

	const int len = Len();
	const int iDelimiterLen = StringUtil::Length(_delimiter);
	if (iOffset - 1 < 0)
	{
		if (_includeEmpty)
		{
			vecTokens.EmplaceBack(EmptySource);
		}
	}
	else
	{
		vecTokens.EmplaceBack(GetRange(0, iOffset - 1));
	}

	iOffset += iDelimiterLen;

	while (iOffset < len)
	{
		const int iNextOffset = Find(iOffset, len - 1, _delimiter);

		if (iNextOffset == -1)
		{
			break;
		}

		if (iNextOffset <= iOffset)
		{
			if (_includeEmpty)
			{
				vecTokens.EmplaceBack(EmptySource);
			}
		}
		else
		{
			vecTokens.EmplaceBack(GetRange(iOffset, iNextOffset - 1));
		}
		iOffset = iNextOffset + 1;
	}

	if (iOffset < len)
	{
		vecTokens.EmplaceBack(GetRange(iOffset, len - 1));
	}
	else
	{
		if (_includeEmpty)
		{
			vecTokens.EmplaceBack(EmptySource);
		}
	}

	return vecTokens;
}

inline Vector<StrForBench> StrForBench::Split(const char _delimiter, const bool _includeEmpty) const
{
	Vector<StrForBench> vecTokens;
	const int len = Len();
	char* pData = Data();
	int iOffset = 0;

	for (int i = 0; i <= len; i++)
	{
		if (i == len || (pData && pData[i] == _delimiter))
		{
			if (i > iOffset)
			{
				vecTokens.EmplaceBack(GetRange(iOffset, i - 1));
			}
			else if (_includeEmpty)
			{
				vecTokens.EmplaceBack(EmptySource);
			}

			iOffset = i + 1;
		}
	}

	return vecTokens;
}

inline void StrForBench::Initialize(int _capacity)
{
	FreeCtx();
	if (_capacity <= 0)
	{
		return;
	}

	StrBenchImpl* pImpl = PoolNewImpl();
	pImpl->len_ = 0;
	if (_capacity - 1 <= SSO_CAP)
	{
		pImpl->sso_[0] = 0;
	}
	else
	{
		pImpl->pHeap_ = dbg_new char[_capacity];
		pImpl->cap_ = _capacity;
		pImpl->pHeap_[0] = 0;
	}
	pCtx_ = pImpl;
}

inline StrForBench StrForBench::ToLowerCase() const
{
	StrForBench copy = *this;

	for (int i = 0; i < copy.Length(); i++)
	{
		if (IsUpperCaseAlphabat(copy[i]))
		{
			copy[i] += static_cast<char>(32);
		}
	}

	return copy;
}

inline StrForBench StrForBench::ToUpperCase() const
{
	StrForBench copy = *this;

	for (int i = 0; i < copy.Length(); i++)
	{
		if (IsLowerCaseAlphabat(copy[i]))
		{
			copy[i] -= static_cast<char>(32);
		}
	}

	return copy;
}

inline int StrForBench::LeadingZeroCount() const
{
	char* pData = Data();
	const int len = Len();
	int iCount = 0;
	for (int i = 0; i < len; ++i)
	{
		if (pData[i] == '0')
			++iCount;
		else
			break;
	}

	return iCount;
}

inline std::string StrForBench::ToStd()
{
	return Source();
}

inline char& StrForBench::operator[](const int _idx) const
{
	ThrowIfInvalidIndex(_idx);
	return Data()[_idx];
}

inline StrForBench StrForBench::operator+(const StrForBench& _other) const
{
	StrForBench temp = *this;
	temp.Append(_other);
	return temp;
}

inline StrForBench StrForBench::operator+(const char _ch) const
{
	StrForBench temp = *this;
	temp.Append(_ch);
	return temp;
}

inline StrForBench StrForBench::operator+(const char* _pStr) const
{
	StrForBench temp = *this;
	temp.Append(_pStr);
	return temp;
}

inline StrForBench& StrForBench::operator+=(const StrForBench& _other)
{
	Append(_other);
	return *this;
}

inline StrForBench& StrForBench::operator+=(const char _ch)
{
	Append(_ch);
	return *this;
}

inline StrForBench& StrForBench::operator+=(char* _pStr)
{
	Append(_pStr);
	return *this;
}

inline StrForBench& StrForBench::operator+=(const char* _pStr)
{
	Append(_pStr);
	return *this;
}

inline StrForBench& StrForBench::operator=(const StrForBench& _other)
{
	if (!_other.pCtx_)
	{
		FreeCtx();
	}
	else if (_other.pCtx_->pHeap_)
	{
		StrBenchImpl& ctx = Ctx();
		ResizeIfNeeded(_other.Len());
		const int len = _other.Len();
		memcpy(Data(), _other.Data(), len + 1);
		SetLen(len);
		(void)ctx;
	}
	else
	{
		StrBenchImpl& ctx = Ctx();
		ToSso(ctx, _other.Data(), _other.Len());
	}
	return *this;
}

inline StrForBench& StrForBench::operator=(StrForBench&& _other) noexcept
{
	FreeCtx();
	pCtx_ = _other.pCtx_;
	_other.pCtx_ = nullptr;
	return *this;
}

inline StrForBench& StrForBench::operator=(const char* _other)
{
	const int iToLen = StringUtil::Length(_other);
	const int iExpectedCapaity = iToLen + 10;

	if (iExpectedCapaity > Cap())
	{
		FreeCtx();
		StrBenchImpl* pImpl = PoolNewImpl();
		if (iToLen <= SSO_CAP)
		{
			ToSso(*pImpl, _other, iToLen);
		}
		else
		{
			const int cap = iExpectedCapaity + DEFAULT_BUFFER_SIZE;
			pImpl->pHeap_ = dbg_new char[cap];
			pImpl->cap_ = cap;
			pImpl->len_ = 0;
			pImpl->pHeap_[0] = 0;
		}
		pCtx_ = pImpl;
	}
	else
	{
		Ctx();
	}

	memcpy(Data(), _other, iToLen + 1);
	SetLen(iToLen);

	return *this;
}

inline StrForBench& StrForBench::operator=(std::nullptr_t _other)
{
	FreeCtx();
	return *this;
}

inline bool StrForBench::operator==(const StrForBench& _other) const
{
	return Compare(_other) == 0;
}

inline bool StrForBench::operator==(const char* _pOther) const
{
	return Compare(_pOther) == 0;
}

inline bool StrForBench::operator<(const StrForBench& _other) const
{
	return Compare(_other) < 0;
}

inline bool StrForBench::operator<(const char* _pOther) const
{
	return Compare(_pOther) < 0;
}

inline bool StrForBench::operator>(const StrForBench& _other) const
{
	return Compare(_other) > 0;
}

inline bool StrForBench::operator>(const char* _pOther) const
{
	return Compare(_pOther) > 0;
}

inline bool StrForBench::operator<=(const StrForBench& _other) const
{
	return Compare(_other) <= 0;
}

inline bool StrForBench::operator<=(const char* _pOther) const
{
	return Compare(_pOther) <= 0;
}

inline bool StrForBench::operator>=(const StrForBench& _other) const
{
	return Compare(_other) >= 0;
}

inline bool StrForBench::operator>=(const char* _pOther) const
{
	return Compare(_pOther) >= 0;
}

inline std::ostream& operator<<(std::ostream& _os, const StrForBench& _src)
{
	_os << _src.Data();
	return _os;
}

inline void StrForBench::ThrowIfInvalidRangeIndex(const int _startIdx, const int _endIdx) const
{
	if (!IsValidIndexRange(_startIdx, _endIdx))
	{
		throw OutOfRangeException("인덱스 범위를 벗어났습니다.");
	}
}

inline void StrForBench::ThrowIfNotInitialized() const
{
	if (Data() == nullptr)
	{
		throw NullPointerException("String을 먼저 초기화해주세요.");
	}
}

inline void StrForBench::ThrowIfInvalidIndex(const int _idx) const
{
	if (!IsValidIndex(_idx))
	{
		throw OutOfRangeException("인덱스가 범위를 벗어났습니다.");
	}
}

inline _s8 StrForBench::ToInt8(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_s8>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _u8 StrForBench::ToUInt8(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_u8>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _s16 StrForBench::ToInt16(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_s16>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _u16 StrForBench::ToUInt16(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_u16>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _s32 StrForBench::ToInt32(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_s32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _u32 StrForBench::ToUInt32(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_u32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _s64 StrForBench::ToInt64(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_s64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _u64 StrForBench::ToUInt64(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_u64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _f32 StrForBench::ToFloat(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_f32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _f64 StrForBench::ToDouble(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_f64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline bool StrForBench::TryToInt8(OUT _s8& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToUInt8(OUT _u8& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToInt16(OUT _s16& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToUInt16(OUT _u16& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToInt32(OUT _s32& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToUInt32(OUT _u32& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToInt64(OUT _s64& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToUInt64(OUT _u64& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToFloat(OUT _f32& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToDouble(OUT _f64& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline char StrForBench::PopBack()
{
	if (!pCtx_ || Len() == 0)
		return '\0';

	char* pData = Data();
	char popped = pData[Len() - 1];
	pData[Len() - 1] = NULL;
	SetLen(Len() - 1);
	return popped;
}
