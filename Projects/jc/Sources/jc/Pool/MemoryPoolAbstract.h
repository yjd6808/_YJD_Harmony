/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 4:01:33 PM
 * =====================
 * 모든 메모라풀의 공통 기능!
 */

#pragma once

#include <jc/Primitives/SmartPtr.h>
#include <jc/Container/HashMap.h>

#include <jc/Pool/MemoryPoolStatistics.h>
#include <jc/Pool/MemoryPoolCaptured.h>

NS_JC_BEGIN

class MemoryPoolAbstract
{
public:
    MemoryPoolAbstract()
    : name_(0)
    , initialized_(false)
    {
    }

    MemoryPoolAbstract(const String& _name)
    : name_(_name)
    , initialized_(false)
    {
    }

    virtual ~MemoryPoolAbstract() = default;

    virtual void Initialize(const HashMap<int, int>& _allocationMap) = 0;
    virtual void Finalize() = 0;
    virtual void* DynamicPop(int _requestSize) = 0;    // 요청한 바이트 크기
    virtual void* DynamicPop(int _requestSize, int& _realAllocatedSize) = 0;    // 요청한 바이트 크기와 반환된 바이트 크기
    virtual void DynamicPush(void* _pMemory, int _returnSize) = 0;
    virtual int Algorithm() = 0;

    const String& Name()
    {
        return name_;
    }

    bool IsInitialized()
    {
        return initialized_;
    }

#if _DEBUG
    _u64 GetTotalAllocated()
    {
        return statistics_.GetTotalAllocated();
    }

    _u64 GetTotalUsed()
    {
        return statistics_.GetTotalUsed();
    }

    _u64 GetTotalReturned()
    {
        return statistics_.GetTotalReturned();
    }

    _u64 GetInitAllocated()
    {
        return statistics_.GetInitAllocated();
    }

    _u64 GetNewAllocated()
    {
        return statistics_.GetNewAllocated();
    }

    _u64 GetTotalUsing()
    {
        return statistics_.GetTotalUsing();
        /*
        _u64 usingSize = 0;
        for (int i = 0; i < detail::MemoryBlockSizeMapSize_v; ++i)
        {
            usingSize = GetBlockUsingCounter(i) * detail::AllocationLengthMapConverter::ToSize(i);
        }
        return usingSize;
        */
    }

    int GetBlockTotalCounter(int _blockIndex)
    {
        jc_assert_msg(_blockIndex >= 0 && _blockIndex <= detail::MemoryBlockSizeMapSize_v, "유효한 범위의 블록인덱스가 아닙니다.");
        return statistics_.blockTotalCounter_[_blockIndex];
    }

    int GetBlockUsedCounter(int _blockIndex)
    {
        jc_assert_msg(_blockIndex >= 0 && _blockIndex <= detail::MemoryBlockSizeMapSize_v, "유효한 범위의 블록인덱스가 아닙니다.");
        return statistics_.blockUsedCounter_[_blockIndex];
    }

    int GetBlockNewAllocCounter(int _blockIndex)
    {
        jc_assert_msg(_blockIndex >= 0 && _blockIndex <= detail::MemoryBlockSizeMapSize_v, "유효한 범위의 블록인덱스가 아닙니다.");
        return statistics_.blockNewAllocCounter_[_blockIndex];
    }

    int GetBlockUsingCounter(int _blockIndex)
    {
        jc_assert_msg(_blockIndex >= 0 && _blockIndex <= detail::MemoryBlockSizeMapSize_v, "유효한 범위의 블록인덱스가 아닙니다.");
        return statistics_.blockUsingCounter_[_blockIndex];
    }

    void ResetStatistics()
    {
        statistics_.Reset();
    }

    bool HasUsingBlock()
    {
        return statistics_.HasUsingBlock();
    }

    void StartDetectLeak()
    {
        CancelDetectLeak();
        detecting_ = true;
    }

