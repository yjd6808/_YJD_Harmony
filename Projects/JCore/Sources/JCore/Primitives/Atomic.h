/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 3:54:27 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/TypeTraits.h>

namespace JCore {

    template <typename T, Int8 BitSize = sizeof(T) <= 4 ? 4 : 8>
    struct Atomic
    {
        static_assert(IsPrimitiveType_v<T>, "T must be fundamental data type");
    };

    template <typename T>
    struct Atomic<T, 4>
    {
    public:
        void Add(T value)
        {
            
        }

        T Value;
    };

    template <typename T>
    struct Atomic<T, 8>
    {
        T Value;
    };


} // namespace JCore

