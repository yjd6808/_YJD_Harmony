#include "jc/Io/Http/HttpResponse.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
HttpResponse::~HttpResponse()
{
	CloseBody();
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::MemoryStreamPtr HttpResponse::TakeBody()
{
	taken_ = true;
	jc::MemoryStreamPtr spBody = spBody_;
	spBody_ = nullptr;
	return spBody;
}

//////////////////////////////////////////////////////////////////////////////////////////
int HttpResponse::ReadBody(void* _pBuffer, int _len)
{
	if (spBody_ != nullptr)
		return (int)spBody_->Read(_pBuffer, (_u32)_len);
	if (spConn_ != nullptr)
		return spConn_->ReadBody(_pBuffer, _len);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void HttpResponse::CloseBody()
{
	if (spConn_ != nullptr)
	{
		spConn_->Close();
		spConn_ = nullptr;
	}
}

NS_END