    template <bool KeepDetectingState = true>    // 릭 디텍팅 상태를 복구할지
    _u64 StopDetectLeak(OUT_OPT int* _pDetail = nullptr)
    {
        jc_assert_msg(Detecting(), "어라? StartDetectLeak()이 호출되지 않았어요.");
        _u64 leakedBytes = 0;

        for (int i = 0; i < detail::MemoryBlockSizeMapSize_v; ++i)
        {
            int leakedBlockCount = pBlockUsedCounter_[i] - pBlockReturnedCounter_[i];
            leakedBytes += static_cast<_u64>(leakedBlockCount) * detail::AllocationLengthMapConverter::ToSize(i);
            if (_pDetail)
                _pDetail[i] = leakedBlockCount;

            if (i == 0)
                Console::WriteLine("┌ [릭 탐지 결과]");
            if (leakedBlockCount > 0)
                Console::WriteLine("│ [%8d]: %d", detail::AllocationLengthMapConverter::ToSize(i), leakedBlockCount);
            if (i == detail::MemoryBlockSizeMapSize_v - 1)
                Console::WriteLine("└──────────────── %llu바이트 릭 <", leakedBytes);
        }

        if constexpr (!KeepDetectingState)
            detecting_ = false;

        return leakedBytes;
    }

    void CancelDetectLeak()
    {
        detecting_ = false;
        Memory::Copy(pBlockUsedCounter_, detail::MemoryBlockSizeMapSize_v, pBlockUsedCounter_, 0);
        Memory::Copy(pBlockReturnedCounter_, detail::MemoryBlockSizeMapSize_v, pBlockReturnedCounter_, 0);
    }

    bool Detecting()
    {
        return detecting_;
    }

protected:
    void AddInitBlock(_s32 _blockIndex, _s32 _blockCount)
    {
        statistics_.AddInitBlock(_blockIndex, _blockCount);
    }

    void AddAllocated(_s32 _blockIndex, bool _createNew)
    {
        statistics_.AddAllocated(_blockIndex, _createNew);
        if (detecting_)
            ++pBlockUsedCounter_[_blockIndex];
    }

    void AddDeallocated(_s32 _blockIndex)
    {
        statistics_.AddDeallocated(_blockIndex);
        if (detecting_)
            ++pBlockReturnedCounter_[_blockIndex];
    }

    AtomicBool detecting_{};
    CMemoryPoolStatistics statistics_;

    // TODO: 멀티 쓰레딩 디텍션을 수행할려면 쓰레드로컬처럼 동작해야함.
    // 근데 멤버변수로는 쓰레드로컬로 둘 수 없으므로 다른 방식으로 구현해야함.
    // 이거 해결안하면 멀티쓰레드기반환경에서 메모리릭 제대로 못잡아냄;
    // 일단 싱글쓰레드로 가정하고 릭 감지기능 자체만 구현먼저 해놓자.
    AtomicInt pBlockUsedCounter_[detail::MemoryBlockSizeMapSize_v]{};         // 블록 종류별로 몇번 사용되었는지
    AtomicInt pBlockReturnedCounter_[detail::MemoryBlockSizeMapSize_v]{};     // 블록 종류별로 몇번 반환되었는지
#else
public:
    _u64 GetTotalAllocated()
    {
        return 0;
    }

    _u64 GetTotalUsed()
    {
        return 0;
    }

    _u64 GetTotalReturned()
    {
        return 0;
    }

    _u64 GetInitAllocated()
    {
        return 0;
    }

    _u64 GetNewAllocated()
    {
        return 0;
    }

    _u64 GetTotalUsing()
    {
        return 0;
    }

    int GetBlockTotalCounter(int _blockIndex)
    {
        return 0;
    }

    int GetBlockUsedCounter(int _blockIndex)
    {
        return 0;
    }

    int GetBlockNewAllocCounter(int _blockIndex)
    {
        return 0;
    }

    int GetBlockUsingCounter(int _blockIndex)
    {
        return 0;
    }

    void ResetStatistics()
    {
    }

    bool HasUsingBlock()
    {
        return false;
    }

    void StartDetectLeak()
    {
    }

    template <bool KeepDetectingState = true>
    _u64 StopDetectLeak(OUT_OPT int* _pDetail = nullptr)
    {
        return 0;
    }

    bool Detecting()
    {
        return false;
    }

    void CancelDetectLeak()
    {
    }

protected:
    void AddInitBlock(_s32 _blockIndex, _s32 _blockCount)
    {
    }

    void AddAllocated(_s32 _blockIndex, bool _createNew)
    {
    }

    void AddDeallocated(_s32 _blockIndex)
    {
    }
#endif

protected:
    String name_;
    bool initialized_;

    friend class CMemoryPoolManager;
};

using MemoryPoolAbstractPtr = SharedPtr<MemoryPoolAbstract>;

NS_END
