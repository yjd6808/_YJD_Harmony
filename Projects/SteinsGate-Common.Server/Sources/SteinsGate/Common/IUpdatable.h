#pragma once



namespace JCore
{
	struct TimeSpan;
}

struct IUpdatable
{
	virtual ~IUpdatable() = default;
	virtual void OnUpdate(const JCore::TimeSpan& elapsed) = 0;
};