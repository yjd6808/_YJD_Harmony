/*
 * 작성자: 윤정도
 * 생성일: 8/15/2023 3:18:37 AM
 * =====================
 * HashTable에서 사용할 노드와 버킷
 */

#pragma once

#include <jc/Comparator.h>
#include <jc/Hasher.h>
#include <jc/Memory.h>
#include <jc/TypeTraits.h>

#include <jc/Allocator/DefaultAllocator.h>
#include <jc/Container/Pair.h>
#include <jc/Pattern/NonCopyableh.h>

NS_JC_BEGIN

enum class HashTableType
{
    Set,
    Map
};

template <typename T>
struct BucketNode
{
    using TThis = BucketNode<T>;

    BucketNode(const T& _data, Int32U _hash) : hash_(_hash)
    {
		new (&data_) T(_data);
    }
    BucketNode(T&& _data, Int32U _hash) : hash_(_hash)
    {
        // 호출됨: HashTable:Insert()
		new (&data_) T(Move(_data));
    } 
    BucketNode(const TThis& _other) : hash_(_other.hash_)
    {
        // 호출됨: HashTable:operator=(const THashTable& other)
		new (&data_) T(_other.data_);
    } 
    BucketNode(TThis&& _other) noexcept : hash_(_other.hash_)
    {
        // 호출됨: HashTable:Expand
		new (&data_) T(Move(_other.data_));
    } 
    ~BucketNode()
    {
		data_.~T();
    }

    TThis& operator=(const TThis& _other) = delete;
    TThis& operator=(TThis&& _other) noexcept
    {
		if (this != &_other) 
		{
			data_.~T();
			new (&data_) T(Move(_other.data_));
			hash_ = _other.hash_;
		}
        return *this;
    }

	union
    {
		T data_;
    };
    Int32U hash_; // 처음에 한번 계산해놓으면 성능이 좀더 개선될 듯?
};

template <typename TKey, typename TValue>
struct BucketNode<Pair<TKey, TValue>>
{
    using TPair = Pair<TKey, TValue>;
    using TThis = BucketNode<Pair<TKey, TValue>>;

    BucketNode(const TPair& _data, Int32U _hash) : hash_(_hash)
    {
		new (&data_) TPair(_data);
    }
    BucketNode(TPair&& _data, Int32U _hash) : hash_(_hash)
    {
        // 호출됨: HashTable:Insert()
		new (&data_) TPair(Move(_data));
    } 
    BucketNode(const TThis& _other) : hash_(_other.hash_)
    {
        // 호출됨: HashTable:operator=(const THashTable& other)
		new (&data_) TPair(_other.data_);
    } 
    BucketNode(TThis&& _other) noexcept : hash_(_other.hash_)
    {
        // 호출됨: HashTable:Expand
		new (&data_) TPair(Move(_other.data_));
    } 
    ~BucketNode()
    {
		data_.~TPair();
    }

    TThis& operator=(const TThis& _other) = delete;
    TThis& operator=(TThis&& _other) noexcept
    {
		if (this != &_other)
		{
			data_.~TPair();
			new (&data_) TPair(Move(_other.data_));
			hash_ = _other.hash_;
		}
        return *this;
    }

	union
    {
		Pair<TKey, TValue> data_;
    };
    Int32U hash_;
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
	: pNext_(nullptr)
	, pPrevious_(nullptr)
	, pDynamicArray_(nullptr)
	, capacity_(1)
	, size_(0)
    {
    }
    ~Bucket()
    {
        JCORE_PLACEMENT_DELETE_ARRAY_SAFE(pDynamicArray_, size_);
        JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(pDynamicArray_, sizeof(TBucketNode) * capacity_);
    }

    template <typename... Args>
    void EmplaceBack(Args&&... _args)
    {
        if(!IsValid())
            Initialize();

        if(IsFull())
            Expand(capacity_ + 2);

        Memory::PlacementNew(pDynamicArray_[size_++], Forward<Args>(_args)...);
    }

    void PushBack(const TBucketNode& _data)
    {
        if(!IsValid())
            Initialize();

        if(IsFull())
            Expand(capacity_ + 2);

        Memory::PlacementNew(pDynamicArray_[size_++], _data);
    }

    void PushBack(TBucketNode&& _data)
    {
        if(!IsValid())
            Initialize();

        if(IsFull())
            Expand(capacity_ + 2);

        Memory::PlacementNew(pDynamicArray_[size_++], Move(_data));
    }

    void Initialize()
    {
        int iAllocated;
        pDynamicArray_ = TAllocator::template AllocateDynamic<TBucketNode*>(sizeof(TBucketNode) * capacity_, iAllocated);
        // Memory::Set(DynamicArray, sizeof(TBucketNode) * Capacity, 0);
    }

