/*
 * 작성자: 윤정도
 * 생성일: 3/13/2026
 * =====================
 *
 * jdb 공통 유틸리티 구현
 */

#include "Util.h"

USING_NS_JC;

NS_JDB_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
String Util::ReplacePlaceholders(const String& _stmt, const String* _pArgs, int _argCount)
{
	const char* pSrc = _stmt.Source();
	const int len = _stmt.Length();

	String result(len * 2);

	for (int i = 0; i < len; ++i)
	{
		if (pSrc[i] == '{')
		{
			int numStart = i + 1;
			int numEnd = numStart;

			while (numEnd < len && pSrc[numEnd] >= '0' && pSrc[numEnd] <= '9')
				++numEnd;

			if (numEnd > numStart && numEnd < len && pSrc[numEnd] == '}')
			{
				int index = 0;
				for (int k = numStart; k < numEnd; ++k)
					index = index * 10 + (pSrc[k] - '0');

				if (index >= 0 && index < _argCount)
				{
					result += _pArgs[index];
					i = numEnd;
					continue;
				}

				// Query(select * from t_test where c_uid = {0}, {1}, "abcd")
				// -> {1}에 대응하는 인자를 못찾음
				jc_assert_msg(false, "cannot find {%d} argument.\nsource: %s", index, pSrc);
			}
		}

		result += pSrc[i];
	}

	return result;
}



NS_END
