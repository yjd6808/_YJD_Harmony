/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 5:04:25 PM
 * =====================
 *
 */


#include "Core.h"
#include "ServerCoreHeader.h"
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
	componentMap_.ForEachValueRelease();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ComponentCollection::Clear()
{
	componentMap_.ForEachValueRelease();
	componentMap_.Clear();
	updatables_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ComponentCollection::Add(IComponent* _pComponent)
{
	const bool isAdded = componentMap_.Insert(_pComponent->GetType(), _pComponent);
	IUpdatable* pUpdatableComponent = dynamic_cast<IUpdatable*>(_pComponent);
	ISessionEventHandler* pSessionEventHandlerComponent = dynamic_cast<ISessionEventHandler*>(_pComponent);

	if (isAdded && pUpdatableComponent != nullptr)
	{
		updatables_.PushBack(pUpdatableComponent);
	}

	if (isAdded && pSessionEventHandlerComponent != nullptr)
	{
		sessionEventHandlers_.PushBack(pSessionEventHandlerComponent);
	}

	return isAdded;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ComponentCollection::Has(int _type) const
{
	return componentMap_.Exist(_type);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ComponentCollection::Remove(int _type)
{
	IComponent** pFoundComponent = componentMap_.Find(_type);

	if (pFoundComponent == nullptr)
	{
		return false;
	}

	IUpdatable* pUpdatableComponent = dynamic_cast<IUpdatable*>(*pFoundComponent);
	ISessionEventHandler* pSessionEventHandlerComponent = dynamic_cast<ISessionEventHandler*>(*pFoundComponent);

	if (pUpdatableComponent)
	{
		const bool isRemoved = updatables_.Remove(pUpdatableComponent);
		jc_assert(isRemoved);
	}

	if (pSessionEventHandlerComponent)
	{
		const bool isRemoved = sessionEventHandlers_.Remove(pSessionEventHandlerComponent);
		jc_assert(isRemoved);
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
void ComponentCollection::OnUpdate(const jc::TimeSpan& _elapsed)
{
	for (int index = 0; index < updatables_.Size(); ++index)
	{
		updatables_[index]->OnUpdate(_elapsed);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ComponentCollection::OnConnected()
{
	for (int index = 0; index < sessionEventHandlers_.Size(); ++index)
	{
		sessionEventHandlers_[index]->OnConnected();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ComponentCollection::OnDisconnected()
{
	for (int index = 0; index < sessionEventHandlers_.Size(); ++index)
	{
		sessionEventHandlers_[index]->OnDisconnected();
	}
}
