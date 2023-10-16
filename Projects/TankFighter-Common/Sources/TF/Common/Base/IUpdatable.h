#pragma once

#include <TF/Common/Namespace.h>

NS_JC_BEGIN
struct TimeSpan;
NS_JC_END

struct IUpdatable
{
	virtual ~IUpdatable() = default;
	virtual void OnUpdate(const JCore::TimeSpan& elapsed) = 0;
};