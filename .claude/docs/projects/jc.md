# jc (JCore)

## 개요
모든 프로젝트의 기반이 되는 **C++ 범용 코어 Static Library**.  
Windows 플랫폼 전용, Modern C++17/20 기반.  
다른 프로젝트 의존성 없음 (완전 독립).

초기화/종료 필수:
```cpp
InitializeJCore(argc, argv);
// ...
FinalizeJCore();
```

## 주요 모듈

| 모듈 | 경로 | 설명 |
|------|------|------|
| **Type/Define** | `jc/Type.h`, `jc/Define.h` | 커스텀 타입(`_u32`, `_s64`) 및 매크로 |
| **Memory** | `jc/Memory.h/cpp` | 메모리 복사/할당/PlacementNew |
| **Container** | `jc/Container/` | ArrayList, HashMap, HashSet, LinkedList, Queue, Stack 등 |
| **Primitives** | `jc/Primitives/` | String, StringView, SmartPtr, Atomic, BitFlag, Span 등 |
| **Sync** | `jc/Sync/` | NormalLock, SpinLock, RecursiveLock, Semaphore, ConditionVariable, RwLock, Event 등 |
| **Threading** | `jc/Threading/` | Thread, ThreadPool, Scheduler, Pulser, ThreadLocal |
| **Pool** | `jc/Pool/` | ObjectPool, IndexedMemoryPool, MemoryPoolAbstract 등 |
| **FileSystem** | `jc/FileSystem/` | File, Directory, Path, FileStream |
| **Logger** | `jc/Logger/` | ConsoleLogger, LoggerAbstract |
| **Debug** | `jc/Debug/` | MemoryLeakDetector, Debug New/Delete |
| **TypeTraits** | `jc/TypeTraits/` | 컴파일 타임 타입 체크/변환 |
| **Pattern** | `jc/Pattern/` | Singleton, NonCopyable |
| **Utils** | `jc/Utils/` | Console, ConsoleColor, VirtualKey |
| **Wrapper** | `jc/Wrapper/` | WinApi, CRuntime 래퍼 |

## 핵심 클래스

| 클래스 | 파일 | 역할 |
|--------|------|------|
| `String` | `Primitives/String.h` | 동적 문자열 |
| `StringView` | `Primitives/StringView.h` | 비소유 문자열 뷰 |
| `HashMap<K,V>` | `Container/HashMap.h` | 해시 테이블 |
| `Thread` | `Threading/Thread.h` | 스레드 래퍼 |
| `ThreadPool` | `Threading/ThreadPool.h` | 스레드 풀 |
| `Scheduler` | `Threading/Scheduler.h` | 태스크 스케줄러 |
| `NormalLock` | `Sync/NormalLock.h` | 뮤텍스 |
| `SpinLock` | `Sync/SpinLock.h` | 스핀 락 |
| `SharedPtr<T>` | `Primitives/SmartPtr.h` | 공유 포인터 |
| `RefCountObject` | `Primitives/RefCountObject.h` | 참조 카운팅 기반 클래스 |
| `ObjectPool<T>` | `Pool/ObjectPool.h` | 객체 풀 |
| `IndexedMemoryPool` | `Pool/IndexedMemoryPool.h` | 인덱스 기반 메모리 풀 |
| `ConsoleLogger` | `Logger/ConsoleLogger.h` | 콘솔 로거 |
| `FileStream` | `FileSystem/FileStream.h` | 파일 스트림 |
| `Singleton<T>` | `Pattern/Singleton.h` | 싱글턴 패턴 |

## 의존성
- Windows API (Win32)
- C++ 표준 라이브러리 (STL)
- 외부 프로젝트 참조 없음

## 빌드 산출물
- `jc.lib` (Static Library)
- 구성: Debug/Release × Win32/x64
