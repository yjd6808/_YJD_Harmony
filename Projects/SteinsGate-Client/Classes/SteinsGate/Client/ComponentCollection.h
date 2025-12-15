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
	ComponentCollection(int _capacity);
	~ComponentCollection() override;

	void clear();
	bool add(IComponent* _pComponent);
	bool has(IComponent::Type _type) const;
	bool remove(IComponent::Type _type);
	void initialize();

	template <typename TComponent>
	TComponent* get() const
	{
		static_assert(JCore::IsNaturalType_v<TComponent>, "... TComponent must be natural type");
		static_assert(JCore::IsBaseOf_v<IComponent, TComponent>, "... TComponent must be derived from IComponent");

		IComponent::Type type = TComponent::type();
		IComponent** pFoundComponent = componentMap_.Find(type);

		if (pFoundComponent == nullptr)
		{
			return nullptr;
		}

		return dynamic_cast<TComponent*>(*pFoundComponent);
	}

	void onUpdate(float _dt) override;

private:
	JCore::HashMap<int, IComponent*> componentMap_;
	JCore::Vector<IUpdatable*> updatables_;
};
