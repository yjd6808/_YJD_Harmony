/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Primitives/String.h>

namespace JNetwork {

struct IConnection
{
	virtual ~IConnection() = default;
	virtual void ExecuteQuery(const JCore::String& query) = 0;
};

} // namespace JNetwork