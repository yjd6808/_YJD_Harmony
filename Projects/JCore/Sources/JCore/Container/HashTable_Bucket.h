/*
 * 작성자: 윤정도
 * 생성일: 8/15/2023 3:18:37 AM
 * =====================
 * HashTable에서 사용할 노드와 버킷
 */


#pragma once

#include <JCore/Hasher.h>
#include <JCore/Memory.h>

#include <JCore/Container/Pair.h>

NS_JC_BEGIN

template <typename T>
struct BucketNode
{
	T Data;
	Int32U Hash;	// 처음에 한번 계산해놓으면 성능이 좀더 개선될 듯?
};

/* ==============================================================
 *
 * 12월 25일 일요일 크리스마스 기념! 버킷을 공허의 다이나믹 배열로 변경
 * 공허의 다이나믹 배열?: 데이터를 넣을때 초기화를 진행한다! 데이터를 넣기전까진 無의 상태
 * 보통 1개의 데이터를 넣게 되므로 초기 용량은 1로 설정하고 이후 +2씩 커지도록 하였다.
 * 근데 3개까지 찰 확률은 거의 없다.
 *
 * ============================================================== */
template <typename...>
struct Bucket;

#pragma region Bucket<TKey, TAllocator>

// 해쉬셋을 위한 버킷
template <typename TKey, typename TAllocator>
struct Bucket<TKey, TAllocator>
{
	using TBucketNode = BucketNode<TKey>;

	Bucket()
		: Next(nullptr)
		, Previous(nullptr)
		, DynamicArray(nullptr)
		, Capacity(1)
		, Size(0) {}
	~Bucket() {
		JCORE_PLACEMENT_DELETE_ARRAY_SAFE(DynamicArray, Size);
		JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(DynamicArray, sizeof(TBucketNode) * Capacity);
	}

	template <typename... Args>
	void EmplaceBack(Args&&... args) {
		if (!IsValid())
			Initialize();

		if (IsFull())
			Expand(Capacity + 2);

		Memory::PlacementNew(DynamicArray[Size++], Forward<Args>(args)...);
	}

	void PushBack(const TBucketNode& data) {
		if (!IsValid())
			Initialize();

		if (IsFull())
			Expand(Capacity + 2);

		Memory::PlacementNew(DynamicArray[Size++], data);
	}

	void PushBack(TBucketNode&& data) {
		if (!IsValid())
			Initialize();

		if (IsFull())
			Expand(Capacity + 2);

		Memory::PlacementNew(DynamicArray[Size++], Move(data));
	}

	void Initialize() {
		int iAllocated;
		DynamicArray = TAllocator::template AllocateDynamic<TBucketNode*>(sizeof(TBucketNode) * Capacity, iAllocated);
		// Memory::Set(DynamicArray, sizeof(TBucketNode) * Capacity, 0);
	}

	void Expand(int newCapacity) {
		int iAllocated;
		TBucketNode* pNewDynamicArray = TAllocator::template AllocateDynamic<TBucketNode*>(sizeof(TBucketNode) * newCapacity, iAllocated);

		if constexpr (!IsFundamentalType_v<TKey> && !IsPointerType_v<TKey>) {
			if constexpr (IsStringType_v<TKey>)
				Memory::PlacementNewArray(pNewDynamicArray, newCapacity, TBucketNode{ { 0 }, 0 });	// 문자열은 동적할당 안된 상태로 생성해주자. String(0)는 동적할당안함
			else
				DebugAssert(false); // String도 아니고 int같은 기본 타입도 아닌 새로운 키타입을 추가하고자 한다면 성능향상을 위해 여기서 직접 수정 ㄱ, 그냥 Memory::PlacementNewArray(pNewDynamicArray, newCapacity)를 수행해도 동작하는데 문제없긴함
		}

		for (int i = 0; i < Size; i++) {
			pNewDynamicArray[i] = Move(DynamicArray[i]);	// 2023/02/23 Move 필수.. Value가 포인터라지만 Key가 String 같은 타입일 수가 있다. 만약 Move를 안하고 Copy를 한다면 DynamicArray의 메모리를 해제하기전에 수동으로 소멸자 호출을 해줘야함.
		}

		TAllocator::DeallocateDynamic(DynamicArray, sizeof(TBucketNode) * Capacity);
		DynamicArray = pNewDynamicArray;
		Capacity = newCapacity;
	}

	void Clear() {
		JCORE_PLACEMENT_DELETE_ARRAY_SAFE(DynamicArray, Size);
		Size = 0;
	}

	bool Exist(const TKey& key) {
		for (int i = 0; i < Size; i++) {
			if (DynamicArray[i].Data == key) {
				return true;
			}
		}

		return false;
	}

	TKey* Find(const TKey& key) {
		for (int i = 0; i < Size; i++) {
			if (DynamicArray[i].Data == key) {
				return AddressOf(DynamicArray[i].Data);
			}
		}

		return nullptr;
	}

	bool Remove(const TKey& key) {
		int iFind = -1;
		for (int i = 0; i < Size; i++) {
			if (DynamicArray[i].Data == key) {
				Memory::PlacementDelete(DynamicArray[i]);
				iFind = i;
				break;
			}
		}

		if (iFind == -1) {
			return false;
		}

		for (int i = iFind; i < Size - 1; i++) {
			DynamicArray[i] = Move(DynamicArray[i + 1]);
		}

		--Size;
		return true;
	}

	bool IsFull() {
		return Size == Capacity;
	}

	bool IsEmpty() {
		return Size == 0;
	}

	bool IsValid() {
		return DynamicArray != nullptr;
	}


