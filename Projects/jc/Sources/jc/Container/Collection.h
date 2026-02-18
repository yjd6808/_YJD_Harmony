/*
 * 작성자 : 윤정도
 */

#pragma once

#include <jc/Container/Iterable.h>
#include <jc/Container/Iterator.h>
#include <jc/Container/CollectionExtension.h>
#include <jc/Container/CollectionType.h>
#include <jc/Container/ContainerType.h>

#pragma warning(push)
  #pragma warning(disable: 26495) // Member variable is uninitialized

NS_JC_BEGIN

/*=====================================================================================
                                    콜렉션
                            모든 컨테이너의 기본 인터페이스
                             
                                     자식
                              1. CListCollection<T>
                              2. CArrayCollection<T>
                              3. MapCollection<T>
                
=====================================================================================*/

template <typename T, typename TAllocator>
class Collection : public Iterable<T, TAllocator>
{
    using TCollectionExtension   = CollectionExtension<T, TAllocator>;
    using TIterable              = Iterable<T, TAllocator>;
    using TCollection            = Collection<T, TAllocator>;
    using TEnumerator            = Enumerator<T, TAllocator>;

public:
    Collection()
        : TIterable()
        , owner_(this, true)
    {
    }

    ~Collection() noexcept override = default;

protected:
    CVoidOwner& GetOwner() const
    {
        return const_cast<CVoidOwner&>(owner_);
    }

public:
    /// <summary>
    /// 가상함수화할 생각 없었는데..
    /// [오버라이딩]
    ///  - HashMapKeyCollection
    ///  - HashMapValueCollection
    /// </summary>
    virtual bool IsEmpty() const = 0;
    virtual int Size() const = 0;
    virtual ContainerType GetContainerType() = 0;
    virtual CollectionType GetCollectionType() = 0;

    TCollectionExtension Extension()
    {
        return TCollectionExtension(this);
    }

protected:
    CVoidOwner owner_;
};

NS_END

#pragma warning(pop)
