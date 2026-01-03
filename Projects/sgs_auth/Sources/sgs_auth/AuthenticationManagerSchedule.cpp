/*
 * 작성자: 윤정도
 * 생성일: 6/15/2023 1:48:33 AM
 * =====================
 *
 */

#include "Core.h"
#include "AuthCoreHeader.h"
#include "AuthenticationManagerSchedule.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
void AuthenticationManagerSchedule::Initialize()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthenticationManagerSchedule::Finalize()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthenticationManagerSchedule::OnFirstScheduled()
{
	JC_PASS;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthenticationManagerSchedule::OnScheduled()
{
	if (!AuthenticationManager::Singleton_IsDeleted()) // TODO: 싱글톤 삭제될 때 호출되는 케이스는 애초에 없어야한다. 매니저 삭제전에 스케쥴러가 먼저 정지가 되어야함,
	{
		AuthenticationManager::Get()->OnScheduled(this);
	}
}
