/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 9:44:58 PM
 * =====================
 *
 */

#pragma once

#include "sgs/_Net/NetSession.h"
#include "sgs/IComponent.h"
#include "sgs/ISessionEventHandler.h"

NS_SG_BEGIN

class SessionComponent : public IComponent, public ISessionEventHandler
{
public:
	SessionComponent(sg::NetSession* _pSession);
	sg::NetSession* GetSession() const { return pSession_; }

protected:
	sg::NetSession* pSession_;
};

NS_END