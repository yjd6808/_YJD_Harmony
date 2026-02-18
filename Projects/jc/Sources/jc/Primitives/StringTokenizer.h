/*
 * 작성자: 윤정도
 * 생성일: 7/7/2024 9:10:07 PM
 * =====================
 * 무지성으로 만듬 (CMTokenizer랑 비슷한 느낌으루다가)
 */


#pragma once

#include <jc/Primitives/String.h>
#include <jc/Container/Vector.h>

NS_JC_BEGIN

class StringTokenizer
{
public:
	StringTokenizer(const char* _str);
	StringTokenizer(const String& _str);

	bool	IsDelim(char _delim);
	void	AddDelim(char _delim);

	bool    HasNext();
	String	Next(OUT char* _catchedDelim = nullptr);

private:
	jc::Vector<char> delims_;
	const char* pStr_;
	int strLen_ = 0;
	int prev_ = 0;
	int cur_ = 0;
};

NS_END