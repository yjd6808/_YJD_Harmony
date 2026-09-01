#pragma once

#include "jc/Namespace.h"
#include "jc/Type.h"
#include "jc/Primitives/String.h"

NS_JC_BEGIN

enum UriScheme
{
	usUnknown,
	usFile,
	usHttp,
	usHttps
};

class Uri
{
public:
	Uri() = default;
	Uri(const char* _raw) { Parse(_raw); }
	Uri(const String& _raw) { Parse(_raw); }

	Uri(const Uri& _other) = default;
	Uri(Uri&& _other) noexcept = default;
	Uri& operator=(const Uri& _other) = default;
	Uri& operator=(Uri&& _other) noexcept = default;
	~Uri() = default;

	bool IsValid() const { return valid_; }
	bool IsLocal() const { return scheme_ == UriScheme::usFile; }
	bool IsRemote() const { return scheme_ == UriScheme::usHttp || scheme_ == UriScheme::usHttps; }

	UriScheme GetScheme() const { return scheme_; }
	const String& GetRaw() const { return raw_; }
	const String& GetHost() const { return host_; }
	int GetPort() const { return port_; }
	const String& GetPath() const { return path_; }
	const String& GetQuery() const { return query_; }
	const String& GetFileName() const { return fileName_; }
	const String& GetLocalPath() const { return path_; }
	String GetPathAndQuery() const;

private:
	bool Parse(const String& _raw);
	bool Parse(const char* _raw);
	bool FinishLocal();

private:
	UriScheme scheme_ = UriScheme::usUnknown;
	String raw_;
	String host_;
	String path_;
	String query_;
	String fileName_;
	int port_ = 0;
	bool valid_ = false;
};

using Url = Uri;

NS_END
