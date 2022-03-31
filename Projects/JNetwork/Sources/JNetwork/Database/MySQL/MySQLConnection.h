/*
	작성자 : 윤정도
*/

#pragma once

#include <JNetwork/Database/IConnection.h>

namespace JNetwork {

class MySQLConnection : IConnection
{
public:
	MySQLConnection();
	~MySQLConnection() override {}
public:


private:
};

} // namespace JNetwork