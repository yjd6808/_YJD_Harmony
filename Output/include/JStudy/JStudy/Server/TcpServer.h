#pragma once

#include <JStudy/Base.h>

namespace JStudy {

class TcpServer : public Base
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

