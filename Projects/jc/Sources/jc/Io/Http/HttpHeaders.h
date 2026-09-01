#pragma once

#include "jc/Primitives/String.h"
#include "jc/Container/TreeMap.h"

#include "jc/Namespace.h"

NS_JC_BEGIN

class HttpHeaders
{
public:
	HttpHeaders() = default;
	HttpHeaders(const HttpHeaders& _other) = default;
	HttpHeaders(HttpHeaders&& _other) noexcept = default;
	HttpHeaders& operator=(const HttpHeaders& _other) = default;
	HttpHeaders& operator=(HttpHeaders&& _other) noexcept = default;
	~HttpHeaders() = default;

	void Set(const jc::String& _name, const jc::String& _value);
	bool TryGet(const jc::String& _name, OUT jc::String& _value) const;
	bool Contains(const jc::String& _name) const;
	bool Remove(const jc::String& _name);
	void Clear();

	_s64 GetContentLength() const;
	int Count() const;
	bool IsEmpty() const;

	const jc::TreeMap<jc::String, jc::String>& GetMap() const { return headers_; }
	jc::TreeMap<jc::String, jc::String>& GetMap() { return headers_; }

	auto Begin() const { return headers_.Begin(); }
	auto End() const { return headers_.End(); }

private:
	static jc::String NormalizeKey(const jc::String& _name);

private:
	jc::TreeMap<jc::String, jc::String> headers_;
};

NS_END

