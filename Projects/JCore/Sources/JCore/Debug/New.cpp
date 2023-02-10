/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 12:20:12 AM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Memory.h>
#include <JCore/FileSystem/Path.h>

#include <stacktrace>
#include <crtdbg.h>

USING_NS_JC;
USING_NS_STD;



/* 

  폐기물... 3시간동안 뻘짓했다.

template <class T, int BlockType = _NORMAL_BLOCK>
struct malloc_allocator {
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T               value_type;

    template <class U> struct rebind { typedef malloc_allocator<U> other; };
    malloc_allocator() throw() {}
    malloc_allocator(const malloc_allocator&) throw() {}
    template <class U> malloc_allocator(const malloc_allocator<U>&) throw() {}
    ~malloc_allocator() throw() {}


    pointer allocate(size_type s, void const* = 0) {
        // 블록타입에 변경가능토록
        return (pointer)_malloc_dbg(s * sizeof(T), BlockType, nullptr, 0);;
    }

    void deallocate(pointer p, size_type) {

        if constexpr (BlockType != _IGNORE_BLOCK)
            free(p);
    }

    template <typename... args>
    void construct(pointer p, args&&... argss) {
        Memory::PlacementNew(p, Forward<args>(argss)...);
    }

    void destroy(pointer p) {
        Memory::PlacementDelete(p);
    }
};


// 1. 이 2녀석은 예외적으로 메모리릭에 검사되지 않도록 무시블록 타입으로 정한다.
// 2. 매번 맬록시 문자열을 생성해버리면 메모리릭이 엄청 커지기때문에 파일들 수만큼으로 제한하기 위함이다.

using ignore_malloc_string = std::basic_string<char, std::char_traits<char>, malloc_allocator<char, _IGNORE_BLOCK>>;
using ignore_umap = std::unordered_map<char*, char*, std::hash<char*>, std::equal_to<char*>, malloc_allocator<std::pair<char*, char*>>> ;

Size_t __stdcall malloc_string_fill(
    const Size_t sz, void* str, void* context, const _Stacktrace_string_fill_callback callback) {
    if (callback) {
        static_cast<ignore_malloc_string*>(str)->resize_and_overwrite(sz,
            [callback, context](char* data, Size_t size) noexcept { return callback(data, size, context); });
        return static_cast<ignore_malloc_string*>(str)->size();
    }
    else {
        return static_cast<ignore_malloc_string*>(str)->capacity();
    }
}


* operater new내부에서 new 호출없이 동적할당해야함.
*
* 실패 1
* 흠.. _malloc_dbg(size, _NORMAL_BLOCK, (++std::stacktrace::current().begin())->source_file().c_str(), __LINE__)
* std::string 동적할당해서 오류난다.
* 다른 방법..
* ==================================================================================
* 실패2 std::stacktrace가 vector기반이네..
* void* pParentAddress = (++std::stacktrace::current().begin())->native_handle();
* char szSourceFile[256];
* __std_stacktrace_address_to_string(pParentAddress, szSourceFile, nullptr);
* ===================================================================================
* 실패3 커스텀 얼로케이터 정의
* std::string을 호출해서 안됨
* 실패2와 실패3을 조합하면 될듯
* 실패... 너무 느림 모든 동적할당시마다 스택트레이스 만드니까 속도가 답이없다.
* 
*
using malloc_stacktrace = std::basic_stacktrace<malloc_allocator<std::stacktrace_entry>>;
using malloc_string = std::basic_string<char, std::char_traits<char>, malloc_allocator<char>>;
MemHeader* header = nullptr;
{
    void* pParentAddress = (++malloc_stacktrace::current().begin())->native_handle();
    ignore_malloc_string str;
    __std_stacktrace_address_to_string(pParentAddress, &str, malloc_string_fill);


#if DebugMode
    char* mem = (char*)_malloc_dbg(size, _NORMAL_BLOCK, str.c_str(), __LINE__);
    header = (MemHeader*)(mem - sizeof(MemHeader));
}
*/
