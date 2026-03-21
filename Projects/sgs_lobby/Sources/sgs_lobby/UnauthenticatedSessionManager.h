/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 8:27:52 PM
 * =====================
 * 로비서버에서만 씀 (나중에 옮겨야할 듯)
 */


#pragma once

#include "jc/Pattern/Singleton.h"

NS_SG_BEGIN

class NetSession;
class UnauthenticatedSessionManager : public jc::SingletonPointer<UnauthenticatedSessionManager>
{
public:
	bool Add(AuthenticationSerial_t _serial, sg::NetSession* _pSession);
	bool Remove(AuthenticationSerial_t _serial);

private:
	jc::NormalLock sync_;
	jc::HashMap<AuthenticationSerial_t, sg::NetSession*> sessionMap_;
};

NS_END

#define g_cUnauthenticatedSessionManager JC_DECL_SINGLETON_BODY(sg::UnauthenticatedSessionManager)