/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:15:04 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/Vector.h>
#include <SteinsGate/Common/IUpdatable.h>

class UpdatableCollection
{
public:
	void Update(const JCore::TimeSpan& elapsed);

	bool Add(int id, IUpdatable* updatable);
	bool Has(int id);
	bool Has(IUpdatable* updatable);
	bool Remove(int id);
	bool Remove(IUpdatable* updatable);
	void Clear();

	template <typename TUpdatable>
	TUpdatable* Get(int id) const {
		static_assert(JCore::IsNaturalType_v<TUpdatable>, "... TUpdatable must be natural type");
		static_assert(JCore::IsBaseOf_v<IUpdatable, TUpdatable>, "... TUpdatable must be derived from IUpdatable");

		IUpdatable** ppFoundUpdatable = m_hUpdatableMap.Find(id);

		if (ppFoundUpdatable == nullptr) {
			return nullptr;
		}

		return dynamic_cast<TUpdatable*>(*ppFoundUpdatable);
	}
private:
	JCore::HashMap<int, IUpdatable*> m_hUpdatableMap;
	JCore::Vector<IUpdatable*> m_vUpdatable;
};


