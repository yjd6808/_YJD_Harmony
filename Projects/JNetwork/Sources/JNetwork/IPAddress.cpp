#include <JNetwork/Network.h>
#include <JNetwork/IPAddress.h>

#include <JCore/Ascii.h>
#include <JCore/Exception.h>
#include <JCore/Limit.h>
#include <JCore/StaticString.h>




namespace JNetwork {

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
		throw InvalidArgumentException("�ε����� 0�̻� 3���ϸ� ���ڷ� ���� �� �ֽ��ϴ�.");
	}

	return m_Addr.Seg[3 - idx];
}


String IPv4Address::ToString() const {
	String s(32);
	std::ostringstream o;
	
	for (int i = 0; i < 4; i++) {
		// tip : << �����ڰ� &���� �켱������ ���Ƽ� ���� ()�� �ȹ���
		o << (int)GetAddressOctet(i);
		if (i == 3)
			s.Append(o.str());
		else
			s.Append(o.str() + '.');

		// tip : ostringstream ���� ��
		// @���� : https://stackoverflow.com/questions/5288036/how-to-clear-ostringstream
		o.str("");		// ���� ���ڿ��� ���ڿ��� �ٲ���
		// o.clear();	// Ȱ��ȭ�� ���� �÷��׸� ������
	}

	return s;
}

IPv4Address IPv4Address::Parse(const char* hostOrderedAddressString) {
	IPv4Address result = IPv4Address::Any();

	int addr_idx = 0;
	int temp_idx = 0;

	// xxx.xxx.xxx.xxx��� IP�ּ� ���ڿ��� �Ľ��� ��
	// step�� 0�϶��� ���� ������ xxx ������ ���������� �ϰ�
	// step�� 1�϶��� �ι�° xxx ���ڿ��� ������ �����ؼ� ���������� �ϴ� ���
	int step = 0;		
	StaticString<4> temp;

	// xxx.xxx.xxx.xxx
	// -----------
	// �տ� 3���� xxx�� ���� ������ ������ ������

	while (hostOrderedAddressString[addr_idx] != NULL) {
		if (!IsNumeric(hostOrderedAddressString[addr_idx]) && hostOrderedAddressString[addr_idx] != '.')
			throw InvalidArgumentException("�ùٸ� IPv4 �ּҸ� �������ּ���. ���� �Ǵ� ��(.)�� �ƴ� ���ڰ� ���ԵǾ� �ֽ��ϴ�.");

		if (hostOrderedAddressString[addr_idx] == '.') {
			temp.Source[temp_idx] = '\0';
			const int val = atoi(temp.Source);
			if (val > 255) {
				throw InvalidArgumentException("�ùٸ� IPv4 �ּҸ� �������ּ���. 255.255.255.255���� ū IP �ּ��Դϴ�.");
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
		throw InvalidArgumentException("�ùٸ� IPv4 �ּҸ� �������ּ���. ��(.)�� 3������ �մϴ�.");

	// xxx.xxx.xxx.xxx
	//             ---
	// ������ xxx�� ���� ������ ������ ������
	temp.Source[temp_idx] = '\0';
	const int val = atoi(temp.Source);
	if (val > 255) {
		throw InvalidArgumentException("�ùٸ� IPv4 �ּҸ� �������ּ���. 255.255.255.255���� ū IP �ּ��Դϴ�.");
	}
	result.m_Addr.Seg[0] = (Byte)val;

	return result;
}


/*=====================================================================================
							IPv6Address
=======================================================================================*/

}

