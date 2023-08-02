/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:15:04 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/Vector.h>
#include <SteinsGate/Common/IUpdatable.h>

class UpdatableCollection : JCore::Vector<IUpdatable*>
{
public:
	void OnUpdate(const JCore::TimeSpan& elapsed);
};
