/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 4:05:43 AM
 * =====================
 *
 */

#include "InterServerSendHelper.h"

#include "sgs/Net/NetCore.h"

USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
InterServerSendHelperBase::Information::Information()
: sender_(nullptr)
, strategy_(SendStrategy::SendAlloc)
, toId_(InvalidValue_v)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void InterServerSendHelperBase::FlushSendBuffer()
{
	if (SendInformation.sender_ == nullptr)
	{
		_LogError_("샌더 미할당");
		return;
	}

	SendInformation.sender_->FlushSendBuffer();
}

//////////////////////////////////////////////////////////////////////////////////////////
void InterServerSendHelperBase::SetInformation(Session* _pSender, SendStrategy _strategy, int _toServerId)
{
	if (!IsValidInformation(_pSender, _strategy, _toServerId))
	{
		return;
	}

	SendInformation.sender_ = _pSender;
	SendInformation.strategy_ = _strategy;

	if (_toServerId == InvalidValue_v)
		return;

	SendInformation.toId_ = _toServerId;
	// TODO: 리팩토링해야함. 다 지워야함.
	// SendInformation.destination_ = sg::ServerProcessInfoPackage->infoMap_[SendInformation.toId_]->remoteInterServerEp_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void InterServerSendHelperBase::SetInformation(Session* _pSender, SendStrategy _strategy, SingleServerType_t _toServerType)
{
	jc_assert(_toServerType >= SingleServerType::Begin && _toServerType <= SingleServerType::End);

	if (!SendHelperBase::IsValidInformation(_pSender, _strategy))
	{
		return;
	}

	SendInformation.sender_ = _pSender;
	SendInformation.strategy_ = _strategy;
	SendInformation.toId_ = SingleServerId[_toServerType];
	// SendInformation.destination_ = sg::ServerProcessInfoPackage->infoMap_[SendInformation.toId_]->remoteInterServerEp_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void InterServerSendHelperBase::SendEnd(IPacket* _packet)
{
	switch (SendInformation.strategy_)
	{
	case SendStrategy::SendAsync:
		SendInformation.sender_->SendAsync(_packet);
		break;
	case SendStrategy::SendToAsync:
		SendInformation.sender_->SendToAsync(_packet, SendInformation.destination_);
		break;
	case SendStrategy::SendAlloc:
		// 할거 없음
		break;
	default:
		_LogWarn_("전송전략이 제대로 설정되어있지 않습니다. (커맨드 유실 위험)");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool InterServerSendHelperBase::IsValidInformation(Session* _pSender, SendStrategy _strategy, int _toServerId)
{
	if (!SendHelperBase::IsValidInformation(_pSender, _strategy))
	{
		return false;
	}

	if (_pSender->Protocol() == TransportProtocol::UDP)
	{
		if (_toServerId == InvalidValue_v)
		{
			_LogWarn_("UDP인데 송신지 ID를 기입하지 않습니다.");
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
int InterServerSendHelperBase::GetSenderProcessType()
{
	return g_cNetCore.GetProcessType();
}
