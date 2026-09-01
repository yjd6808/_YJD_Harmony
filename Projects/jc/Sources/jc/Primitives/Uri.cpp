#include "jc/Primitives/Uri.h"

#include "jc/Primitives/StringUtil.h"
#include "jc/Ascii.h"
#include "jc/FileSystem/Path.h"

NS_JC_BEGIN

namespace
{
	//////////////////////////////////////////////////////////////////////////////////////
	String TrimCopy(const String& _s)
	{
		if (_s.IsEmpty())
			return _s;

		const char* p = _s.Source();
		int len = _s.Length();
		int start = 0;
		int end = len - 1;

		while (start <= end && IsSpace(p[start]))
			++start;
		while (end >= start && IsSpace(p[end]))
			--end;

		if (start > end)
			return String::Empty;

		if (start == 0 && end == len - 1)
			return _s;

		return _s.SubStr(start, end - start + 1);
	}

	//////////////////////////////////////////////////////////////////////////////////////
	bool StartsWithIgnoreCase(const String& _s, const char* _prefix)
	{
		int preLen = StringUtil::Length(_prefix);
		if (_s.Length() < preLen)
			return false;
		return _s.Find(_prefix, false) == 0;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	bool Contains(const String& _s, const char* _sub)
	{
		return _s.Find(_sub, true) != -1;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	String SubFrom(const String& _s, int _start)
	{
		if (_start < 0)
			_start = 0;
		if (_start >= _s.Length())
			return String::Empty;
		return _s.SubStr(_start, _s.Length() - _start);
	}

	//////////////////////////////////////////////////////////////////////////////////////
	String SubRange(const String& _s, int _start, int _count)
	{
		if (_count <= 0)
			return String::Empty;
		if (_start < 0)
			_start = 0;
		if (_start >= _s.Length())
			return String::Empty;
		if (_start + _count > _s.Length())
			_count = _s.Length() - _start;
		return _s.SubStr(_start, _count);
	}

	//////////////////////////////////////////////////////////////////////////////////////
	bool TryParseInt(const String& _s, OUT int& _out)
	{
		if (_s.IsEmpty())
			return false;
		return _s.TryToInt32(OUT _out);
	}

	//////////////////////////////////////////////////////////////////////////////////////
	String LastSegmentOf(const String& _path)
	{
		if (_path.IsEmpty())
			return String::Empty;
		int lastSlash = _path.FindReverse("/", true);
		int lastBack = _path.FindReverse("\\", true);
		int last = lastSlash > lastBack ? lastSlash : lastBack;
		if (last < 0)
			return _path;
		return SubFrom(_path, last + 1);
	}

	//////////////////////////////////////////////////////////////////////////////////////
	String NormalizeSeparatorsCopy(const String& _path)
	{
		if (_path.IsEmpty())
			return _path;
		String copy = _path;
		int len = copy.Length();
		for (int i = 0; i < len; ++i)
		{
			if (copy[i] == '/')
				copy.SetAt(i, '\\');
		}
		return copy;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
String Uri::GetPathAndQuery() const
{
	if (query_.IsEmpty())
		return path_;
	String ret = path_;
	ret.Append('?');
	ret.Append(query_);
	return ret;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Uri::Parse(const String& _raw)
{
	raw_ = TrimCopy(_raw);
	if (raw_.IsEmpty())
	{
		valid_ = false;
		return false;
	}

	// reset
	host_ = String::Empty;
	path_ = String::Empty;
	query_ = String::Empty;
	fileName_ = String::Empty;
	port_ = 0;
	scheme_ = UriScheme::usUnknown;
	valid_ = false;

	String rest;
	if (StartsWithIgnoreCase(raw_, "http://"))
	{
		scheme_ = UriScheme::usHttp;
		port_ = 80;
		rest = SubFrom(raw_, 7);
	}
	else if (StartsWithIgnoreCase(raw_, "https://"))
	{
		scheme_ = UriScheme::usHttps;
		port_ = 443;
		rest = SubFrom(raw_, 8);
	}
	else if (StartsWithIgnoreCase(raw_, "file://"))
	{
		scheme_ = UriScheme::usFile;
		path_ = SubFrom(raw_, 7);
		return FinishLocal();
	}
	else if (Contains(raw_, "://"))
	{
		valid_ = false;
		return false;
	}
	else
	{
		scheme_ = UriScheme::usFile;
		path_ = raw_;
		return FinishLocal();
	}

	// http(s): host[:port][/path][?query]
	int slash = rest.Find("/", true);
	String authority;
	String pathPart;
	if (slash < 0)
	{
		authority = rest;
		pathPart = "/";
	}
	else
	{
		authority = SubRange(rest, 0, slash);
		pathPart = SubFrom(rest, slash);
	}

	if (authority.IsEmpty())
	{
		valid_ = false;
		return false;
	}

	int colon = authority.Find(":", true);
	if (colon < 0)
	{
		host_ = authority;
	}
	else
	{
		host_ = SubRange(authority, 0, colon);
		String portStr = SubFrom(authority, colon + 1);
		if (!TryParseInt(portStr, OUT port_))
		{
			valid_ = false;
			return false;
		}
	}

	int q = pathPart.Find("?", true);
	if (q < 0)
	{
		path_ = pathPart;
		query_ = String::Empty;
	}
	else
	{
		path_ = SubRange(pathPart, 0, q);
		query_ = SubFrom(pathPart, q + 1);
	}

	fileName_ = LastSegmentOf(path_);
	valid_ = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Uri::Parse(const char* _raw)
{
	if (_raw == nullptr)
	{
		raw_ = String::Empty;
		host_ = String::Empty;
		path_ = String::Empty;
		query_ = String::Empty;
		fileName_ = String::Empty;
		scheme_ = UriScheme::usUnknown;
		port_ = 0;
		valid_ = false;
		return false;
	}
	return Parse(String(_raw));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Uri::FinishLocal()
{
	path_ = NormalizeSeparatorsCopy(path_);
	fileName_ = Path::FileName(path_);
	valid_ = !path_.IsEmpty();
	return valid_;
}

NS_END
