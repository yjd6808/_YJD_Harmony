/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/IPAddress.h>

#include <JCore/Primitives/StaticString.h>
#include <JCore/Ascii.h>
#include <JCore/Exception.h>
#include <JCore/Limit.h>

NS_JNET_BEGIN

using namespace JCore;

//////////////////////////////////////////////////////////////////////////////////////////
IPv4Address::IPv4Address(const char* _pHostOrderedAddressString)
{
	*this = Parse(_pHostOrderedAddressString);
}

//////////////////////////////////////////////////////////////////////////////////////////
Byte IPv4Address::GetAddressOctet(int _index) const
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
	String result(32);
	std::ostringstream stream;

	for (int index = 0; index < 4; ++index)
	{
		// tip : << 연산자가 &보다 우선순위가 높아서 굳이 ()로 안묶음
		stream << static_cast<int>(GetAddressOctet(index));

		if (index == 3)
		{
			result.Append(stream.str());
		}
		else
		{
			result.Append(stream.str() + '.');
		}

		// tip : ostringstream 비우는 법
		// @참고 : https://stackoverflow.com/questions/5288036/how-to-clear-ostringstream
		stream.str(""); // 내부 문자열을 빈문자열로 바꿔줌
		// stream.clear(); // 활성화된 에러 플래그를 지워줌
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
IPv4Address IPv4Address::Parse(const char* _pHostOrderedAddressString)
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

	while (_pHostOrderedAddressString[addressIndex] != '\0')
	{
		if (!IsNumeric(_pHostOrderedAddressString[addressIndex]) && _pHostOrderedAddressString[addressIndex] != '.')
		{
			DebugAssertMsg(false, "올바른 IPv4 주소를 전달해주세요. 숫자 또는 점(.)이 아닌 문자가 포함되어 있습니다.");
		}

		if (_pHostOrderedAddressString[addressIndex] == '.')
		{
			temp.Source[tempIndex] = '\0';
			const int value = atoi(temp.Source);
			DebugAssertMsg(value <= 255, "올바른 IPv4 주소를 전달해주세요. 255.255.255.255보다 큰 IP 주소입니다.");
			result.address_.seg_[3 - step] = static_cast<Byte>(value);
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

	DebugAssertMsg(step == 3, "올바른 IPv4 주소를 전달해주세요. 점(.)이 3개여야 합니다.");

	// xxx.xxx.xxx.xxx
	//             ---
	// 마지막 xxx에 대한 정보를 정수로 변경함
	temp.Source[tempIndex] = '\0';
	const int value = atoi(temp.Source);
	DebugAssertMsg(value <= 255, "올바른 IPv4 주소를 전달해주세요. 255.255.255.255보다 큰 IP 주소입니다.");
	result.address_.seg_[0] = static_cast<Byte>(value);

	return result;
}

NS_JNET_END
