#pragma once

#include <JStudy/ServerBase.h>

namespace JStudy {

class TcpServer : public ServerBase
{
protected:
	bool Bind();
	bool Listen();
	virtual void Accept() = 0;
public:
	virtual void Open();
	virtual void Close();
};

} // namespace JStudy

