/*
 * 작성자: 윤정도
 * 생성일: 7/7/2024 9:10:17 PM
 * =====================
 *
 */

#include "StringTokenizer.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
StringTokenizer::StringTokenizer(const char* _str)
: pStr_(_str)
{
	strLen_ = StringUtil::Length(_str);
	delims_.Reserve(4);
}

//////////////////////////////////////////////////////////////////////////////////////////
StringTokenizer::StringTokenizer(const String& _str)
: pStr_(_str.Source())
{
	strLen_ = _str.Length();
	delims_.Reserve(4);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringTokenizer::IsDelim(char _delim)
{
	return delims_.Exist(_delim);
}

//////////////////////////////////////////////////////////////////////////////////////////
void StringTokenizer::AddDelim(char _delim)
{
	if (IsDelim(_delim))
		return;

	delims_.PushBack(_delim);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool StringTokenizer::HasNext()
{
	if (cur_ >= strLen_)
		return false;

	// 현재 위치부터 남은 문자열에서 비딜리미터가 있는지 확인
	for (int i = cur_; i < strLen_; ++i)
	{
		if (!IsDelim(pStr_[i]))
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
String StringTokenizer::Next(OUT char* _catchedDelim /*= nullptr*/)
{
	// 연속된 딜리미터 스킵
	while (cur_ < strLen_ && IsDelim(pStr_[cur_]))
	{
		++cur_;
	}

	// 토큰 시작 위치 설정 (prev_ 업데이트)
	prev_ = cur_;

	// 토큰 끝 찾기
	for (; cur_ < strLen_; ++cur_)
	{
		char ch = pStr_[cur_];
		if (IsDelim(ch))
		{
			if (_catchedDelim)
			{
				*_catchedDelim = ch;
			}
			break;
		}
	}

	String ret = StringUtil::SubStr(pStr_, strLen_, prev_, cur_ - prev_);
	cur_ = cur_ + 1;	// 딜리미터 스킵
	return ret;
}


NS_END
