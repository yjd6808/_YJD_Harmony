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

#include <TF/Common/Base/IUpdatable.h>
#include <TF/Common/Base/IComponent.h>
#include <TF/Common/Base/ISessionEventHandler.h>

class ComponentCollection
	: public IUpdatable
	, public ISessionEventHandler
{
public:
	ComponentCollection();
	ComponentCollection(int capacity);
	~ComponentCollection() override;

	void Clear();
	bool Add(IComponent* component);
	bool Has(int type) const;
	bool Remove(int type);
	void Initialize();

	template <typename TComponent>
	TComponent* Get(bool addRef = false) const {
		static_assert(JCore::IsNaturalType_v<TComponent>, "... TComponent must be natural type");
		static_assert(JCore::IsBaseOf_v<IComponent, TComponent>, "... TComponent must be derived from IComponent");

		IComponent** ppFoundComponent = m_hComponentMap.Find(TComponent::GetType());

		if (ppFoundComponent == nullptr) {
			return nullptr;
		}
		TComponent* pComponent = dynamic_cast<TComponent*>(*ppFoundComponent);
		if (addRef) pComponent->AddRef();
		return pComponent;
	}

	template <typename TComponent>
	JCore::RefCountObjectPtr<TComponent> GetRPtr() const {
		static_assert(JCore::IsNaturalType_v<TComponent>, "... TComponent must be natural type");
		static_assert(JCore::IsBaseOf_v<IComponent, TComponent>, "... TComponent must be derived from IComponent");

		IComponent** ppFoundComponent = m_hComponentMap.Find(TComponent::GetType());

		if (ppFoundComponent == nullptr) {
			return nullptr;
		}

		return JCore::RefCountObjectPtr<TComponent>{ dynamic_cast<TComponent*>(*ppFoundComponent) };
	}

	void OnUpdate(const JCore::TimeSpan& elapsed) override;
	void OnConnected() override;
	void OnDisconnected() override;
private:
	JCore::HashMap<int, IComponent*> m_hComponentMap;
	JCore::Vector<IUpdatable*> m_vUpdatables;
	JCore::Vector<ISessionEventHandler*> m_vSessionEventHandlers;
};