	TBucketNode& GetAt(const int idx) {
		DebugAssert(DynamicArray);
		DebugAssert(idx >= 0 && idx < Size);
		return DynamicArray[idx];
	}

	Bucket* Next;
	Bucket* Previous;
	TBucketNode* DynamicArray;
	int Capacity;
	int Size;

};

#pragma endregion

#pragma region Bucket<TKey, TValue, TAllocator>
template <typename TKey, typename TValue, typename TAllocator>
struct Bucket<TKey, TValue, TAllocator>
{
	using TBucketNode = BucketNode<Pair<TKey, TValue>>;

	Bucket()
		: Next(nullptr)
		, Previous(nullptr)
		, DynamicArray(nullptr)
		, Capacity(1)
		, Size(0) {}
	~Bucket() {
		JCORE_PLACEMENT_DELETE_ARRAY_SAFE(DynamicArray, Size);
		JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(DynamicArray, sizeof(TBucketNode) * Capacity);
	}

	template <typename... Args>
	void EmplaceBack(Args&&... args) {
		if (!IsValid())
			Initialize();

		if (IsFull())
			Expand(Capacity + 2);

		Memory::PlacementNew(DynamicArray[Size++], Forward<Args>(args)...);
	}

	void PushBack(const TBucketNode& data) {
		if (!IsValid())
			Initialize();

		if (IsFull())
			Expand(Capacity + 2);

		Memory::PlacementNew(DynamicArray[Size++], data);
	}

	void PushBack(TBucketNode&& data) {
		if (!IsValid())
			Initialize();

		if (IsFull())
			Expand(Capacity + 2);

		Memory::PlacementNew(DynamicArray[Size++], Move(data));
	}

	void Initialize() {
		int iAllocated;
		DynamicArray = TAllocator::template AllocateDynamic<TBucketNode*>(sizeof(TBucketNode) * Capacity, iAllocated);
		// Memory::Set(DynamicArray, sizeof(TBucketNode) * Capacity, 0);
	}

	void Expand(int newCapacity) {
		int iAllocated;
		TBucketNode* pNewDynamicArray = TAllocator::template AllocateDynamic<TBucketNode*>(sizeof(TBucketNode) * newCapacity, iAllocated);

		// 2023/02/23
		// 키가 String 같은 타입인 경우 그냥 대입 해버리면 Key의 생성자로 초기화가 수행이 안되어있기때문에 오류가 발생한다.
		// String의 경우 m_pBuffer가 nullptr로 초기화가 되지 않음
		// 기본 생성자를 호출해서 초기화를 해놔야한다.
		if constexpr (IsPointerType_v<TValue> && !IsFundamentalType_v<TKey> && !IsPointerType_v<TKey>) {
			if constexpr (IsStringType_v<TKey>)
				Memory::PlacementNewArray(pNewDynamicArray, newCapacity, TBucketNode{ { 0, nullptr }, 0 });	// 문자열은 동적할당 안된 상태로 생성해주자. String(0)는 동적할당안함
			else
				DebugAssert(false); // String도 아니고 int같은 기본 타입도 아닌 새로운 키타입을 추가하고자 한다면 성능향상을 위해 여기서 직접 수정 ㄱ, 그냥 Memory::PlacementNewArray(pNewDynamicArray, newCapacity)를 수행해도 동작하는데 문제없긴함
		}

		for (int i = 0; i < Size; i++) {
			if constexpr (IsPointerType_v<TValue>)
				pNewDynamicArray[i] = Move(DynamicArray[i]);	// 2023/02/23 Move 필수.. Value가 포인터라지만 Key가 String 같은 타입일 수가 있다. 만약 Move를 안하고 Copy를 한다면 DynamicArray의 메모리를 해제하기전에 수동으로 소멸자 호출을 해줘야함.
			else
				Memory::PlacementNew(pNewDynamicArray[i], Move(DynamicArray[i]));
		}

		TAllocator::DeallocateDynamic(DynamicArray, sizeof(TBucketNode) * Capacity);
		DynamicArray = pNewDynamicArray;
		Capacity = newCapacity;
	}

	void Clear() {
		JCORE_PLACEMENT_DELETE_ARRAY_SAFE(DynamicArray, Size);
		Size = 0;
	}

	bool Exist(const TKey& key) {
		for (int i = 0; i < Size; i++) {
			if (DynamicArray[i].Data.Key == key) {
				return true;
			}
		}

		return false;
	}

	TValue* Find(const TKey& key) {
		for (int i = 0; i < Size; i++) {
			if (DynamicArray[i].Data.Key == key) {
				return AddressOf(DynamicArray[i].Data.Value);
			}
		}

		return nullptr;
	}

	bool Remove(const TKey& key) {
		int iFind = -1;
		for (int i = 0; i < Size; i++) {
			if (DynamicArray[i].Data.Key == key) {
				Memory::PlacementDelete(DynamicArray[i]);
				iFind = i;
				break;
			}
		}

		if (iFind == -1) {
			return false;
		}

		for (int i = iFind; i < Size - 1; i++) {
			DynamicArray[i] = Move(DynamicArray[i + 1]);
		}

		--Size;
		return true;
	}

	bool IsFull() {
		return Size == Capacity;
	}

	bool IsEmpty() {
		return Size == 0;
	}

	bool IsValid() {
		return DynamicArray != nullptr;
	}


	TBucketNode& GetAt(const int idx) {
		DebugAssert(DynamicArray);
		DebugAssert(idx >= 0 && idx < Size);
		return DynamicArray[idx];
	}

	Bucket* Next;
	Bucket* Previous;
	TBucketNode* DynamicArray;
	int Capacity;
	int Size;
};
#pragma endregion

NS_JC_END