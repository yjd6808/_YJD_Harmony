/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Core/ElementRegistry.h"

namespace sgui
{

ElementRegistry* ElementRegistry::Get()
{
	static ElementRegistry s_registry;
	return &s_registry;
}

_u32 ElementRegistry::Allocate(UIElement* _pElement)
{
	for (;;)
	{
		const _u32 id = next_++;

		if (id == InvalidId_v)
		{
			// 오버플로우 랩어라운드: 0은 무효 ID이므로 건너뛴다.
			continue;
		}

		if (elementMap_.find(id) != elementMap_.end())
		{
			// 랩어라운드 이후 아직 살아있는 요소가 점유중인 ID는 재사용하지 않는다.
			continue;
		}

		elementMap_.emplace(id, _pElement);
		return id;
	}
}

void ElementRegistry::Release(_u32 _id, UIElement* _pElement)
{
	if (_id == InvalidId_v)
	{
		return;
	}

	auto it = elementMap_.find(_id);

	if (it == elementMap_.end())
	{
		_LogWarn_("[ElementRegistry] 해제하려는 ID(%u)가 등록되어있지 않습니다.", _id);
		return;
	}

	if (it->second != _pElement)
	{
		_LogWarn_("[ElementRegistry] ID(%u)의 소유 요소가 일치하지 않아 해제를 무시합니다.", _id);
		return;
	}

	elementMap_.erase(it);
}

UIElement* ElementRegistry::Find(_u32 _id) const
{
	const auto it = elementMap_.find(_id);
	return it != elementMap_.end() ? it->second : nullptr;
}

} // namespace sgui
