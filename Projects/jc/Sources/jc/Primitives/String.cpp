/*
	작성자 : 윤정도
	operator 함수 학습을 위해 제작해본 문자열 클래스입니다.
*/


#include <jc/Core.h>
#include <jc/Primitives/String.h>
#include <jc/Primitives/StringUtil.h>
#include <jc/Memory.h>
#include <jc/Exception.h>
#include <jc/Ascii.h>
#include <jc/Tuple.h>
#include <jc/Container/Vector.h>

NS_JC_BEGIN

const String String::Empty;
const String String::Null(0);

//////////////////////////////////////////////////////////////////////////////////////////
String::String()
{
	Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
String::String(const int _capacity)
{
	if (_capacity == 0)
	{
		pBuffer_ = nullptr;
		capacity_ = 0;
		len_ = 0;
	}
	else
	{
		*this = String(EmptySource, _capacity);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
String::String(const char* _pStr, const int _capacity)
{
	if (_pStr == nullptr)
	{
		pBuffer_ = nullptr;
		capacity_ = 0;
		len_ = 0;
		return;
	}

	const int length = StringUtil::Length(_pStr);
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

	pBuffer_ = dbg_new char[expectedCapacity];
	capacity_ = expectedCapacity;
	len_ = length;

	StringUtil::Copy(pBuffer_, capacity_, _pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
String::String(const char* _pStr)
: String(_pStr, DEFAULT_BUFFER_SIZE)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
String::String(char _ch, int _count)
{
	pBuffer_ = dbg_new char[_count + DEFAULT_BUFFER_SIZE];
	capacity_ = _count + DEFAULT_BUFFER_SIZE;
	len_ = _count;

	for (int i = 0; i < _count; ++i)
	{
		pBuffer_[i] = _ch;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
String::String(const std::string& _str)
	: String(_str.c_str())
{
}

//////////////////////////////////////////////////////////////////////////////////////////
String::String(const String& _str)
	: String(_str.pBuffer_)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
String::String(String&& _str) noexcept
{
	this->operator=(Move(_str));
}

//////////////////////////////////////////////////////////////////////////////////////////
String::~String()
{
	JC_DELETE_ARRAY_SAFE(pBuffer_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::ExchangeSource(char* _pSrc, int _len)
{
	JC_DELETE_ARRAY_SAFE(pBuffer_);
	pBuffer_ = _pSrc;
	len_ = _len;
	capacity_ = _len + 1;
}


//////////////////////////////////////////////////////////////////////////////////////////
// 제네릭 Append 템플릿 구현 (기본값)
template <typename T>
void String::Append(const T& _str)
{
	// std::to_string을 사용하여 변환
	std::string s = std::to_string(_str);
	Append(s.c_str(), (int)s.length());
}

//////////////////////////////////////////////////////////////////////////////////////////
// _s32 특수화 (최적화)
template <>
void String::Append<_s32>(const _s32& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%d", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// _u32 특수화 (최적화)
template <>
void String::Append<_u32>(const _u32& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%u", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// _s32l 특수화 (최적화)
template <>
void String::Append<_s32l>(const _s32l& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%ld", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// _u32l 특수화 (최적화)
template <>
void String::Append<_u32l>(const _u32l& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%lu", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// _s64 특수화 (최적화)
template <>
void String::Append<_s64>(const _s64& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%lld", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// _u64 특수화 (최적화)
template <>
void String::Append<_u64>(const _u64& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%llu", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// _s16 특수화 (최적화)
template <>
void String::Append<_s16>(const _s16& _val)
{
	char buffer[16];
	const int len = snprintf(buffer, sizeof(buffer), "%hd", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// _u16 특수화 (최적화)
template <>
void String::Append<_u16>(const _u16& _val)
{
	char buffer[16];
	const int len = snprintf(buffer, sizeof(buffer), "%hu", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// _s8 특수화 (최적화)
template <>
void String::Append<_s8>(const _s8& _val)
{
	char buffer[8];
	const int len = snprintf(buffer, sizeof(buffer), "%d", static_cast<int>(_val));
	if (len > 0)
	{
		Append(buffer, len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// _u8 특수화 (최적화)
template <>
void String::Append<_u8>(const _u8& _val)
{
	char buffer[8];
	const int len = snprintf(buffer, sizeof(buffer), "%u", static_cast<unsigned int>(_val));
	if (len > 0)
	{
		Append(buffer, len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// _f32 특수화 (최적화)
template <>
void String::Append<_f32>(const _f32& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%g", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// _f64 특수화 (최적화)
template <>
void String::Append<_f64>(const _f64& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%g", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// _f64l 특수화 (최적화)
template <>
void String::Append<_f64l>(const _f64l& _val)
{
	char buffer[32];
	const int len = snprintf(buffer, sizeof(buffer), "%Lg", _val);
	if (len > 0)
	{
		Append(buffer, len);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// bool 특수화 (최적화)
template <>
void String::Append<bool>(const bool& _val)
{
	int len = _val ? 4 : 5; // "true"는 4글자, "false"는 5글자
	Append(_val ? "true" : "false", len);
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::Append(const char _ch)
{
	const int iDstLen = len_ + 1;
	ResizeIfNeeded(iDstLen);

	Memory::CopyUnsafe(pBuffer_ + len_, &_ch, 1);
	pBuffer_[iDstLen] = NULL;
	len_ = iDstLen;
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::Append(const char* _pStr)
{
	if (_pStr == nullptr)
	{
		throw NullPointerException("추가하고자 하는 문자열이 nullptr 입니다.");
	}

	const int iStrLen = StringUtil::Length(_pStr);
	const int iDstLen = len_ + iStrLen;
	ResizeIfNeeded(iDstLen);

	Memory::CopyUnsafe(pBuffer_ + len_, _pStr, iStrLen);
	pBuffer_[iDstLen] = NULL;
	len_ = iDstLen;
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::Append(const char* _pStr, int _len)
{
	if (_pStr == nullptr)
	{
		throw NullPointerException("추가하고자 하는 문자열이 nullptr 입니다.");
	}

	const int iDstLen = len_ + _len;
	ResizeIfNeeded(iDstLen);

	Memory::CopyUnsafe(pBuffer_ + len_, _pStr, _len);
	pBuffer_[iDstLen] = NULL;
	len_ = iDstLen;
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::Append(char* _pStr)
{
	Append(const_cast<const char*>(_pStr));
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::Append(const std::string& _str)
{
	if (_str.empty())
	{
		return;
	}

	Append(_str.c_str());
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::Append(const String& _str)
{
	if (_str.Length() == 0)
	{
		return;
	}

	Append(_str.pBuffer_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::Append(String&& _str)
{
	if (_str.Length() == 0)
	{
		return;
	}

	Append(_str.pBuffer_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::Insert(const int _idx, const char* _pStr)
{
	const int iLen = StringUtil::Length(_pStr);

	// 빈 문자열 삽입은 무시
	if (iLen == 0)
	{
		return;
	}

	// 인덱스 범위 검사: [0, len_] 범위 허용 (len_은 끝 위치)
	if (_idx < 0 || _idx > len_)
	{
		throw OutOfRangeException("인덱스가 범위를 벗어났습니다.");
	}

	ResizeIfNeeded(len_ + iLen);

	if (len_ - _idx > 0)
	{
		Memory::CopyReverse(
			pBuffer_ + _idx + iLen,
			capacity_ - _idx - iLen,
			pBuffer_ + _idx,
			len_ - _idx);
	}

	for (int i = 0; i < iLen; ++i)
	{
		pBuffer_[i + _idx] = _pStr[i];
	}

	len_ += iLen;
	pBuffer_[len_] = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::Insert(const int _idx, const String& _str)
{
	Insert(_idx, _str.SafeSource());
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::Resize(const int _capacity)
{
	char* pTempBuffer = pBuffer_;

	pBuffer_ = dbg_new char[_capacity];
	capacity_ = _capacity;

	StringUtil::Copy(pBuffer_, capacity_, pTempBuffer);
	JC_DELETE_ARRAY_SAFE(pTempBuffer);
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::ResizeIfNeeded(int _len)
{
	bool bNeedResize = false;
	if (_len >= capacity_)
	{
		_len *= EXPANDING_FACTOR;
		bNeedResize = true;
	}

	if (bNeedResize)
		Resize(_len + 1);
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Compare(const String& _str) const
{
	return Compare(_str.SafeSource(), _str.len_);
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Compare(const char* _str, const int _strLen) const
{
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

	if (len_ > iStrLen)
		return 1;
	if (len_ < iStrLen)
		return -1;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<int, CDefaultAllocator> String::FindAll(int _startIdx, int _endIdx, const char* _pStr) const
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
		int iFind = Find(i, _endIdx, _pStr);

		if (iFind == -1)
		{
			break;
		}

		offsets.PushBack(iFind);
		i += iFind + iStrLen;
	}

	return offsets;
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<int, CDefaultAllocator> String::FindAll(const char* _pStr) const
{
	return FindAll(0, len_ - 1, _pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<int, CDefaultAllocator> String::FindAll(const String& _str) const
{
	return FindAll(_str.pBuffer_);
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Find(int _startIdx, int _endIdx, const char* _pStr) const
{
	return StringUtil::Find(pBuffer_, len_, _startIdx, _endIdx, _pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Find(int _startIdx, const char* _pStr) const
{
	return Find(_startIdx, len_ - 1, _pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Find(const char* _pStr) const
{
	return Find(0, len_ - 1, _pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Find(const String& _str) const
{
	return Find(_str.pBuffer_);
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Find(int _startIdx, const String& _str) const
{
	return Find(_startIdx, len_ - 1, _str.SafeSource());
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::FindReverse(int _startIdx, int _endIdx, const char* _pStr) const
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

	for (int i = _endIdx; i >= _startIdx; i--)
	{
		int iContinuous = 0;

		while (iContinuous < iFindStrLen && pBuffer_[i + iContinuous] == _pStr[iContinuous])
		{
			iContinuous++;
		}

		if (iContinuous == iFindStrLen)
		{
			return i;
		}
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::FindReverse(const String& _str) const
{
	return FindReverse(0, len_ - 1, _str.pBuffer_);
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::FindReverse(const char* _pStr) const
{
	return FindReverse(0, len_ - 1, _pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::Clear()
{
	if (pBuffer_)
	{
		len_ = 0;
		pBuffer_[len_] = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::Clear(int _offset, int _length)
{
	if (_offset >= len_)
		return;

	const int iRemoveLen = _offset + _length > len_ ? len_ - _offset : _length;
	const int iMoveCharCount = len_ - _offset - iRemoveLen;

	Memory::CopyUnsafe(
		pBuffer_ + _offset,
		pBuffer_ + _offset + iRemoveLen,
		iMoveCharCount);

	len_ -= iRemoveLen;
	pBuffer_[len_] = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Count(const char* _pStr) const
{
	return Count(0, len_ - 1, _pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Count(const String& _value) const
{
	return Count(0, len_ - 1, _value.SafeSource());
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Count(const int _startIdx, const int _endIdx, const char* _pStr) const
{
	ThrowIfNotInitialized();
	ThrowIfInvalidRangeIndex(_startIdx, _endIdx);

	const int iStrLen = StringUtil::Length(_pStr);

	int iOffset = 0;
	int iCount = 0;

	while (iOffset <= _endIdx && (iOffset = Find(iOffset, _endIdx, _pStr)) != -1)
	{
		iCount++;
		iOffset += iStrLen;
	}
	return iCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Count(const int _startIdx, const int _endIdx, const String& _value) const
{
	return Count(_startIdx, _endIdx, _value.SafeSource());
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Replace(const char* _pFrom, const String& _to)
{
	return Replace(Find(_pFrom), StringUtil::Length(_pFrom), _to);
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Replace(const String& _from, const String& _to)
{
	return Replace(Find(_from.SafeSource()), _from.Length(), _to);
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Replace(int _offset, int _length, const String& _to)
{
	if (_offset == -1)
		return -1;

	ThrowIfInvalidIndex(_offset);
	const int iLen = _offset + _length > len_ ? len_ - _offset : _length;

	if (iLen < _to.Length())
	{
		ResizeIfNeeded(len_ + _to.Length() - iLen);

		Memory::CopyUnsafeReverse(
			pBuffer_ + _offset + _to.Length(),
			pBuffer_ + _offset + iLen,
			len_ - _offset - iLen
		);

		for (int i = 0; i < _to.Length(); ++i)
		{
			pBuffer_[_offset + i] = _to[i];
		}

		len_ += _to.Length() - iLen;
	}
	else
	{
		Memory::CopyUnsafe(
			pBuffer_ + _offset + _to.Length(),
			pBuffer_ + _offset + iLen,
			len_ - _offset - _to.Length()
		);

		for (int i = 0; i < _to.Length(); ++i)
		{
			pBuffer_[_offset + i] = _to[i];
		}

		len_ -= iLen - _to.Length();
	}

	pBuffer_[len_] = NULL;
	const int iNextOffset = _offset + _to.Length();
	return iNextOffset >= len_ ? -1 : iNextOffset;
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Replace(int _offset, const char* _pFrom, const String& _to)
{
	return Replace(Find(_offset, _pFrom), StringUtil::Length(_pFrom), _to);
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::Replace(int _offset, const String& _from, const String& _to)
{
	return Replace(Find(_offset, _from), _from.Length(), _to);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::Contain(const char* _pStr) const
{
	return Find(_pStr) != -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::Contain(const String& _str) const
{
	return Find(_str.pBuffer_) != -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::Format(const char* _format, ...)
{
	va_list args;
	va_start(args, _format);

	const int iExpectedLen = vsnprintf(nullptr, 0, _format, args);

	if (iExpectedLen <= 0)
	{
		throw RuntimeException("문자열 포맷 수행중 오류가 발생하였습니다.");
	}

	if (capacity_ < iExpectedLen + 1)
	{
		Resize(iExpectedLen + DEFAULT_BUFFER_SIZE);
	}

	vsnprintf(pBuffer_, capacity_, _format, args);
	pBuffer_[iExpectedLen] = NULL;
	len_ = iExpectedLen;
	
	va_end(args);
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::ReplaceAll(const char* _pFrom, const char* _pTo)
{
	const int iFromLen = StringUtil::Length(_pFrom);
	
	if (iFromLen == 0)
	{
		*this = _pTo;
		return;
	}

	int iReplaceOffset = 0;
	while ((iReplaceOffset = Replace(iReplaceOffset, _pFrom, _pTo)) != -1)
	{
		// EMPTY
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::SetAt(const int _idx, const char _ch)
{
	ThrowIfInvalidIndex(_idx);
	pBuffer_[_idx] = _ch;
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::SetAtForce(int _idx, char _ch)
{
	if (_idx < 0 || _idx >= capacity_)
		return;
	pBuffer_[_idx] = _ch;
}

//////////////////////////////////////////////////////////////////////////////////////////
char String::GetAt(const int _idx) const
{
	return pBuffer_[_idx];
}

//////////////////////////////////////////////////////////////////////////////////////////
String String::GetRange(const int _startIdx, const int _endIdx) const
{
	return StringUtil::GetRange(pBuffer_, len_, _startIdx, _endIdx);
}

//////////////////////////////////////////////////////////////////////////////////////////
String String::SubStr(int _startIdx, int _count) const
{
	ThrowIfInvalidIndex(_startIdx);
	if (_count < 0)
	{
		throw InvalidArgumentException("부분 문자열의 길이는 0 이상이어야 합니다.");
	}
	if (_startIdx + _count > len_)
	{
		_count = len_ - _startIdx;
	}
	return GetRange(_startIdx, _startIdx + _count - 1);
}

//////////////////////////////////////////////////////////////////////////////////////////
Tuple<char*, int, int> String::GetRangeUnsafe(const int _startIdx, const int _endIdx) const
{
	return StringUtil::GetRangeUnsafe(pBuffer_, len_, _startIdx, _endIdx);
}

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String> String::Split(const char* _delimiter, const bool _includeEmpty) const
{
	Vector<String> vecTokens;
	int iOffset = Find(_delimiter);

	if (iOffset == -1)
	{
		vecTokens.EmplaceBack(pBuffer_);
		return vecTokens;
	}

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

	while (iOffset < len_)
	{
		const int iNextOffset = Find(iOffset, len_ - 1, _delimiter);

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

	if (iOffset < len_)
	{
		vecTokens.EmplaceBack(GetRange(iOffset, len_ - 1));
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

//////////////////////////////////////////////////////////////////////////////////////////
Vector<String> String::Split(const char _delimiter, const bool _includeEmpty) const
{
	Vector<String> vecTokens;
	int iOffset = 0;

	for (int i = 0; i <= len_; i++)
	{
		if (i == len_ || pBuffer_[i] == _delimiter)
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

//////////////////////////////////////////////////////////////////////////////////////////
void String::Initialize(int _capacity)
{
	JC_DELETE_ARRAY_SAFE(pBuffer_);

	if (_capacity <= 0)
	{
		pBuffer_ = nullptr;
		len_ = 0;
		capacity_ = 0;
	}
	else
	{
		pBuffer_ = dbg_new char[_capacity];
		len_ = 0;
		capacity_ = _capacity;
		pBuffer_[0] = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
String String::ToLowerCase() const
{
	String copy = *this;

	for (int i = 0; i < copy.Length(); i++)
	{
		if (IsUpperCaseAlphabat(copy[i]))
		{
			copy[i] += static_cast<char>(32);
		}
	}

	return copy;
}

//////////////////////////////////////////////////////////////////////////////////////////
String String::ToUpperCase() const
{
	String copy = *this;

	for (int i = 0; i < copy.Length(); i++)
	{
		if (IsLowerCaseAlphabat(copy[i]))
		{
			copy[i] -= static_cast<char>(32);
		}
	}

	return copy;
}

//////////////////////////////////////////////////////////////////////////////////////////
int String::LeadingZeroCount() const
{
	int iCount = 0;
	for (int i = 0; i < len_; ++i)
	{
		if (pBuffer_[i] == '0')
			++iCount;
		else
			break;
	}

	return iCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::string String::ToStd()
{
	return Source();
}

//////////////////////////////////////////////////////////////////////////////////////////
char& String::operator[](const int _idx) const
{
	ThrowIfInvalidIndex(_idx);
	return pBuffer_[_idx];
}

//////////////////////////////////////////////////////////////////////////////////////////
String String::operator+(const String& _other) const
{
	String temp = *this;
	temp.Append(_other);
	return temp;
}

//////////////////////////////////////////////////////////////////////////////////////////
String String::operator+(const char _ch) const
{
	String temp = *this;
	temp.Append(_ch);
	return temp;
}

//////////////////////////////////////////////////////////////////////////////////////////
String String::operator+(const char* _pStr) const
{
	String temp = *this;
	temp.Append(_pStr);
	return temp;
}

//////////////////////////////////////////////////////////////////////////////////////////
String& String::operator+=(const String& _other)
{
	Append(_other);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
String& String::operator+=(const char _ch)
{
	Append(_ch);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
String& String::operator+=(char* _pStr)
{
	Append(_pStr);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
String& String::operator+=(const char* _pStr)
{
	Append(_pStr);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
String& String::operator=(const String& _other)
{
	if (_other.pBuffer_ == nullptr)
	{
		JC_DELETE_ARRAY_SAFE(pBuffer_);
		len_ = 0;
		capacity_ = 0;
	}
	else
	{
		ResizeIfNeeded(_other.len_);
		len_ = _other.len_;
		Memory::CopyUnsafe(pBuffer_, _other.pBuffer_, len_);
		pBuffer_[len_] = NULL;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
String& String::operator=(String&& _other) noexcept
{
	JC_DELETE_ARRAY_SAFE(pBuffer_);

	capacity_ = _other.capacity_;
	len_ = _other.len_;
	pBuffer_ = _other.pBuffer_;

	_other.pBuffer_ = nullptr;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
String& String::operator=(const char* _other)
{
	const int iToLen = StringUtil::Length(_other);
	const int iExpectedCapaity = iToLen + 10;

	if (iExpectedCapaity > capacity_)
	{
		Initialize(iExpectedCapaity + DEFAULT_BUFFER_SIZE);
	}

	StringUtil::Copy(pBuffer_, capacity_, _other);
	len_ = iToLen;

	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
String& String::operator=(std::nullptr_t _other)
{
	JC_DELETE_ARRAY_SAFE(pBuffer_);

	capacity_ = 0;
	len_ = 0;

	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::operator==(const String& _other) const
{
	return Compare(_other) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::operator==(const char* _pOther) const
{
	return Compare(_pOther) == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::operator<(const String& _other) const
{
	return Compare(_other) < 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::operator<(const char* _pOther) const
{
	return Compare(_pOther) < 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::operator>(const String& _other) const
{
	return Compare(_other) > 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::operator>(const char* _pOther) const
{
	return Compare(_pOther) > 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::operator<=(const String& _other) const
{
	return Compare(_other) <= 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::operator<=(const char* _pOther) const
{
	return Compare(_pOther) <= 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::operator>=(const String& _other) const
{
	return Compare(_other) >= 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::operator>=(const char* _pOther) const
{
	return Compare(_pOther) >= 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& _os, const String& _src)
{
	_os << _src.pBuffer_;
	return _os;
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::ThrowIfInvalidRangeIndex(const int _startIdx, const int _endIdx) const
{
	if (!IsValidIndexRange(_startIdx, _endIdx))
	{
		throw OutOfRangeException("인덱스 범위를 벗어났습니다.");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::ThrowIfNotInitialized() const
{
	if (pBuffer_ == nullptr)
	{
		throw NullPointerException("String을 먼저 초기화해주세요.");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void String::ThrowIfInvalidIndex(const int _idx) const
{
	if (!IsValidIndex(_idx))
	{
		throw OutOfRangeException("인덱스가 범위를 벗어났습니다.");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
_s8 String::ToInt8(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_s8>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u8 String::ToUInt8(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_u8>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s16 String::ToInt16(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_s16>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u16 String::ToUInt16(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_u16>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 String::ToInt32(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_s32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 String::ToUInt32(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_u32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s64 String::ToInt64(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_s64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 String::ToUInt64(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_u64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_f32 String::ToFloat(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_f32>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
_f64 String::ToDouble(bool _ignoreLeadingZero) const
{
	return StringUtil::ToNumber<_f64>(SafeSource(), nullptr, _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::TryToInt8(OUT _s8& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::TryToUInt8(OUT _u8& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::TryToInt16(OUT _s16& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::TryToUInt16(OUT _u16& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::TryToInt32(OUT _s32& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::TryToUInt32(OUT _u32& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::TryToInt64(OUT _s64& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::TryToUInt64(OUT _u64& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::TryToFloat(OUT _f32& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool String::TryToDouble(OUT _f64& _outValue, bool _ignoreLeadingZero) const
{
	return StringUtil::TryToNumber(_outValue, SafeSource(), _ignoreLeadingZero);
}

NS_END


