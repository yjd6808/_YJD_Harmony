/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 9:44:58 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/Session.h>

#include <TF/Common/Base/IComponent.h>
#include <TF/Common/Base/ISessionEventHandler.h>

class SessionComponent : public IComponent, public ISessionEventHandler
{
public:
	SessionComponent(JNetwork::Session* session);
	JNetwork::Session* GetSession() const { return m_pSession; }
protected:
	JNetwork::Session* m_pSession;
};
