/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 9:44:58 PM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Common/CommonSession.h>
#include <SteinsGate/Common/IComponent.h>
#include <SteinsGate/Common/ISessionEventHandler.h>

class SessionComponent : public IComponent, public ISessionEventHandler
{
public:
	SessionComponent(CommonSession* _pSession);
	CommonSession* GetSession() const { return pSession_; }

protected:
	CommonSession* pSession_;
};
