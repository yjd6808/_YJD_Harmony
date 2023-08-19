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
	T Generate(T inclusiveBegin, T inclusiveEnd) {
		if (inclusiveBegin > inclusiveEnd) {
			throw InvalidArgumentException("begin > end 되면 안댐");
		}

		std::uniform_int_distribution<T> dist(inclusiveBegin, inclusiveEnd);
		return dist(ms_DefaultRandomEngine);
	}

	static int GenerateInt(int inclusiveBegin, int exclusiveEnd);
	static double GenerateDouble(double inclusiveBegin, double inclusiveEnd);

	
	template <typename T>
	static const T& Pick(std::initializer_list<T> ilist) {
		if (ilist.size() == 0) throw InvalidArgumentException("최소 1개이상의 엘리먼트가 있어야합니다.");

		const int iSelectedIndex = Random::GenerateInt(0, ilist.size());
		for (int i = 0; const T & value : ilist) {
			if (i == iSelectedIndex) {
				return value;
			}
			++i;
		}

		return *ilist.begin();
	}

	template <typename T, typename TAllocator>
	static T& Pick(const Collection<T, TAllocator>& collection) {
		if (collection.Size() == 0) throw InvalidArgumentException("최소 1개이상의 엘리먼트가 있어야합니다.");
		const int iSelectedIndex = Random::GenerateInt(0, collection.Size());
		auto it = collection.Begin();
		for (int i = 0; i < collection.Size(); ++i) {
			if (i == iSelectedIndex) return it->Current();
			it->Next();
		}
		return it->Current();
	}

	static char GenerateAlphabat();
	
	static void EngineInitialize();
private:
	inline static bool ms_bInitialized;
    inline static std::mt19937 ms_DefaultRandomEngine;
    inline static std::random_device ms_RandomDevice;
};


NS_DETAIL_BEGIN
template <typename T>
struct RandomPicker
{
	
};

template <template <typename> typename Collection, typename T>
struct RandomPicker<Collection<T>>
{
	
};


NS_DETAIL_END

NS_JC_END