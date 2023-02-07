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

/*=====================================================================================
							IPAddress
=======================================================================================*/


/*=====================================================================================
							IPv4Address
=======================================================================================*/

IPv4Address::IPv4Address(const char* hostOrderedAddressString) {
	*this = Parse(hostOrderedAddressString);
}

Byte IPv4Address::GetAddressOctet(int idx) const {
	if (idx < 0 || idx > 3) {
		throw InvalidArgumentException("인덱스는 0이상 3이하만 인자로 받을 수 있습니다.");
	}

	return m_Addr.Seg[3 - idx];
}


String IPv4Address::ToString() const {
	String s(32);
	std::ostringstream o;
	
	for (int i = 0; i < 4; i++) {
		// tip : << 연산자가 &보다 우선순위가 높아서 굳이 ()로 안묶음
		o << (int)GetAddressOctet(i);
		if (i == 3)
			s.Append(o.str());
		else
			s.Append(o.str() + '.');

		// tip : ostringstream 비우는 법
		// @참고 : https://stackoverflow.com/questions/5288036/how-to-clear-ostringstream
		o.str("");		// 내부 문자열을 빈문자열로 바꿔줌
		// o.clear();	// 활성화된 에러 플래그를 지워줌
	}

	return s;
}

IPv4Address IPv4Address::Parse(const char* hostOrderedAddressString) {
	IPv4Address result = IPv4Address::Any();

	int addr_idx = 0;
	int temp_idx = 0;

	// xxx.xxx.xxx.xxx라는 IP주소 문자열을 파싱할 때
	// step이 0일때는 제일 왼쪽의 xxx 정수를 가져오도록 하고
	// step이 1일때는 두번째 xxx 문자열을 정수로 변경해서 가져오도록 하는 방식
	int step = 0;		
	StaticString<4> temp;

	// xxx.xxx.xxx.xxx
	// -----------
	// 앞에 3개의 xxx에 대한 정보를 정수로 변경함

	while (hostOrderedAddressString[addr_idx] != NULL) {
		if (!IsNumeric(hostOrderedAddressString[addr_idx]) && hostOrderedAddressString[addr_idx] != '.')
			throw InvalidArgumentException("올바른 IPv4 주소를 전달해주세요. 숫자 또는 점(.)이 아닌 문자가 포함되어 있습니다.");

		if (hostOrderedAddressString[addr_idx] == '.') {
			temp.Source[temp_idx] = '\0';
			const int val = atoi(temp.Source);
			if (val > 255) {
				throw InvalidArgumentException("올바른 IPv4 주소를 전달해주세요. 255.255.255.255보다 큰 IP 주소입니다.");
			}
			result.m_Addr.Seg[3 - step] = (Byte)val;
			step++;
			temp_idx = -1;
		} else {
			temp.Source[temp_idx] = hostOrderedAddressString[addr_idx];
		}

		addr_idx++;
		temp_idx++;
	}

	if (step != 3)
		throw InvalidArgumentException("올바른 IPv4 주소를 전달해주세요. 점(.)이 3개여야 합니다.");

	// xxx.xxx.xxx.xxx
	//             ---
	// 마지막 xxx에 대한 정보를 정수로 변경함
	temp.Source[temp_idx] = '\0';
	const int val = atoi(temp.Source);
	if (val > 255) {
		throw InvalidArgumentException("올바른 IPv4 주소를 전달해주세요. 255.255.255.255보다 큰 IP 주소입니다.");
	}
	result.m_Addr.Seg[0] = (Byte)val;

	return result;
}


/*=====================================================================================
							IPv6Address
=======================================================================================*/

}

