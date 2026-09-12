/*
 * 작성자 : 윤정도
 */

#include "jnet/IPAddress.h"

#include "jc/Primitives/StaticString.h"
#include "jc/Primitives/StringUtil.h"
#include "jc/Wrapper/CRuntime.h"
#include "jc/Ascii.h"
#include "jc/Exception.h"
#include "jc/Limit.h"

NS_JNET_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
IPv4Address::IPv4Address(const _char* _pHostOrderedAddressString)
{
	*this = Parse(_pHostOrderedAddressString);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u8 IPv4Address::GetAddressOctet(int _index) const
{
	if (_index < 0 || _index > 3)
	{
		throw InvalidArgumentException("인덱스는 0이상 3이하만 인자로 받을 수 있습니다.");
	}

	return address_.seg_[3 - _index];
}

//////////////////////////////////////////////////////////////////////////////////////////
String IPv4Address::ToString() const
{
	return StringUtil::Format(_T("%d.%d.%d.%d"),
		static_cast<int>(GetAddressOctet(0)), static_cast<int>(GetAddressOctet(1)),
		static_cast<int>(GetAddressOctet(2)), static_cast<int>(GetAddressOctet(3)));
}

//////////////////////////////////////////////////////////////////////////////////////////
IPv4Address IPv4Address::Parse(const _char* _pHostOrderedAddressString)
{
	IPv4Address result = IPv4Address::Any();

	int addressIndex = 0;
	int tempIndex = 0;

	// xxx.xxx.xxx.xxx라는 IP주소 문자열을 파싱할 때
	// step이 0일때는 제일 왼쪽의 xxx 정수를 가져오도록 하고
	// step이 1일때는 두번째 xxx 문자열을 정수로 변경해서 가져오도록 하는 방식
	int step = 0;
	StaticString<4> temp;

	// xxx.xxx.xxx.xxx
	// -----------
	// 앞에 3개의 xxx에 대한 정보를 정수로 변경함

	while (_pHostOrderedAddressString[addressIndex] != _T('\0'))
	{
		if (!IsNumeric(_pHostOrderedAddressString[addressIndex]) && _pHostOrderedAddressString[addressIndex] != _T('.'))
		{
			jc_assert_msg(false, _T("올바른 IPv4 주소를 전달해주세요. 숫자 또는 점(.)이 아닌 문자가 포함되어 있습니다."));
		}

		if (_pHostOrderedAddressString[addressIndex] == _T('.'))
		{
			temp.Source[tempIndex] = _T('\0');
			const int value = CRuntime::StrToLong(temp.Source, nullptr, 10);
			jc_assert_msg(value <= 255, _T("올바른 IPv4 주소를 전달해주세요. 255.255.255.255보다 큰 IP 주소입니다."));
			result.address_.seg_[3 - step] = static_cast<_u8>(value);
			++step;
			tempIndex = -1;
		}
		else
		{
			temp.Source[tempIndex] = _pHostOrderedAddressString[addressIndex];
		}

		++addressIndex;
		++tempIndex;
	}

	jc_assert_msg(step == 3, _T("올바른 IPv4 주소를 전달해주세요. 점(.)이 3개여야 합니다."));

	// xxx.xxx.xxx.xxx
	//             ---
	// 마지막 xxx에 대한 정보를 정수로 변경함
	temp.Source[tempIndex] = _T('\0');
	const int value = CRuntime::StrToLong(temp.Source, nullptr, 10);
	jc_assert_msg(value <= 255, _T("올바른 IPv4 주소를 전달해주세요. 255.255.255.255보다 큰 IP 주소입니다."));
	result.address_.seg_[0] = static_cast<_u8>(value);

	return result;
}

NS_END
