/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:15:04 AM
 * =====================
 *
 */


#pragma once

#include "jc/Container/Vector.h"
#include "sgs/IUpdatable.h"

NS_SG_BEGIN

class UpdatableCollection
{
public:
	void Update(const jc::TimeSpan& _elapsed);

	bool Add(int _id, IUpdatable* _pUpdatable);
	bool Has(int _id);
	bool Has(IUpdatable* _pUpdatable);
	bool Remove(int _id);
	bool Remove(IUpdatable* _pUpdatable);
	void Clear();

	template <typename TUpdatable>
	TUpdatable* Get(int _id) const
	{
		static_assert(jc::IsNaturalType_v<TUpdatable>, "... TUpdatable must be natural type");
		static_assert(jc::IsBaseOf_v<IUpdatable, TUpdatable>, "... TUpdatable must be derived from IUpdatable");

		IUpdatable** pFoundUpdatablePtr = updatableMap_.Find(_id);

		if (pFoundUpdatablePtr == nullptr)
		{
			return nullptr;
		}

		return dynamic_cast<TUpdatable*>(*pFoundUpdatablePtr);
	}

private:
	jc::HashMap<int, IUpdatable*> updatableMap_;
	jc::Vector<IUpdatable*> updatable_;
};

NS_END