/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 4:14:21 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ComponentCollection.h"

//////////////////////////////////////////////////////////////////////////////////////////
ComponentCollection::ComponentCollection()
: componentMap_(0)
, updatables_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ComponentCollection::ComponentCollection(int _capacity)
: componentMap_(_capacity)
, updatables_(_capacity)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ComponentCollection::~ComponentCollection()
{
	componentMap_.ForEachValueDelete();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ComponentCollection::Clear()
{
	componentMap_.ForEachValueDelete();
	componentMap_.Clear();
	updatables_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ComponentCollection::Add(IComponent* _pComponent)
{
	const bool added = componentMap_.Insert(static_cast<int>(_pComponent->GetType()), _pComponent);
	IUpdatable* pUpdatableComponent = dynamic_cast<IUpdatable*>(_pComponent);

	if (added && pUpdatableComponent != nullptr)
	{
		updatables_.PushBack(pUpdatableComponent);
	}

	return added;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ComponentCollection::Has(IComponent::Type _type) const
{
	return componentMap_.Exist(_type);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ComponentCollection::Remove(IComponent::Type _type)
{
	IComponent** pFoundComponent = componentMap_.Find(_type);

	if (pFoundComponent == nullptr)
	{
		return false;
	}

	IUpdatable* pUpdatableComponent = dynamic_cast<IUpdatable*>(*pFoundComponent);

	if (pUpdatableComponent != nullptr)
	{
		const bool removed = updatables_.Remove(pUpdatableComponent);
		DebugAssert(removed);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ComponentCollection::Initialize()
{
	componentMap_.ForEachValue([](IComponent* _pComponent)
	{
		_pComponent->Initialize();
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
void ComponentCollection::OnUpdate(float _dt)
{
	for (int i = 0; i < updatables_.Size(); ++i)
	{
		updatables_[i]->OnUpdate(_dt);
	}
}