    void Expand(int _newCapacity)
    {
        int iAllocated;
        TBucketNode* pNewDynamicArray = TAllocator::template AllocateDynamic<TBucketNode*>(sizeof(TBucketNode) * _newCapacity, iAllocated);

        if constexpr(!IsFundamentalType_v<TKey> && !IsPointerType_v<TKey>)
        {
            if constexpr(IsStringType_v<TKey>)
                Memory::PlacementNewArray(pNewDynamicArray, _newCapacity, TBucketNode{{0}, 0}); // 문자열은 동적할당 안된 상태로 생성해주자. String(0)는 동적할당안함
            else
                jc_assert(false); // String도 아니고 int같은 기본 타입도 아닌 새로운 키타입을 추가하고자 한다면 성능향상을 위해 여기서 직접 수정 ㄱ, 그냥 Memory::PlacementNewArray(pNewDynamicArray,
                                    // newCapacity)를 수행해도 동작하는데 문제없긴함
        }

        for(int i = 0; i < size_; i++)
        {
            pNewDynamicArray[i] = Move(pDynamicArray_[i]); // 2023/02/23 Move 필수.. Value가 포인터라지만 Key가 String 같은 타입일 수가 있다. 만약 Move를 안하고 Copy를 한다면 DynamicArray의 메모리를
                                                           // 해제하기전에 수동으로 소멸자 호출을 해줘야함.
        }

        TAllocator::DeallocateDynamic(pDynamicArray_, sizeof(TBucketNode) * capacity_);
        pDynamicArray_ = pNewDynamicArray;
        capacity_ = _newCapacity;
    }

    void Clear()
    {
        JCORE_PLACEMENT_DELETE_ARRAY_SAFE(pDynamicArray_, size_);
        size_ = 0;
    }

    template <typename Ky>
    bool Exist(const Ky& _key)
    {
        for(int i = 0; i < size_; i++)
        {
            if(Comparator<TKey>()(pDynamicArray_[i].data_, _key) == 0)
            {
                return true;
            }
        }

        return false;
    }

    template <typename Ky>
    TKey* Find(const Ky& _key)
    {
        for(int i = 0; i < size_; i++)
        {
            if(Comparator<TKey>()(pDynamicArray_[i].data_, _key) == 0)
            {
                return AddressOf(pDynamicArray_[i].data_);
            }
        }

        return nullptr;
    }

    template <typename Ky>
    bool Remove(const Ky& _key)
    {
        int findPos = -1;
        for(int i = 0; i < size_; i++)
        {
            if(Comparator<TKey>()(pDynamicArray_[i].data_, _key) == 0)
            {
                Memory::PlacementDelete(pDynamicArray_[i]);
                findPos = i;
                break;
            }
        }

        if(findPos == -1)
        {
            return false;
        }

        for(int i = findPos; i < size_ - 1; i++)
        {
            pDynamicArray_[i] = Move(pDynamicArray_[i + 1]);
        }

        --size_;
        return true;
    }

    bool IsFull()
    {
        return size_ == capacity_;
    }

    bool IsEmpty()
    {
        return size_ == 0;
    }

    bool IsValid()
    {
        return pDynamicArray_ != nullptr;
    }

    TBucketNode& GetAt(const int _idx)
    {
        jc_assert(pDynamicArray_);
        jc_assert(_idx >= 0 && _idx < size_);
        return pDynamicArray_[_idx];
    }

    Bucket* pNext_;
    Bucket* pPrevious_;
    TBucketNode* pDynamicArray_;
    int capacity_;
    int size_;
};

#pragma endregion

#pragma region Bucket<TKey, TValue, TAllocator>
template <typename TKey, typename TValue, typename TAllocator>
struct Bucket<TKey, TValue, TAllocator>
{
    using TBucket = Bucket<TKey, TValue, TAllocator>;
    using TBucketNode = BucketNode<Pair<TKey, TValue>>;

    Bucket()
	: pNext_(nullptr)
	, pPrevious_(nullptr)
	, pDynamicArray_(nullptr)
	, capacity_(1)
	, size_(0)
    {
    }

    ~Bucket()
    {
        JCORE_PLACEMENT_DELETE_ARRAY_SAFE(pDynamicArray_, size_);
        JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(pDynamicArray_, sizeof(TBucketNode) * capacity_);
    }

    template <typename... Args>
    void EmplaceBack(Args&&... _args)
    {
        if(!IsValid())
            Initialize();

        if(IsFull())
            Expand(capacity_ + 2);

        Memory::PlacementNew(pDynamicArray_[size_++], Forward<Args>(_args)...);
    }

    void PushBack(const TBucketNode& _data)
    {
        if(!IsValid())
            Initialize();

        if(IsFull())
            Expand(capacity_ + 2);

        Memory::PlacementNew(pDynamicArray_[size_++], _data);
    }

