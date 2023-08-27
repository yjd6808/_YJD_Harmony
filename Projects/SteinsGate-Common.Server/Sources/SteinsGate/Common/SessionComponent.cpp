/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 9:47:34 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "SessionComponent.h"

SessionComponent::SessionComponent(CommonSession* session) : m_pSession(session) {}
