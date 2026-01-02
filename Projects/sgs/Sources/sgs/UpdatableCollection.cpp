/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:18:15 AM
 * =====================
 *
 */


#include "Core.h"
#include "ServerCoreHeader.h"
#include "UpdatableCollection.h"

//////////////////////////////////////////////////////////////////////////////////////////
void UpdatableCollection::Update(const jc::TimeSpan& _elapsed)
{
	const int size = updatable_.Size();
	for (int i = 0; i < size; ++i)
	{
		updatable_[i]->OnUpdate(_elapsed);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UpdatableCollection::Add(int _id, IUpdatable* _pUpdatable)
{
	if (!updatableMap_.Insert(_id, _pUpdatable))
	{
		return false;
	}

	updatable_.PushBack(_pUpdatable);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UpdatableCollection::Has(int _id)
{
	return updatableMap_.Exist(_id);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UpdatableCollection::Has(IUpdatable* _pUpdatable)
{
	for (int i = 0; i < updatable_.Size(); ++i)
	{
		if (updatable_[i] == _pUpdatable)
		{
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UpdatableCollection::Remove(int _id)
{
	IUpdatable** pFoundUpdatablePtr = updatableMap_.Find(_id);
	if (pFoundUpdatablePtr == nullptr)
	{
		return false;
	}

	if (updatable_.Remove(*pFoundUpdatablePtr) && updatableMap_.Remove(_id))
	{
		return true;
	}

	jc_assert(false);
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UpdatableCollection::Remove(IUpdatable* _pUpdatable)
{
	bool found = false;
	int foundId = InvalidValue_v;

	updatableMap_.ForEach([&](jc::Pair<int, IUpdatable*> _pair)
	{
		if (found)
		{
			return;
		}

		if (_pUpdatable == _pair.value_)
		{
			found = true;
			foundId = _pair.key_;
		}
	});

	if (found)
	{
		return updatableMap_.Remove(foundId) && updatable_.Remove(_pUpdatable);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UpdatableCollection::Clear()
{
	updatableMap_.Clear();
	updatable_.Clear();
}
