/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 3:15:18 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/Vector.h>
#include <JCore/TypeTraits.h>

#include <SteinsGate/Client/IUpdatable.h>
#include <SteinsGate/Client/IComponent.h>

class ComponentCollection : public IUpdatable
{
public:
	ComponentCollection();
	ComponentCollection(int capacity);
	~ComponentCollection() override;

	void clear();
	bool add(IComponent* component);
	bool has(IComponent::Type type) const;
	bool remove(IComponent::Type type);

	template <typename TComponent>
	TComponent* get() const {
		static_assert(JCore::IsNaturalType_v<TComponent>, "... TComponent must be natural type");
		static_assert(JCore::IsBaseOf_v<IComponent, TComponent>, "... TComponent must be derived from IComponent");

		IComponent::Type eType = TComponent::type();
		IComponent** ppFoundComponent = m_hComponentMap.Find(int(eType));

		if (ppFoundComponent == nullptr) {
			return nullptr;
		}

		return dynamic_cast<TComponent*>(*ppFoundComponent);
	}

	void onUpdate(float dt) override;
private:
	JCore::HashMap<int, IComponent*> m_hComponentMap;
	JCore::Vector<IUpdatable*> m_vUpdatables;
};


