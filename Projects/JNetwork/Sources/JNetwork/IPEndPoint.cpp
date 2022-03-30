#include <JNetwork/Network.h>
#include <JNetwork/IPEndPoint.h>

#include <JCore/StaticString.h>
#include <JCore/StringUtil.h>
#include <JCore/Exception.h>
#include <JCore/Ascii.h>

namespace JNetwork {

	using namespace JCore;

	/*=====================================================================================
								IPEndPoint
	=======================================================================================*/
	IPEndPoint::~IPEndPoint() = default;


	/*=====================================================================================
								IPv4EndPoint
	=======================================================================================*/

	IPv4EndPoint::IPv4EndPoint() {
		m_Address = IPv4Address::Any();
		m_uiPort = 0;
	}

	IPv4EndPoint::IPv4EndPoint(const IPv4Address& ipAddr, const Int16U port) {
		m_Address = ipAddr;
		m_uiPort = port;
	}

	IPv4EndPoint::IPv4EndPoint(const char* endPointAddrString) {
		*this = Parse(endPointAddrString);
	}

	IPv4EndPoint::IPv4EndPoint(const JCore::String& endPointAddrString) {
		*this = Parse(endPointAddrString.Source());
	}

	String IPv4EndPoint::ToString() const {
		String ret = GetAddress().ToString() + ':';
		ret += m_uiPort;
		return ret;
	}

	IPv4EndPoint IPv4EndPoint::Parse(const char* endPointAddr) {
		constexpr int EndPointLen = IPv4Len_v + 6; // +6 : ��Ʈ �ִ� 5�ڸ� + ���� ':'�� ������ ����
		using IPv4EndPointString = StaticString<EndPointLen>;

		const int iLen = StringUtil::Length(endPointAddr);

		if (iLen > EndPointLen) {
			throw InvalidArgumentException("�ùٸ��� ���� EndPoint �����Դϴ�. ���ڿ� ���̰� �ִ�� ������ EndPoint ���̸� �ʰ��մϴ�.");
		}

		IPv4EndPointString ep{};
		ep.CopyFrom(endPointAddr);
		const int idx = ep.Find(":");

		if (idx == -1) {
			throw InvalidArgumentException("�ùٸ��� ���� EndPoint �����Դϴ�. ������ ':'�� ã�� ���߽��ϴ�.");
		}

		IPv4EndPointString ipAddrString{};
		ipAddrString.CopyFrom(0, idx - 1, const_cast<char*>(ep.Source));

		IPv4EndPointString portString{};
		portString.CopyFrom(idx + 1, ep.Length() - 1, const_cast<char*>(ep.Source));
		const int iPortStrLen = portString.Length();

		for (int i = 0; i < iPortStrLen; i++) {
			if (!IsNumeric(portString[i])) {
				throw InvalidArgumentException("�ùٸ��� ���� EndPoint �����Դϴ�. �߸��� ���ڰ� ���ԵǾ��ֽ��ϴ�.");
			}
		}

		const int iPort = atoi(portString.Source);
		if (iPort < 0 || iPort > 0xffff) {
			throw InvalidArgumentException("�ùٸ��� ���� EndPoint �����Դϴ�. ��Ʈ��ȣ�� 0�̻� 65535���ϸ� �����մϴ�.");
		}

		return IPv4EndPoint(ipAddrString.Source, atoi(portString.Source));
	}

	IPv4EndPoint IPv4EndPoint::Parse(const JCore::String& endPointAddr) {
		return Parse(endPointAddr.Source());
	}
}
