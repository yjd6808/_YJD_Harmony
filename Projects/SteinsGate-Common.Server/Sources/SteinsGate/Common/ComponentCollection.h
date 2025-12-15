/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 5:03:54 PM
 * =====================
 * 쓰레드세이프 하지 않음, 동적생성 금지
 */


#pragma once

#include <JCore/TypeTraits.h>
#include <JCore/Primitives/RefCountObjectPtr.h>

#include <JCore/Container/Vector.h>
#include <JCore/Container/HashMap.h>

#include <SteinsGate/Common/IUpdatable.h>
#include <SteinsGate/Common/IComponent.h>
#include <SteinsGate/Common/ISessionEventHandler.h>

class ComponentCollection
	: public IUpdatable
	  , public ISessionEventHandler
{
public:
	ComponentCollection();
	ComponentCollection(int _capacity);
	~ComponentCollection() override;

	void Clear();
	bool Add(IComponent* _pComponent);
	bool Has(int _type) const;
	bool Remove(int _type);
	void Initialize();

	template <typename TComponent>
	TComponent* Get(bool _addRef = false) const
	{
		static_assert(JCore::IsNaturalType_v<TComponent>, "... TComponent must be natural type");
		static_assert(JCore::IsBaseOf_v<IComponent, TComponent>, "... TComponent must be derived from IComponent");

		IComponent** pFoundComponent = componentMap_.Find(TComponent::GetType());

		if (pFoundComponent == nullptr)
		{
			return nullptr;
		}

		TComponent* pComponent = dynamic_cast<TComponent*>(*pFoundComponent);
		if (_addRef)
		{
			pComponent->AddRef();
		}

		return pComponent;
	}

	template <typename TComponent>
	JCore::RefCountObjectPtr<TComponent> GetRPtr() const
	{
		static_assert(JCore::IsNaturalType_v<TComponent>, "... TComponent must be natural type");
		static_assert(JCore::IsBaseOf_v<IComponent, TComponent>, "... TComponent must be derived from IComponent");

		IComponent** pFoundComponent = componentMap_.Find(TComponent::GetType());

		if (pFoundComponent == nullptr)
		{
			return nullptr;
		}

		return JCore::RefCountObjectPtr<TComponent>{ dynamic_cast<TComponent*>(*pFoundComponent) };
	}

	void OnUpdate(const JCore::TimeSpan& _elapsed) override;
	void OnConnected() override;
	void OnDisconnected() override;

private:
	JCore::HashMap<int, IComponent*> componentMap_;
	JCore::Vector<IUpdatable*> updatables_;
	JCore::Vector<ISessionEventHandler*> sessionEventHandlers_;
};
