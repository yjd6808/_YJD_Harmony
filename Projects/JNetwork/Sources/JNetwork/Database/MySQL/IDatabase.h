/*
	작성자 : 윤정도
*/

#pragma once

#include <JNetwork/Database/IConnection.h>

namespace JNetwork {

class IDatabase
{
	virtual ~IDatabase() = default;
	virtual IConnection* GetConnection() = 0;
	virtual bool Connect() = 0;
};

} // namespace JNetwork