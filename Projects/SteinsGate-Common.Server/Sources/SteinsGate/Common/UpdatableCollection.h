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
	void Update(const JCore::TimeSpan& _elapsed);

	bool Add(int _id, IUpdatable* _pUpdatable);
	bool Has(int _id);
	bool Has(IUpdatable* _pUpdatable);
	bool Remove(int _id);
	bool Remove(IUpdatable* _pUpdatable);
	void Clear();

	template <typename TUpdatable>
	TUpdatable* Get(int _id) const
	{
		static_assert(JCore::IsNaturalType_v<TUpdatable>, "... TUpdatable must be natural type");
		static_assert(JCore::IsBaseOf_v<IUpdatable, TUpdatable>, "... TUpdatable must be derived from IUpdatable");

		IUpdatable** pFoundUpdatablePtr = updatableMap_.Find(_id);

		if (pFoundUpdatablePtr == nullptr)
		{
			return nullptr;
		}

		return dynamic_cast<TUpdatable*>(*pFoundUpdatablePtr);
	}

private:
	JCore::HashMap<int, IUpdatable*> updatableMap_;
	JCore::Vector<IUpdatable*> updatable_;
};
