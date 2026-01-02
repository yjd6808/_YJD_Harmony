#pragma once

#include <sg/Namepsace.h>

NS_JC_BEGIN
struct TimeSpan;
NS_JC_END

struct IUpdatable
{
	virtual ~IUpdatable() = default;
	virtual void OnUpdate(const jc::TimeSpan& _elapsed) = 0;
};
