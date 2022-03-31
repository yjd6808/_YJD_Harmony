/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/String.h>

namespace JNetwork {

struct IConnection
{
	virtual ~IConnection() = default;
	virtual void ExecuteQuery(const JCore::String& query) = 0;
};

} // namespace JNetwork