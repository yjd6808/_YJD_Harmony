/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 5:03:54 PM
 * =====================
 * 쓰레드세이프 하지 않음, 동적생성 금지
 */


#pragma once

#include <jc/TypeTraits.h>
#include <jc/Primitives/RefCountObjectPtr.h>

#include <jc/Container/Vector.h>
#include <jc/Container/HashMap.h>

#include <sgs/IUpdatable.h>
#include <sgs/IComponent.h>
#include <sgs/ISessionEventHandler.h>

class ComponentCollection : public IUpdatable, public ISessionEventHandler
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
		static_assert(jc::IsNaturalType_v<TComponent>, "... TComponent must be natural type");
		static_assert(jc::IsBaseOf_v<IComponent, TComponent>, "... TComponent must be derived from IComponent");

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
	jc::RefCountObjectPtr<TComponent> GetRPtr() const
	{
		static_assert(jc::IsNaturalType_v<TComponent>, "... TComponent must be natural type");
		static_assert(jc::IsBaseOf_v<IComponent, TComponent>, "... TComponent must be derived from IComponent");

		IComponent** pFoundComponent = componentMap_.Find(TComponent::GetType());

		if (pFoundComponent == nullptr)
		{
			return nullptr;
		}

		return jc::RefCountObjectPtr<TComponent>{ dynamic_cast<TComponent*>(*pFoundComponent) };
	}

	void OnUpdate(const jc::TimeSpan& _elapsed) override;
	void OnConnected() override;
	void OnDisconnected() override;

private:
	jc::HashMap<int, IComponent*> componentMap_;
	jc::Vector<IUpdatable*> updatables_;
	jc::Vector<ISessionEventHandler*> sessionEventHandlers_;
};
