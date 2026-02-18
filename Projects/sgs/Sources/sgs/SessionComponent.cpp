/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 9:47:34 PM
 * =====================
 *
 */

#include "Core.h"
#include "SessionComponent.h"

USING_NS_SG;

////////////////////////////////////////////////////////////////////////////////////////////////////
SessionComponent::SessionComponent(sg::NetSession* _pSession)
: pSession_(_pSession)
{
}
