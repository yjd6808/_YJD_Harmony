#pragma once

#include <sg/Namespace.h>

NS_JC_BEGIN
struct TimeSpan;
NS_END

NS_SG_BEGIN

struct IUpdatable
{
	virtual ~IUpdatable() = default;
	virtual void OnUpdate(const jc::TimeSpan& _elapsed) = 0;
};

NS_END