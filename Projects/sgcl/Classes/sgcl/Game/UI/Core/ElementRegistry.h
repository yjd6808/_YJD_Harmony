/*
 * 작성자: 윤정도
 * =====================
 * sgui - UI 요소 고유 ID 레지스트리
 * 모든 UIElement는 생성 시 auto-increment ID를 할당받고, 파괴 시 해제된다.
 * ID 랩어라운드 시에도 이미 사용중인 ID는 중복 할당되지 않는다.
 */

#pragma once

#include "sgcl/Game/UI/Core/UITypes.h"

#include <unordered_map>

namespace sgui
{

class UIElement;

class ElementRegistry
{
public:
	static constexpr _u32 InvalidId_v = 0;

	static ElementRegistry* Get();

	_u32 Allocate(UIElement* _pElement);
	void Release(_u32 _id, UIElement* _pElement);

	UIElement* Find(_u32 _id) const;

	template <typename TElement>
	TElement* Find(_u32 _id) const { return dynamic_cast<TElement*>(Find(_id)); }

	int GetCount() const { return (int)elementMap_.size(); }

private:
	std::unordered_map<_u32, UIElement*> elementMap_;
	_u32 next_ = 1;
};

} // namespace sgui
