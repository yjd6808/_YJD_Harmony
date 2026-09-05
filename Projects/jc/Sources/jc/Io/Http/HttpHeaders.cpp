#include "jc/IO/Http/HttpHeaders.h"
#include "jc/Primitives/StringUtil.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
jc::String HttpHeaders::NormalizeKey(const jc::String& _name)
{
	return _name.ToLowerCase();
}

//////////////////////////////////////////////////////////////////////////////////////////
void HttpHeaders::Set(const jc::String& _name, const jc::String& _value)
{
	jc::String key = NormalizeKey(_name);
	auto pFound = headers_.Find(key);
	if (pFound != nullptr)
	{
		*pFound = _value;
		return;
	}
	headers_.Insert(key, _value);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HttpHeaders::TryGet(const jc::String& _name, OUT jc::String& _value) const
{
	jc::String key = NormalizeKey(_name);
	auto pFound = headers_.Find(key);
	if (pFound == nullptr)
		return false;
	_value = *pFound;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HttpHeaders::Contains(const jc::String& _name) const
{
	jc::String key = NormalizeKey(_name);
	return headers_.Exist(key);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HttpHeaders::Remove(const jc::String& _name)
{
	jc::String key = NormalizeKey(_name);
	return headers_.Remove(key);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HttpHeaders::Clear()
{
	headers_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
_s64 HttpHeaders::GetContentLength() const
{
	jc::String value;
	if (!TryGet("content-length", OUT value))
		return -1;
	_s64 length = -1;
	if (!value.TryToInt64(OUT length))
		return -1;
	return length;
}

//////////////////////////////////////////////////////////////////////////////////////////
int HttpHeaders::Count() const
{
	return headers_.Size();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool HttpHeaders::IsEmpty() const
{
	return headers_.IsEmpty();
}

NS_END