    void PushBack(TBucketNode&& _data)
    {
        if(!IsValid())
            Initialize();

        if(IsFull())
            Expand(capacity_ + 2);

        Memory::PlacementNew(pDynamicArray_[size_++], Move(_data));
    }

    void Initialize()
    {
        int iAllocated;
        pDynamicArray_ = TAllocator::template AllocateDynamic<TBucketNode*>(sizeof(TBucketNode) * capacity_, iAllocated);
        // Memory::Set(DynamicArray, sizeof(TBucketNode) * Capacity, 0);
    }

    void Expand(int _newCapacity)
    {
        int iAllocated;
        TBucketNode* pNewDynamicArray = TAllocator::template AllocateDynamic<TBucketNode*>(sizeof(TBucketNode) * _newCapacity, iAllocated);

        // 2023/02/23
        // 키가 String 같은 타입인 경우 그냥 대입 해버리면 Key의 생성자로 초기화가 수행이 안되어있기때문에 오류가 발생한다.
        // String의 경우 m_pBuffer가 nullptr로 초기화가 되지 않음
        // 기본 생성자를 호출해서 초기화를 해놔야한다.
        if constexpr(IsPointerType_v<TValue> && !IsFundamentalType_v<TKey> && !IsPointerType_v<TKey>)
        {
            if constexpr(IsStringType_v<TKey>)
                Memory::PlacementNewArray(pNewDynamicArray, _newCapacity, TBucketNode{{0, nullptr}, 0}); // 문자열은 동적할당 안된 상태로 생성해주자. String(0)는 동적할당안함
            else
                jc_assert(false); // String도 아니고 int같은 기본 타입도 아닌 새로운 키타입을 추가하고자 한다면 성능향상을 위해 여기서 직접 수정 ㄱ, 그냥 Memory::PlacementNewArray(pNewDynamicArray,
                                    // newCapacity)를 수행해도 동작하는데 문제없긴함
        }

        for(int i = 0; i < size_; i++)
        {
            if constexpr(IsPointerType_v<TValue>)
                pNewDynamicArray[i] = Move(pDynamicArray_[i]); // 2023/02/23 Move 필수.. Value가 포인터라지만 Key가 String 같은 타입일 수가 있다. 만약 Move를 안하고 Copy를 한다면 DynamicArray의
                                                               // 메모리를 해제하기전에 수동으로 소멸자 호출을 해줘야함.
            else
                Memory::PlacementNew(pNewDynamicArray[i], Move(pDynamicArray_[i]));
        }

        TAllocator::DeallocateDynamic(pDynamicArray_, sizeof(TBucketNode) * capacity_);
        pDynamicArray_ = pNewDynamicArray;
        capacity_ = _newCapacity;
    }

    void Clear()
    {
        JCORE_PLACEMENT_DELETE_ARRAY_SAFE(pDynamicArray_, size_);
        size_ = 0;
    }

    template <typename Ky>
    bool Exist(const Ky& _key)
    {
        for(int i = 0; i < size_; i++)
        {
            if(Comparator<TKey>()(pDynamicArray_[i].data_.key_, _key) == 0)
            {
                return true;
            }
        }

        return false;
    }

    template <typename Ky>
    TValue* Find(const Ky& _key)
    {
        for(int i = 0; i < size_; i++)
        {
            if(Comparator<TKey>()(pDynamicArray_[i].data_.key_, _key) == 0)
            {
                return AddressOf(pDynamicArray_[i].data_.value_);
            }
        }

        return nullptr;
    }

    template <typename Ky>
    bool Remove(const Ky& _key)
    {
        int findPos = -1;
        for(int i = 0; i < size_; i++)
        {
            if(Comparator<TKey>()(pDynamicArray_[i].data_.key_, _key) == 0)
            {
                Memory::PlacementDelete(pDynamicArray_[i]);
                findPos = i;
                break;
            }
        }

        if(findPos == -1)
        {
            return false;
        }

        for(int i = findPos; i < size_ - 1; i++)
        {
            pDynamicArray_[i] = Move(pDynamicArray_[i + 1]);
        }

        --size_;
        return true;
    }

    bool IsFull()
    {
        return size_ == capacity_;
    }

    bool IsEmpty()
    {
        return size_ == 0;
    }

    bool IsValid()
    {
        return pDynamicArray_ != nullptr;
    }

    TBucketNode& GetAt(const int _idx)
    {
        jc_assert(pDynamicArray_);
        jc_assert(_idx >= 0 && _idx < size_);
        return pDynamicArray_[_idx];
    }

    Bucket* pNext_;
    Bucket* pPrevious_;
    TBucketNode* pDynamicArray_;
    int capacity_;
    int size_;
};
#pragma endregion

NS_JC_END