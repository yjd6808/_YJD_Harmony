/*
 * StrForBench_S3.inl: S3 StringImpl* + TLS free-list 후보 (8B 핸들).
 * 헤더 가드 없음. StrForBench.h에서 네임스페이스 s3으로 include한다.
 * S2와 동일하되 Impl 슬롯만 TLS pool에서 받고, 문자열 버퍼는 일반 allocator를 쓴다.
 * 같은 스레드에서 생성/소멸한다는 가정 하에 owner 검사가 없다.
 */

namespace jc_gbench { namespace STRFORBENCH_NS {

struct StrBenchImpl
{
	char* pBuf_{};
	int len_{};
	int cap_{};
};

inline thread_local benchpool::TlsSlotPool<StrBenchImpl> t_implPool{};

inline StrBenchImpl* PoolNewImpl()
{
	StrBenchImpl* p = t_implPool.AcquireRaw();
	return new (p) StrBenchImpl{};
}

inline void PoolDeleteImpl(StrBenchImpl* _p)
{
	if (!_p)
		return;
	_p->~StrBenchImpl();
	t_implPool.ReleaseRaw(_p);
}

class StrForBench final
{
	inline static constexpr int DEFAULT_BUFFER_SIZE = 32;
	inline static constexpr int EXPANDING_FACTOR = 2;

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
	char* Data() const { return pCtx_ ? pCtx_->pBuf_ : nullptr; }
	int Len() const { return pCtx_ ? pCtx_->len_ : 0; }
	int Cap() const { return pCtx_ ? pCtx_->cap_ : 0; }
	StrBenchImpl& Ctx() { if (!pCtx_) pCtx_ = PoolNewImpl(); return *pCtx_; }
	void FreeCtx()
	{
		if (pCtx_)
		{
			JC_DELETE_ARRAY_SAFE(pCtx_->pBuf_);
			PoolDeleteImpl(pCtx_);
			pCtx_ = nullptr;
		}
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

static_assert(sizeof(StrForBench) == 8, "S3 StrForBench size must stay 8 bytes.");

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

	const int length = StringUtilT::Length(_pStr);
	int expectedCapacity = int(length * EXPANDING_FACTOR);

	if (expectedCapacity < _capacity)
	{
		expectedCapacity = _capacity;
	}

	if (length == 0)
	{
		Initialize(_capacity);
		return;
	}

	StrBenchImpl* pImpl = PoolNewImpl();
	pImpl->pBuf_ = dbg_new char[expectedCapacity];
	pImpl->cap_ = expectedCapacity;
	pImpl->len_ = length;
	StringUtilT::Copy(pImpl->pBuf_, pImpl->cap_, _pStr);
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
	pImpl->pBuf_ = dbg_new char[_count + DEFAULT_BUFFER_SIZE];
	pImpl->cap_ = _count + DEFAULT_BUFFER_SIZE;
	pImpl->len_ = _count;

	for (int i = 0; i < _count; ++i)
	{
		pImpl->pBuf_[i] = _ch;
	}
	pImpl->pBuf_[_count] = 0;
	pCtx_ = pImpl;
}

inline StrForBench::StrForBench(const std::string& _str)
	: StrForBench(_str.c_str())
{
}

inline StrForBench::StrForBench(const StrForBench& _str)
	: pCtx_(nullptr)
{
	if (_str.pCtx_ && _str.pCtx_->pBuf_)
	{
		*this = StrForBench(_str.pCtx_->pBuf_);
	}
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
	pImpl->pBuf_ = _pSrc;
	pImpl->len_ = _len;
	pImpl->cap_ = _len + 1;
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
	pImpl->pBuf_ = _pSrc;
	pImpl->len_ = _len;
	pImpl->cap_ = _capacity;
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
	Ctx();
	const int iDstLen = Len() + 1;
	ResizeIfNeeded(iDstLen);

	char* pBuf = Data();
	Memory::CopyUnsafe(pBuf + Len(), &_ch, 1);
	pBuf[iDstLen] = NULL;
	pCtx_->len_ = iDstLen;
}

inline void StrForBench::Append(const char* _pStr)
{
	if (_pStr == nullptr)
	{
		throw NullPointerException("추가하고자 하는 문자열이 nullptr 입니다.");
	}

	Ctx();
	const int iStrLen = StringUtilT::Length(_pStr);
	const int iDstLen = Len() + iStrLen;
	ResizeIfNeeded(iDstLen);

	char* pBuf = Data();
	Memory::CopyUnsafe(pBuf + Len(), _pStr, iStrLen);
	pBuf[iDstLen] = NULL;
	pCtx_->len_ = iDstLen;
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

	char* pBuf = Data();
	Memory::CopyUnsafe(pBuf + Len(), _pStr, _len);
	pBuf[iDstLen] = NULL;
	pCtx_->len_ = iDstLen;
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
	const int iLen = StringUtilT::Length(_pStr);

	if (iLen == 0)
	{
		return;
	}

	Ctx();
	const int len = Len();
	if (_idx < 0 || _idx > len)
	{
		throw OutOfRangeException("인덱스가 범위를 벗어났습니다.");
	}

	ResizeIfNeeded(len + iLen);

	char* pBuf = Data();
	if (len - _idx > 0)
	{
		Memory::CopyReverse(
			pBuf + _idx + iLen,
			Cap() - _idx - iLen,
			pBuf + _idx,
			len - _idx);
	}

	for (int i = 0; i < iLen; ++i)
	{
		pBuf[i + _idx] = _pStr[i];
	}

	pCtx_->len_ = len + iLen;
	pBuf[len + iLen] = NULL;
}

inline void StrForBench::Insert(const int _idx, const StrForBench& _str)
{
	Insert(_idx, _str.SafeSource());
}

inline void StrForBench::Resize(const int _capacity)
{
	StrBenchImpl& ctx = Ctx();
	char* pTempBuffer = ctx.pBuf_;

	ctx.pBuf_ = dbg_new char[_capacity];
	ctx.cap_ = _capacity;

	StringUtilT::Copy(ctx.pBuf_, ctx.cap_, pTempBuffer);
	JC_DELETE_ARRAY_SAFE(pTempBuffer);
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
	const int iStrLen = _strLen == -1 ? StringUtilT::Length(_str) : _strLen;
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
	const int iStrLen = StringUtilT::Length(_pStr);

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
	return StringUtilT::Find(Data(), Len(), _startIdx, _endIdx, _pStr, _caseSensitive);
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
	const int iFindStrLen = StringUtilT::Length(_pStr);
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

	char* pBuf = Data();
	for (int i = _endIdx; i >= _startIdx; i--)
	{
		int iContinuous = 0;

		if (_caseSensitive)
		{
			while (iContinuous < iFindStrLen && pBuf[i + iContinuous] == _pStr[iContinuous])
			{
				iContinuous++;
			}
		}
		else
		{
			while (iContinuous < iFindStrLen && tolower(pBuf[i + iContinuous]) == tolower(_pStr[iContinuous]))
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
	if (pCtx_ && pCtx_->pBuf_)
	{
		pCtx_->len_ = 0;
		pCtx_->pBuf_[0] = NULL;
	}
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

	char* pBuf = Data();
	Memory::CopyUnsafe(
		pBuf + _offset,
		pBuf + _offset + iRemoveLen,
		iMoveCharCount);

	pCtx_->len_ = len - iRemoveLen;
	pBuf[Len()] = NULL;
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

	const int iStrLen = StringUtilT::Length(_pStr);

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
	return Replace(Find(_pFrom, _caseSensitive), StringUtilT::Length(_pFrom), _to);
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

		char* pBuf = Data();
		Memory::CopyUnsafeReverse(
			pBuf + _offset + iToLen,
			pBuf + _offset + iLen,
			len - _offset - iLen
		);

		for (int i = 0; i < iToLen; ++i)
		{
			pBuf[_offset + i] = _to[i];
		}

		pCtx_->len_ = len + iToLen - iLen;
	}
	else
	{
		char* pBuf = Data();
		Memory::CopyUnsafe(
			pBuf + _offset + iToLen,
			pBuf + _offset + iLen,
			len - _offset - iToLen
		);

		for (int i = 0; i < iToLen; ++i)
		{
			pBuf[_offset + i] = _to[i];
		}

		pCtx_->len_ = len - (iLen - iToLen);
	}

	char* pBuf = Data();
	pBuf[Len()] = NULL;
	const int iNextOffset = _offset + iToLen;
	return iNextOffset >= Len() ? -1 : iNextOffset;
}

inline int StrForBench::Replace(int _offset, const char* _pFrom, const StrForBench& _to, bool _caseSensitive /*= true*/)
{
	return Replace(Find(_offset, _pFrom, _caseSensitive), StringUtilT::Length(_pFrom), _to);
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
	if (ctx.cap_ < iExpectedLen + 1)
	{
		Resize(iExpectedLen + DEFAULT_BUFFER_SIZE);
	}

	char* pBuf = Data();
	vsnprintf(pBuf, Cap(), _format, args);
	pBuf[iExpectedLen] = NULL;
	ctx.len_ = iExpectedLen;

	va_end(args);
}

inline void StrForBench::ReplaceAll(const char* _pFrom, const char* _pTo, bool _caseSensitive /*= true*/)
{
	const int iFromLen = StringUtilT::Length(_pFrom);

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
	return StringUtilT::GetRangeUnsafe(Data(), Len(), _startIdx, _endIdx);
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
	const int iDelimiterLen = StringUtilT::Length(_delimiter);
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
	char* pBuf = Data();
	int iOffset = 0;

	for (int i = 0; i <= len; i++)
	{
		if (i == len || (pBuf && pBuf[i] == _delimiter))
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
	pImpl->pBuf_ = dbg_new char[_capacity];
	pImpl->cap_ = _capacity;
	pImpl->len_ = 0;
	pImpl->pBuf_[0] = 0;
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
	char* pBuf = Data();
	const int len = Len();
	int iCount = 0;
	for (int i = 0; i < len; ++i)
	{
		if (pBuf[i] == '0')
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
	if (!_other.pCtx_ || !_other.pCtx_->pBuf_)
	{
		FreeCtx();
	}
	else
	{
		StrBenchImpl& ctx = Ctx();
		ResizeIfNeeded(_other.Len());
		ctx.len_ = _other.Len();
		Memory::CopyUnsafe(ctx.pBuf_, _other.pCtx_->pBuf_, ctx.len_);
		ctx.pBuf_[ctx.len_] = NULL;
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
	const int iToLen = StringUtilT::Length(_other);
	const int iExpectedCapaity = iToLen + 10;

	if (iExpectedCapaity > Cap())
	{
		Initialize(iExpectedCapaity + DEFAULT_BUFFER_SIZE);
	}

	StringUtilT::Copy(Data(), Cap(), _other);
	pCtx_->len_ = iToLen;

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
	return StringUtilT::ToNumber<_s8>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _u8 StrForBench::ToUInt8(bool _ignoreLeadingZero) const
{
	return StringUtilT::ToNumber<_u8>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _s16 StrForBench::ToInt16(bool _ignoreLeadingZero) const
{
	return StringUtilT::ToNumber<_s16>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _u16 StrForBench::ToUInt16(bool _ignoreLeadingZero) const
{
	return StringUtilT::ToNumber<_u16>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _s32 StrForBench::ToInt32(bool _ignoreLeadingZero) const
{
	return StringUtilT::ToNumber<_s32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _u32 StrForBench::ToUInt32(bool _ignoreLeadingZero) const
{
	return StringUtilT::ToNumber<_u32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _s64 StrForBench::ToInt64(bool _ignoreLeadingZero) const
{
	return StringUtilT::ToNumber<_s64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _u64 StrForBench::ToUInt64(bool _ignoreLeadingZero) const
{
	return StringUtilT::ToNumber<_u64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _f32 StrForBench::ToFloat(bool _ignoreLeadingZero) const
{
	return StringUtilT::ToNumber<_f32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline _f64 StrForBench::ToDouble(bool _ignoreLeadingZero) const
{
	return StringUtilT::ToNumber<_f64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

inline bool StrForBench::TryToInt8(OUT _s8& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtilT::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToUInt8(OUT _u8& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtilT::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToInt16(OUT _s16& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtilT::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToUInt16(OUT _u16& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtilT::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToInt32(OUT _s32& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtilT::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToUInt32(OUT _u32& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtilT::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToInt64(OUT _s64& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtilT::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToUInt64(OUT _u64& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtilT::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToFloat(OUT _f32& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtilT::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline bool StrForBench::TryToDouble(OUT _f64& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtilT::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

inline char StrForBench::PopBack()
{
	if (!pCtx_ || Len() == 0)
		return '\0';

	char* pBuf = Data();
	char popped = pBuf[Len() - 1];
	pBuf[--pCtx_->len_] = NULL;
	return popped;
}

}} // namespace jc_gbench::STRFORBENCH_NS
