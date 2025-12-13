/*
	작성자 : 윤정도
	난수 생성기
*/

#pragma once

#include <JCore/Exception.h>
#include <JCore/Container/Collection.h>

#include <random>

NS_JC_BEGIN
struct Random final
{
	Random();

	template <typename T>
	static T Generate(T _inclusiveBegin, T _inclusiveEnd)
	{
		if (_inclusiveBegin > _inclusiveEnd)
		{
			throw InvalidArgumentException("begin > end 되면 안댐");
		}

		std::uniform_int_distribution<T> dist(_inclusiveBegin, _inclusiveEnd);
		return dist(DefaultRandomEngine);
	}

	template <typename T>
	static T GenerateF(T _inclusiveBegin, T _inclusiveEnd)
	{
		if (_inclusiveBegin > _inclusiveEnd)
		{
			throw InvalidArgumentException("begin > end 되면 안댐");
		}

		std::uniform_real_distribution<T> dist(_inclusiveBegin, _inclusiveEnd);
		return dist(DefaultRandomEngine);
	}

	static int GenerateInt(int _inclusiveBegin, int _exclusiveEnd);
	static double GenerateDouble(double _inclusiveBegin, double _inclusiveEnd);

	template <typename T>
	static const T& Pick(std::initializer_list<T> _ilist)
	{
		if (_ilist.size() == 0)
		{
			throw InvalidArgumentException("최소 1개이상의 엘리먼트가 있어야합니다.");
		}

		const int selectedIndex = Random::GenerateInt(0, static_cast<int>(_ilist.size()));
		int index = 0;
		for (const T& value : _ilist)
		{
			if (index == selectedIndex)
			{
				return value;
			}

			++index;
		}

		return *_ilist.begin();
	}

	template <typename T, typename TAllocator>
	static T& Pick(const Collection<T, TAllocator>& _collection)
	{
		if (_collection.Size() == 0)
		{
			throw InvalidArgumentException("최소 1개이상의 엘리먼트가 있어야합니다.");
		}

		const int selectedIndex = Random::GenerateInt(0, _collection.Size());
		auto it = _collection.Begin();

		for (int index = 0; index < _collection.Size(); ++index)
		{
			if (index == selectedIndex)
			{
				return it->Current();
			}

			it->Next();
		}

		return it->Current();
	}

	static char GenerateAlphabat();

	// 파라메터: 백분율로 표현된 확률
	// ex) Chance(70.0f) -> 70% 확률로 true 반환
	static bool Chance(float _percentProbability);
	static bool Chance(double _percentProbability);
	static void EngineInitialize();

	static void WriteRandomAlphabatTextBuffered(int _length, char* _pBuff, int _capacity);

private:
	inline static bool Initialized;
	inline static std::mt19937 DefaultRandomEngine;
	inline static std::random_device RandomDevice;
};

NS_DETAIL_BEGIN
template <typename T>
struct RandomPicker
{
};

template <template <typename> typename TCollection, typename T>
struct RandomPicker<TCollection<T>>
{
};

NS_DETAIL_END

NS_JC_END
