# ObjectPoolTLSIndexed\<T\> 설계 및 개발 지시서
### 스레드-로컬 인덱스 풀(S8 정책) 공용화 + `StringImpl_SSOCtxTlsPool` 적용

- 작성일: 2026-09-06
- 대상 프로젝트: `jc` (`Projects/jc/Sources/jc/`), 테스트 `jc_gtest`, 벤치 `jc_gbench`
- 선행 문서
  - `2026-09-06-String-BasicString-재설계.md` (BasicString\<CharT, Storage\> 구조)
  - `2026-09-06-BasicString-재설계-검토.md` (검토 결과: `Adopt` 추가, `Null` 폐기, `Capacity/Cap` 매핑 등)
  - `2026-09-06-S4-CrossThread-안전화와-String-아키텍처-방향.md` (S4→S8 정책 배경)
- 이 문서의 독자: (1) 구현을 맡을 개발자/AI, (2) 설계 검토자. **§11 파일 목록과 §12 개발 순서만 따라가도 구현이 가능하도록** 작성했다. 왜 그렇게 하는지는 §5~§10에 있다.

> 코드 블록의 들여쓰기는 jc 컨벤션대로 **탭**이다. 이 문서의 코드는 설계 스케치이며, 컴파일을 통과시키는 것은 구현 단계의 일이다. 단, 자료구조·불변 조건·연산 순서는 **그대로 지켜야 한다**. 바꿔야 한다면 §16 결정 기록에 사유를 남긴다.

---

## 0. 결정 요약

| # | 결정 | 이유 |
|---|---|---|
| D1 | S4의 스레드-로컬 슬롯 풀을 String 전용으로 두지 않고 **`ObjectPoolTLSIndexed<T>` 공용 풀**로 `jc/Pool/`에 만든다. 첫 사용자는 `StringImpl_SSOCtxTlsPool`. | S8 정책(스레드별 프리리스트 + 소유자 번호 + lock-free inbox)은 String과 무관한 일반 기법이다(mimalloc/tcmalloc의 thread cache + remote free와 같은 구조). 나중에 `CMessage` 등에도 쓸 수 있다. |
| D2 | 기존 `ObjectPool<T>`(연결리스트 + 전역 SpinLock)는 **손대지 않고** 별도 클래스로 추가한다. | 정책이 다르다. `virtual ~ObjectPool()`, 상시 `pNext_`, 전역 락은 64B 슬롯·핫 패스와 충돌한다. |
| D3 | 풀 객체는 `TlsPoolNode`(`_u32 poolIdx_`, 4B)를 **첫 베이스**로 상속한다. 프리 상태에서만 객체의 **offset 8~15**를 링크로 재사용한다. | 살아있는 객체가 링크 8B를 들고 다니지 않는다. `poolIdx_`(offset 0)는 프리 상태에서도 보존된다. |
| D4 | 풀 테이블은 **하이브리드**: 앞 256개 고정 배열 + 넘치면 세그먼트(64개 × 최대 1,024). | 고정 상한은 스레드 수 초과 시 오류. 세그먼트만 쓰면 조회 2단. 하이브리드는 99% 경로 로드 1회 + 상한 오류 없음. |
| D5 | 캐시라인 크기는 `jc/Define.h`의 **`JC_CACHE_ALIGN_SIZE 64`** 한 곳에서 관리. `Aligner.h`의 `alignas(64)`도 교체. SSO 용량은 이 값에서 **파생**. | 하드코딩 64가 흩어져 있다. |
| D6 | owner 스레드 종료: `alive_=0` → 프리리스트 힙 반납 → inbox drain **2회** → 고아 목록 등록. 죽은 owner의 객체를 남이 반납하면 **즉시 힙으로**. 고아 inbox는 `SweepOrphans()`가 회수. | "고아 inbox 영구 잔류" 문제의 해결책. |
| D7 | 고아 목록은 **SpinLock + 단일 연결리스트**(콜드 패스). 핫 패스에는 락이 없다. | 스레드 생성/종료 때만 쓴다. lock-free 인덱스 스택은 ABA 처리가 필요해 복잡도가 이득보다 크다. |
| D8 | `Pool`과 `Segment`는 **한 번 만들면 해제하지 않는다**(주소 불변). 번호는 재사용한다. | 다른 스레드가 락 없이 `PoolAt(idx)`를 읽는 중이라 옮기거나 지울 수 없다. 개수는 "동시에 풀을 쓴 최대 스레드 수"이므로 누적 증가가 아니다. |
| D9 | 원자 변수는 `jc::Atomic<T>`. 단 **`Atomic` 생성자를 `constexpr`로 만드는 선행 작업** 필요. `alive_`는 `Atomic<_u32>`. | 컨벤션 유지. `inline static Atomic<Pool*> FastTable[256]`가 상수 초기화되어야 정적 초기화 순서 문제가 없다. `Interlocked`의 8비트 지원이 불확실해 bool 대신 `_u32`. |
| D10 | 고아 목록 락은 **함수-로컬 static**으로 얻는다(`GetOrphanLock()`). | `SpinLock`은 vptr가 있는 비-trivial 타입. 전역 `inline static`이면 정적 초기화 순서에 따라 생성 전 사용 위험. |
| D11 | 1차에서 슬롯은 **개별 힙 할당**(청크 배열 아님). `Memory::Allocate/Deallocate` 사용. | `ObjectPool<T>`·S4 벤치와 동일 조건. 청크는 live count/해제 문제가 있어 2차. |
| D12 | 풀 공개 API는 **`Pop()` / `Push()`**로 명명한다 (`Acquire/Release` 사용 안 함). | 기존 `ObjectPool<T>`의 `PopObject/PushObject`와 일관. LIFO 프리리스트의 스택 의미와 일치. 고아 목록 함수(`PopOrphan/PushOrphan`)와도 같은 동사. |

---

## 1. 한 줄 정의와 비유

**`ObjectPoolTLSIndexed<T>`는 "스레드마다 자기 서랍(프리리스트)을 하나씩 갖고, 모든 객체에 만든 서랍 번호를 도장으로 찍어 두는 풀"이다.**

- 같은 서랍에서 꺼내고 넣는 동안은 락도 원자 연산도 없다 (핫 패스).
- 남의 서랍 물건을 돌려줄 때만 도장 번호로 주인을 찾아 주인 서랍 옆의 **반납함(inbox)**에 원자 연산(CAS) 한 번으로 넣는다.
- 주인은 자기 서랍이 비었을 때 반납함을 **통째로**(Exchange) 비워 서랍에 옮긴다. 개별 pop이 없으므로 ABA 문제가 없다.
- 주인이 퇴사(스레드 종료)하면 서랍 번호에 "퇴사" 표시(`alive_=0`)를 하고 서랍과 반납함을 비운다. 그 뒤 들어오는 반납은 반납함 대신 바로 창고(힙)로 간다. 빈 번호는 다음 입사자가 물려받는다.

기존 `ObjectPool<T>`는 "회사 전체가 서랍 하나를 공유하고 줄을 서서(SpinLock) 꺼내 쓰는" 구조다. 단순하지만 스레드가 늘면 줄이 길어진다.

---

## 2. 기존 `ObjectPool<T>`와의 관계

### 2.1 비교

| 항목 | `ObjectPool<T>` (기존, `Pool/ObjectPool.h`) | `ObjectPoolTLSIndexed<T>` (신규) |
|---|---|---|
| 프리리스트 | 전역 1개 (`inline static T* Head`) | **스레드별 1개** (`tls_pPool->pFree_`) |
| 동기화 | 전역 `SpinLock`, 매 new/delete `JC_LIB_LOCK_GUARD(Lock)` | 같은 스레드: 없음. 다른 스레드 반납: CAS 1회 |
| 소유자 식별 | 없음 | `poolIdx_` → `PoolAt(idx)` O(1) |
| 객체 오버헤드 (살아있을 때) | `T* pNext_` 8B + vptr 8B = 16B | `_u32 poolIdx_` 4B |
| 객체 오버헤드 (프리 상태) | 같음 | offset 8~15 링크 재사용, 추가 0B |
| 사용 API | `new T`/`delete` 오버로드, `PopObject/PushObject`, `MakeShared` | `Pop()/Push()`. `new/delete` 문법은 `TlsPoolNewDeleteMixin<T>` 옵트인 |
| 배열 `new T[]` | 금지 | 금지 |
| 스레드 종료 처리 | 불필요(전역) | `Guard` TLS 소멸자 |
| 통계 | 락 안에서 정확 | 풀별 비원자 카운터, 집계는 콜드 패스 |
| 적합 대상 | 스레드 적고 소유권이 오래가는 객체 (`Property<T>`) | 생성/소멸 잦고 대부분 같은 스레드에서 죽는 작은 객체 (String 슬롯, 메시지) |

### 2.2 `ObjectPool<T>`에서 가져오지 **않는** 것

1. `virtual ~ObjectPool()` — vptr 8B가 붙으면 `StringSlot` SSO가 8B 줄어든다. 신규 풀은 베이스가 비-virtual `TlsPoolNode`이고, 풀 클래스 자체는 상속하지 않는다(static API만).
2. 상시 멤버 `T* pNext_` — 프리 상태에서만 객체 메모리 일부를 링크로 쓴다.
3. 전역 `Head` + `SpinLock` — 스레드별 풀 + inbox.
4. `operator delete`의 `pNext_ != nullptr` 이중 삭제 검사 — 링크 위치가 사용 중 데이터와 겹치므로 불가. 디버그 빌드용 대안은 §16.

### 2.3 이름

`jc/Pool/`에는 이미 `CIndexedMemoryPool`(사이즈 클래스를 **인덱스**로 고르는 메모리풀)이 있다. `ObjectPoolTLSIndexed`의 "Indexed"는 **소유 풀 번호 인덱스**라는 뜻이다. 헤더 주석에 이를 명시한다. 대안 이름(`ThreadLocalObjectPool`, `TlsObjectPool`)은 채택하지 않는다.

---

## 3. 용어

| 용어 | 뜻 |
|---|---|
| 풀 (`Pool`) | 스레드 하나가 소유하는 프리리스트 + 반납함 + 번호. 힙에 1회 할당, 영구. |
| 풀 번호 (`idx_`, `poolIdx_`) | 0부터 증가하는 `_u32`. 테이블 인덱스. 스레드 종료 후 재사용. |
| owner | 그 풀을 `tls_pPool`로 갖고 있는 스레드. 프리리스트를 락 없이 만질 수 있는 유일한 스레드. |
| 반납함 (`inbox_`) | 다른 스레드가 CAS로 push하는 Treiber 스택. owner만 `Exchange(nullptr)`로 통째로 비운다. |
| `alive_` | 1 = owner 존재, 0 = 종료됨(고아). |
| 고아 (orphan) | owner가 종료된 풀. 번호는 고아 목록에 있고 다음 `AttachPool()`이 물려받는다. |
| foreign 반납 | `Push()`를 부른 스레드의 풀 번호 ≠ 객체의 `poolIdx_`. |
| 프리 상태 | `Push()` 이후 ~ 다음 `Pop()` 전. 이때만 offset 8~15가 링크. |
| drain | owner가 inbox를 `Exchange(nullptr)`로 비워 프리리스트에 붙이는 것. |
| sweep | 고아 풀들의 inbox를 비워 힙으로 돌려보내는 것. |

---

## 4. 자료구조

### 4.1 `JC_CACHE_ALIGN_SIZE` (`jc/Define.h`)

```cpp
// jc/Define.h  (기존 JC_INFINITE 근처)
#define JC_CACHE_ALIGN_SIZE		64
```

- `jc/Aligner.h`의 `alignas(64)` 3곳(L13, L22, L40)을 `alignas(JC_CACHE_ALIGN_SIZE)`로 교체.
- `std::hardware_destructive_interference_size`는 컴파일러/플래그에 따라 값이 달라 ABI가 흔들린다. 고정 상수가 낫다.
- 이 문서의 모든 `64`는 이 매크로에서 파생된다.

### 4.2 `TlsPoolNode` — 4B 베이스, 프리 상태 링크 규약

```cpp
// 풀이 관리하는 객체의 공통 베이스. 반드시 첫 베이스로 상속한다 (offset 0).
struct TlsPoolNode
{
	_u32 poolIdx_;		// 이 객체를 힙에서 처음 받아온 풀의 번호. 풀만 쓴다.
};
```

풀은 `Push()`된 객체를 다음 오버레이로 본다.

```cpp
// 풀 내부 전용. T 의 메모리 위에 겹쳐 보는 뷰.
struct FreeNode
{
	_u32		poolIdx_;		// offset 0  : TlsPoolNode::poolIdx_ 와 같은 자리. 프리 상태에서도 보존.
	_u32		reserved_;		// offset 4  : 건드리지 않음 (T 의 데이터 자리)
	FreeNode*	pNext_;			// offset 8  : 프리 상태에서만 유효
};
```

`T`의 요구 조건:

| 조건 | 검사 |
|---|---|
| `TlsPoolNode`를 첫 베이스로 상속 | `static_assert(IsBaseOf_v<TlsPoolNode, T>)` |
| `sizeof(T) >= 16` | `static_assert(sizeof(T) >= sizeof(FreeNode))` |
| `alignof(T) <= alignof(std::max_align_t)` | `static_assert`. 슬롯을 64B **정렬**할 필요는 없다(64B **크기**만). S4 벤치와 동일 조건. |
| 살아있는 동안 offset 0~3을 쓰지 않음 | `TlsPoolNode` 상속으로 자동 |
| 프리 상태에서 offset 8~15가 덮인다는 것을 인지 | 계약. `StringSlot`은 이 자리가 `pHeap_`라 프리 상태에서 의미 없음 |

왜 offset 8인가: offset 4면 8B 포인터가 비정렬. offset 0이면 `poolIdx_`가 덮여 `Pop()`마다 재도장이 필요하고 `jc_assert(pNode->poolIdx_ == idx_)` 검증도 못 한다.

### 4.3 `Pool`

```cpp
struct alignas(JC_CACHE_ALIGN_SIZE) Pool
{
	// ── 라인 0: owner 스레드만 읽고 쓴다 ──────────────────────────────
	FreeNode*	pFree_;				// 프리리스트 헤드 (LIFO)
	_u32		idx_;				// 풀 번호
	_u32		totalCount_;		// 이 풀이 힙에서 받아온 객체 수 (owner 전용)
	_u32		freeCount_;			// pFree_ 길이 (owner 전용)
	_u32		reserved_;
	Pool*		pNextOrphan_;		// 고아 목록 링크 (OrphanLock 아래에서만)

	// ── 라인 1: 다른 스레드가 접근한다 ────────────────────────────────
	alignas(JC_CACHE_ALIGN_SIZE)
	Atomic<FreeNode*>	inbox_;		// foreign 반납함. push: 누구나 CAS, pop: owner 가 Exchange(nullptr)
	Atomic<_u32>		alive_;		// 1 = owner 살아있음, 0 = 고아
};
static_assert(sizeof(Pool) == 2 * JC_CACHE_ALIGN_SIZE, "Pool 은 캐시라인 2개");
```

라인을 둘로 나눈 이유: `pFree_`는 owner가 매 `Pop/Push`마다 쓴다. 같은 라인에 `inbox_/alive_`가 있으면 foreign 스레드가 `alive_`를 읽을 때마다 owner 라인이 Shared로 떨어지고 owner의 다음 쓰기가 RFO를 낸다(false sharing). foreign이 만지는 두 필드를 라인 1에 모아 두면 foreign은 라인 1만 건드린다.

### 4.4 풀 테이블 — 하이브리드

```cpp
static constexpr _u32 kNone				= ~0u;
static constexpr _u32 kFastPoolCount	= 256;							// 고정 배열 구간
static constexpr _u32 kSegmentBits		= 6;
static constexpr _u32 kSegmentSize		= 1u << kSegmentBits;			// 세그먼트당 64 풀
static constexpr _u32 kMaxSegmentCount	= 1024;
static constexpr _u32 kMaxPoolCount		= kFastPoolCount + kSegmentSize * kMaxSegmentCount;	// 65,792

struct Segment
{
	Atomic<Pool*> slots_[kSegmentSize];		// 64 × 8B = 512B
};

inline static Atomic<Pool*>		FastTable[kFastPoolCount];			// 2KB, 상수 초기화(nullptr)
inline static Atomic<Segment*>	SegmentTable[kMaxSegmentCount];		// 8KB, 상수 초기화(nullptr)
inline static Atomic<_u32>		PoolCount;							// 발급한 번호 수 (= 다음 번호)
```

| | 고정 배열만 | 세그먼트만 | 하이브리드 (채택) |
|---|---|---|---|
| 상한 | `kMaxPools` 넘으면 오류 | 65,536 | 65,792 (사실상 무제한. 그 전에 OS 스레드 한도) |
| 초기 메모리 | 상한 × 8B | 8KB + 사용한 세그먼트 512B씩 | 2KB + 8KB + 사용한 세그먼트만 |
| 조회 (foreign 반납만) | 로드 1회 | 로드 2회 | 256 스레드까지 로드 1회, 이후 2회 |
| 주소 안정성 | ○ | ○ | ○ |

조회 비용에 대해: 세그먼트 조회는 x64에서 `shr, mov, and, mov` = 정수 2 + 의존 로드 2 ≈ 1~2ns이고, **foreign 반납에서만** 실행된다. 그 경로는 CAS(수십~백수십 cycle)가 지배하므로 실측 차이는 오차 범위지만, 하이브리드로 99% 경로를 로드 1회로 만든다.

### 4.5 고아 목록

```cpp
inline static Pool*	OrphanHead	= nullptr;		// GetOrphanLock() 아래에서만
inline static _u32	OrphanCount	= 0;

static TLock& GetOrphanLock()						// 함수-로컬 static: 초기화 순서 안전 (D10)
{
	static TLock lock;
	return lock;
}
```

`Pool::pNextOrphan_`로 잇는 단일 연결리스트. push/pop/sweep 모두 락 안에서. 스레드 생성·종료 시에만 사용된다.

### 4.6 TLS

```cpp
struct Guard
{
	_u32 idx_ = kNone;
	~Guard();						// 스레드 종료 시 풀 정리 (§6.9)
};

inline static thread_local Pool*	tls_pPool = nullptr;	// trivial. 스레드 시작 시 0-fill 로 자동 초기화. 핫 패스는 이것만 읽는다.
inline static thread_local Guard	tls_Guard;				// 비-trivial. 첫 참조 시 생성 + 소멸자 등록. AttachPool() 안에서만 참조한다.
```

둘을 나눈 이유: 소멸자가 있는 `thread_local` 객체는 접근마다 "초기화됐나" 가드 검사가 붙는다(MSVC `_Init_thread_*`). `tls_pPool`은 trivial 포인터라 접근이 `mov [TLS+off]` 1회다. `tls_Guard`는 `AttachPool()`(스레드당 1회)에서만 만져 가드 비용을 핫 패스에서 뺀다.

`T`마다, 스레드마다 별도 인스턴스다. `ObjectPoolTLSIndexed<StringSlot<char>>`와 `ObjectPoolTLSIndexed<CMessage>`는 테이블·TLS 모두 독립이다.

### 4.7 불변 조건

| # | 불변 조건 | 지키는 곳 |
|---|---|---|
| I1 | 객체의 `poolIdx_`는 힙 할당 시 1회 찍히고 이후 바뀌지 않는다. | `Pop()` 신규 할당 경로 |
| I2 | 풀 `P`의 `pFree_`에 있는 모든 노드는 `poolIdx_ == P.idx_`. | `Push()` ①, `Pop()` drain (inbox 노드도 이 풀 번호로 찍힌 것만 들어옴) |
| I3 | 풀 `P`의 `inbox_`에 있는 모든 노드는 `poolIdx_ == P.idx_`. | `Push()` ③ (`PoolAt(poolIdx_)`의 inbox에만 push) |
| I4 | `pFree_`는 owner 스레드만 읽고 쓴다. | `tls_pPool` 비교로만 접근 |
| I5 | `inbox_`에서 노드를 꺼내는 것은 `Exchange(nullptr)`만. 개별 pop 없음. | `Pop()`, `~Guard()`, `SweepOrphans()` |
| I6 | `alive_ == 0`인 풀의 `pFree_`는 비어 있다. | `~Guard()`가 비운 뒤 0으로. 순서는 §6.9 |
| I7 | 고아 목록에 있는 풀은 `alive_ == 0`, `tls_pPool`로 갖는 스레드 없음. | `~Guard()` → `PushOrphan()`, `AttachPool()` → `PopOrphan()` 후 `alive_=1` |
| I8 | `PoolAt(idx)`는 `idx < PoolCount`이면 항상 non-null이고 주소가 바뀌지 않는다. | `CreatePool()` 순서: 풀 생성 → 테이블 store → 번호 사용 |
| I9 | 하나의 풀 번호를 동시에 두 스레드가 `tls_pPool`로 갖지 않는다. | 고아 pop은 락 안, 신규 번호는 `PoolCount.Increment()` |

---

## 5. 헤더 전체 구조 (`jc/Pool/ObjectPoolTLSIndexed.h`)

헤더 온리 템플릿. 클래스 선언 아래에 함수 정의를 `//////` 구분자로 나눠 둔다(`String.cpp` 스타일). `ObjectPool.h`는 클래스 안에 인라인으로 썩는데, 이 클래스는 함수가 길어서 밖으로 뻐는 것이 읽기 쉽다.

```cpp
/*
 * 작성자: 윤정도
 * 생성일: 9/6/2026
 * =====================
 * 스레드-로컬 인덱스 기반 오브젝트풀
 *
 * - 스레드번마다 프리리스트(Pool)를 하나씩 갖고, 객체마다 그 객체를 힙에서 받아온 Pool 번호(poolIdx_)를 찍는다.
 * - 같은 스레드 안 Pop/Push 는 락·원자연산 없음.
 * - 다른 스레드가 Push 하면 poolIdx_ 로 주인 Pool 을 찾아 inbox 에 CAS push. 주인은 프리리스트가 비었을 때 inbox 를 통째로 회수.
 * - 주인 스레드가 종료되면 Pool 은 고아(alive_ = 0)가 되고 번호는 다음 스레드가 재사용한다. 고아 Pool 의 객체를 반납하면 즉시 힙으로.
 *
 * "Indexed" 는 소유 Pool 번호 인덱스를 말한다. CIndexedMemoryPool(사이즈 클래스 인덱싱)과는 무관.
 * 기존 ObjectPool<T>(전역 연결리스트 + SpinLock)과는 다른 정책이므로 대체가 아니라 보완이다.
 *
 * [T 의 요구 조건]
 *  - TlsPoolNode 를 첫 베이스로 상속 (offset 0 에 poolIdx_)
 *  - sizeof(T) >= 16, alignof(T) <= alignof(max_align_t)
 *  - Push 된 상태에서는 풀이 offset 8~15 를 링크로 덮어쓴다
 *
 * [사용]
 *  struct Foo : TlsPoolNode { ... };
 *  Foo* p = ObjectPoolTLSIndexed<Foo>::Pop();   // 생성자 호출 안 함 (메모리만)
 *  ObjectPoolTLSIndexed<Foo>::Push(p);           // 소멸자 호출 안 함
 *
 *  struct Bar : TlsPoolNode, TlsPoolNewDeleteMixin<Bar> { ... };
 *  Bar* p = dbg_new Bar(...);  delete p;            // 생성자/소멸자 포함
 *  new Bar[10] 사용 불가
 */

#pragma once

#include <new>
#include <typeinfo>
#include <type_traits>
#include <cstddef>

#include "jc/Define.h"
#include "jc/Macro.h"
#include "jc/Type.h"
#include "jc/Assert.h"
#include "jc/Memory.h"
#include "jc/Debug/New.h"
#include "jc/Primitives/Atomic.h"
#include "jc/Sync/SpinLock.h"
#include "jc/Sync/LockGuard.h"
#include "jc/Logger/LoggerDefine.h"
#include "jc/TypeTraits.h"

NS_JC_BEGIN

struct TlsPoolNode
{
	_u32 poolIdx_;
};

template <typename T>
class ObjectPoolTLSIndexed;

// new/delete 문법이 필요한 T가 함께 상속한다. String 슬롯에는 쓰지 않는다.
// 의도적으로 클래스 바깥 템플릿이다. 중첩으로 두면 파생 클래스 정의 중에
// 풀 본체가 불완전 타입으로 인스턴스화되어 클래스 영역 static_assert에 걸린다 (CRTP).
template <typename T>
struct TlsPoolNewDeleteMixin
{
	using TPool = ObjectPoolTLSIndexed<T>;

	static void* operator new(size_t _size);
	static void* operator new(size_t _size, int _blockUse, char const* _pFileName, int _lineNumber);
	static void operator delete(void* _pObject);
	static void operator delete(void* _pObject, int _blockUse, char const* _pFileName, int _lineNumber);
	static void* operator new[](size_t _size) = delete;
	static void operator delete[](void* _pObject) = delete;
};

template <typename T>
class ObjectPoolTLSIndexed final
{
	static_assert(IsBaseOf_v<TlsPoolNode, T>, "T 는 TlsPoolNode 를 첫 베이스로 상속해야 합니다.");
	static_assert(sizeof(T) >= 16, "T 는 최소 16바이트여야 합니다. (프리 상태 링크가 offset 8~15 를 사용)");
	static_assert(alignof(T) <= alignof(std::max_align_t), "over-aligned T 는 지원하지 않습니다.");

public:
	using TPool = ObjectPoolTLSIndexed<T>;
	using TLock = SpinLock;

	static constexpr _u32 kNone				= ~0u;
	static constexpr _u32 kFastPoolCount	= 256;
	static constexpr _u32 kSegmentBits		= 6;
	static constexpr _u32 kSegmentSize		= 1u << kSegmentBits;
	static constexpr _u32 kMaxSegmentCount	= 1024;
	static constexpr _u32 kMaxPoolCount		= kFastPoolCount + kSegmentSize * kMaxSegmentCount;

	struct Stats
	{
		_u32 poolCount;			// 발급된 풀 번호 수
		_u32 alivePoolCount;	// alive_ == 1 인 풀 수
		_u32 orphanPoolCount;	// 고아 목록 길이
		_u64 totalCount;		// 살아있는 풀들의 totalCount_ 합 (스냅샷, 근사)
		_u64 freeCount;			// 살아있는 풀들의 freeCount_ 합 (inbox 미포함)
	};

	// new/delete 문법은 TlsPoolNewDeleteMixin<T> 상속으로 opt-in 한다 (클래스 바깥 정의).

	ObjectPoolTLSIndexed() = delete;		// static 전용 클래스

	static T* Pop();					// 메모리만 돌려준다. 생성자 호출 안 함. poolIdx_ 는 찍혀 있다.
	static void Push(T* _pObject);		// 소멸자 호출 안 함. 어느 스레드에서 불러도 안전.
	static void SweepOrphans();				// 고아 풀들의 inbox 를 힙으로. 콜드.
	static void TrimThisThread();			// 현 스레드 풀의 pFree_ 를 힙으로 + SweepOrphans. 콜드.
	static Stats GetStats();				// 집계. 콜드. 스냅샷이므로 근사값.
	static _u32 ThisPoolIdx();				// 현 스레드 풀 번호. 없으면 kNone. 테스트용.

private:
	struct FreeNode
	{
		_u32		poolIdx_;
		_u32		reserved_;
		FreeNode*	pNext_;
	};
	static_assert(sizeof(FreeNode) == 16);

	struct alignas(JC_CACHE_ALIGN_SIZE) Pool
	{
		FreeNode*	pFree_;
		_u32		idx_;
		_u32		totalCount_;
		_u32		freeCount_;
		_u32		reserved_;
		Pool*		pNextOrphan_;

		alignas(JC_CACHE_ALIGN_SIZE)
		Atomic<FreeNode*>	inbox_;
		Atomic<_u32>		alive_;
	};
	static_assert(sizeof(Pool) == 2 * JC_CACHE_ALIGN_SIZE);

	struct Segment
	{
		Atomic<Pool*> slots_[kSegmentSize];
	};

	struct Guard
	{
		_u32 idx_ = kNone;
		~Guard();
	};

	static Pool* PoolAt(_u32 _idx);
	static Pool* ThisPool();
	static Pool* AttachPool();
	static Pool* CreatePool();
	static Pool* PopOrphan();
	static void PushOrphan(Pool* _pPool);
	static _u32 FreeChain(FreeNode* _pHead);
	static TLock& GetOrphanLock();

	inline static Atomic<Pool*>		FastTable[kFastPoolCount];
	inline static Atomic<Segment*>	SegmentTable[kMaxSegmentCount];
	inline static Atomic<_u32>		PoolCount;
	inline static Pool*				OrphanHead = nullptr;
	inline static _u32				OrphanCount = 0;

	inline static thread_local Pool*	tls_pPool = nullptr;
	inline static thread_local Guard	tls_Guard;
};

// (이 아래에 §6 의 함수 정의들)

NS_END
```

---

## 6. 연산 정의

핫 패스는 §6.1 `Pop`, §6.2 `Push` 둘이다. 나머지는 스레드 생성·종료 혹은 운영 훅이다.

### 6.1 `Pop()`

```cpp
//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
T* ObjectPoolTLSIndexed<T>::Pop()
{
	Pool* pPool = ThisPool();
	FreeNode* pNode = pPool->pFree_;

	if (pNode == nullptr)
	{
		// 프리리스트가 비었다. 다른 스레드가 돌려준 것이 있으면 통째로 가져온다.
		pNode = pPool->inbox_.Exchange(nullptr);

		if (pNode == nullptr)
		{
			// 그것도 없다. 힙에서 새로 받고 이 풀 번호를 찍는다. (I1: 도장은 이 곳 1회)
			T* pNew = Memory::Allocate<T*>(sizeof(T));
			pNew->poolIdx_ = pPool->idx_;
			++pPool->totalCount_;
			return pNew;
		}

		// drain 한 체인의 길이는 이 자리에서 세지 않는다. (freeCount_ 는 스냅샷용 근사값)
		// 반환하는 헤드 이후 노드들을 pFree_ 로 잘라붙인다.
		pPool->pFree_ = pNode->pNext_;
		jc_assert(pNode->poolIdx_ == pPool->idx_);		// I3
		return reinterpret_cast<T*>(pNode);
	}

	pPool->pFree_ = pNode->pNext_;
	--pPool->freeCount_;
	jc_assert(pNode->poolIdx_ == pPool->idx_);			// I2
	return reinterpret_cast<T*>(pNode);
}
```

핫 패스 번역: TLS 로드 1 (`ThisPool`의 `tls_pPool`), 분기 1 (non-null), 로드 1 (`pFree_`), 분기 1, 로드 1 (`pNext_`), 스토어 1, 감산 1. S4 `PoolNewImpl()`과 같다.

### 6.2 `Push()`

```cpp
//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void ObjectPoolTLSIndexed<T>::Push(T* _pObject)
{
	if (_pObject == nullptr)
	{
		_LogWarn_("반납할려는 %s 객체가 nullptr 입니다.", typeid(T).name());
		return;
	}

	FreeNode* pNode = reinterpret_cast<FreeNode*>(_pObject);
	Pool* pMine = tls_pPool;

	// ① 내 풀 것 — 핫 패스. 락·원자 연산 없음.
	if (pMine != nullptr && pNode->poolIdx_ == pMine->idx_)
	{
		pNode->pNext_ = pMine->pFree_;
		pMine->pFree_ = pNode;
		++pMine->freeCount_;
		return;
	}

	// ② 다른 스레드 것. 주인을 찾는다.
	jc_assert(pNode->poolIdx_ < PoolCount.Load());
	Pool* pOwner = PoolAt(pNode->poolIdx_);

	if (pOwner->alive_.Load() == 0)
	{
		// 주인 스레드가 이미 종료됨. inbox 에 넣으면 아무도 안 꺼내가므로 바로 힙으로.
		Memory::Deallocate(_pObject);
		return;
	}

	// ③ 주인 inbox 에 CAS push (Treiber). 실패하면 _expected 가 최신 헤드로 갱신되어 있다.
	FreeNode* pHead = pOwner->inbox_.Load();
	do
	{
		pNode->pNext_ = pHead;
	}
	while (!pOwner->inbox_.CompareExchange(pHead, pNode));
}
```

`tls_pPool == nullptr`인 스레드(풀을 받은 적이 없고 남이 만든 객체를 지우기만 하는 스레드, 또는 `~Guard()` 이후 정적 소멸 경로)는 ①을 건너뛰고 ②/③로 가므로 풀을 붙이지 않는다. 이것이 의도된 동작이다 — 소보자만 하는 스레드는 테이블을 차지하지 않는다.

### 6.3 `PoolAt()`

```cpp
//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
typename ObjectPoolTLSIndexed<T>::Pool* ObjectPoolTLSIndexed<T>::PoolAt(_u32 _idx)
{
	if (_idx < kFastPoolCount)
	{
		return FastTable[_idx].Load();
	}

	const _u32 overflowIdx = _idx - kFastPoolCount;
	Segment* pSegment = SegmentTable[overflowIdx >> kSegmentBits].Load();
	jc_assert(pSegment != nullptr);
	return pSegment->slots_[overflowIdx & (kSegmentSize - 1)].Load();
}
```

### 6.4 `ThisPool()`

```cpp
//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
typename ObjectPoolTLSIndexed<T>::Pool* ObjectPoolTLSIndexed<T>::ThisPool()
{
	Pool* pPool = tls_pPool;

	if (pPool != nullptr)
	{
		return pPool;
	}

	return AttachPool();		// 스레드당 1회
}
```

### 6.5 `AttachPool()` — 스레드의 첫 `Pop()`

```cpp
//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
typename ObjectPoolTLSIndexed<T>::Pool* ObjectPoolTLSIndexed<T>::AttachPool()
{
	jc_assert(tls_pPool == nullptr);

	SweepOrphans();								// 기회가 된 김에 고아 inbox 정리 (D6)

	Pool* pPool = PopOrphan();					// 반납된 번호가 있으면 재사용
	if (pPool == nullptr)
	{
		pPool = CreatePool();					// 없으면 새 번호
	}

	jc_assert(pPool->pFree_ == nullptr);		// I6
	pPool->alive_.Store(1);						// 이 시점부터 foreign Push 가 inbox 로 온다
	tls_pPool = pPool;
	tls_Guard.idx_ = pPool->idx_;				// tls_Guard 첫 참조: 생성자 실행 + 소멸자 등록
	return pPool;
}
```

순서가 중요하다: `alive_=1` 이전에 들어온 foreign Push는 ②에서 힙으로 가고(고아 처리), 이후는 inbox로 온다. 어느 쪽이든 안전하다. `tls_pPool` 대입은 `alive_=1` 다음 — 같은 스레드이므로 순서 자체는 문제 없으나 읽기 명확성 많이.

### 6.6 `CreatePool()` — 새 번호 발급 + 테이블 등록

```cpp
//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
typename ObjectPoolTLSIndexed<T>::Pool* ObjectPoolTLSIndexed<T>::CreatePool()
{
	const _u32 idx = PoolCount.Increment() - 1;			// Increment 는 증가 후 값을 반환
	jc_assert_msg(idx < kMaxPoolCount, "ObjectPoolTLSIndexed: 풀 번호가 상한(%u)을 넘었습니다.", kMaxPoolCount);

	// Pool 은 alignas(64). 일반 new 는 C++17 aligned new 를 탄다. dbg_new(placement 인자) 와는 혼용 안 되므로
	// 정렬 할당 + placement new 로 명시한다. 해제하지 않는다 (D8).
	void* pMem = ::operator new(sizeof(Pool), std::align_val_t{ alignof(Pool) });
	Pool* pPool = new (pMem) Pool{};
	pPool->idx_ = idx;

	if (idx < kFastPoolCount)
	{
		FastTable[idx].Store(pPool);
		return pPool;
	}

	const _u32 overflowIdx = idx - kFastPoolCount;
	const _u32 segIdx = overflowIdx >> kSegmentBits;

	Segment* pSegment = SegmentTable[segIdx].Load();
	if (pSegment == nullptr)
	{
		// 같은 세그먼트에 속한 번호를 받은 다른 스레드와 경쟁할 수 있다. CAS 로 한 명만 이긴다.
		Segment* pFresh = new Segment{};
		Segment* pExpected = nullptr;
		if (SegmentTable[segIdx].CompareExchange(pExpected, pFresh))
		{
			pSegment = pFresh;
		}
		else
		{
			delete pFresh;
			pSegment = pExpected;
		}
	}

	pSegment->slots_[overflowIdx & (kSegmentSize - 1)].Store(pPool);
	return pPool;
}
```

`PoolCount`가 번호를 발급한 직후 ~ 테이블 store 사이에 다른 스레드가 그 번호로 `PoolAt()`을 부를 수 있는가? 없다. 그 번호를 가진 객체는 `Pop()`가 만들고, `Pop()`는 `AttachPool()`이 끝난 다음에만 실행된다(I8).

### 6.7 고아 목록: `PopOrphan()` / `PushOrphan()` / `GetOrphanLock()`

```cpp
//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
typename ObjectPoolTLSIndexed<T>::TLock& ObjectPoolTLSIndexed<T>::GetOrphanLock()
{
	static TLock lock;
	return lock;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
typename ObjectPoolTLSIndexed<T>::Pool* ObjectPoolTLSIndexed<T>::PopOrphan()
{
	JC_LIB_LOCK_GUARD(GetOrphanLock());

	Pool* pPool = OrphanHead;
	if (pPool == nullptr)
	{
		return nullptr;
	}

	OrphanHead = pPool->pNextOrphan_;
	pPool->pNextOrphan_ = nullptr;
	--OrphanCount;
	return pPool;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void ObjectPoolTLSIndexed<T>::PushOrphan(Pool* _pPool)
{
	JC_LIB_LOCK_GUARD(GetOrphanLock());

	_pPool->pNextOrphan_ = OrphanHead;
	OrphanHead = _pPool;
	++OrphanCount;
}
```

### 6.8 `SweepOrphans()` / `FreeChain()` / `TrimThisThread()`

```cpp
//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
_u32 ObjectPoolTLSIndexed<T>::FreeChain(FreeNode* _pHead)
{
	_u32 count = 0;
	while (_pHead != nullptr)
	{
		FreeNode* pNext = _pHead->pNext_;
		Memory::Deallocate(_pHead);
		_pHead = pNext;
		++count;
	}
	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void ObjectPoolTLSIndexed<T>::SweepOrphans()
{
	JC_LIB_LOCK_GUARD(GetOrphanLock());

	for (Pool* pPool = OrphanHead; pPool != nullptr; pPool = pPool->pNextOrphan_)
	{
		jc_assert(pPool->alive_.Load() == 0);
		FreeChain(pPool->inbox_.Exchange(nullptr));		// 고아가 된 뒤 뒦어 들어온 반납물
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void ObjectPoolTLSIndexed<T>::TrimThisThread()
{
	Pool* pPool = tls_pPool;
	if (pPool != nullptr)
	{
		FreeChain(pPool->pFree_);
		pPool->pFree_ = nullptr;
		pPool->totalCount_ -= pPool->freeCount_;
		pPool->freeCount_ = 0;
		FreeChain(pPool->inbox_.Exchange(nullptr));
	}

	SweepOrphans();
}
```

### 6.9 `Guard::~Guard()` — 스레드 종료

```cpp
//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
ObjectPoolTLSIndexed<T>::Guard::~Guard()
{
	if (idx_ == kNone)
	{
		return;
	}

	Pool* pPool = PoolAt(idx_);

	// 1) 먼저 고아 선언. 이후 foreign Push 는 ② 경로(즉시 힙)로 간다.
	pPool->alive_.Store(0);

	// 2) 내 프리리스트 힙 반납. (이 스레드만 만지는 것이라 락 불필요)
	FreeChain(pPool->pFree_);
	pPool->pFree_ = nullptr;
	pPool->freeCount_ = 0;
	pPool->totalCount_ = 0;

	// 3) inbox 두 번 drain.
	//    첫 번째: alive_=0 이전에 들어온 것.
	//    두 번째: "alive_==1 을 읽고 → 우리가 alive_=0 하고 1차 drain → 그 다음 CAS push" 한 연산을 거의 다 잡는다.
	//    두 번째 이후에도 극히 뒦게 push 된 것은 SweepOrphans() 가 회수한다.
	FreeChain(pPool->inbox_.Exchange(nullptr));
	FreeChain(pPool->inbox_.Exchange(nullptr));

	// 4) 번호 반납
	tls_pPool = nullptr;
	PushOrphan(pPool);
}
```

이 스레드가 다른 스레드에 넘긴 객체(아직 살아있고 `poolIdx_ == idx_`)는 어떤 리스트에도 없으므로 여기서 건드리지 않는다. 그 객체들은 나중에 `Push()`되뱴 (a) `alive_==0`이면 힙으로, (b) 번호가 재배정되어 `alive_==1`이면 새 owner의 inbox로 가서 재사용된다. 둘 다 옵바른 동작이다 — 객체는 같은 `T`의 같은 크기이고, 새 owner의 `totalCount_`가 약간 어그나는 것은 통계 오차일 뿐이다.

### 6.10 `GetStats()` / `ThisPoolIdx()`

```cpp
//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
typename ObjectPoolTLSIndexed<T>::Stats ObjectPoolTLSIndexed<T>::GetStats()
{
	Stats stats{};
	stats.poolCount = PoolCount.Load();

	for (_u32 i = 0; i < stats.poolCount; ++i)
	{
		Pool* pPool = PoolAt(i);
		if (pPool->alive_.Load() == 0)
		{
			continue;
		}
		++stats.alivePoolCount;
		stats.totalCount += pPool->totalCount_;		// owner 가 쓰는 중일 수 있다. 스냅샷이므로 근사.
		stats.freeCount  += pPool->freeCount_;
	}

	{
		JC_LIB_LOCK_GUARD(GetOrphanLock());
		stats.orphanPoolCount = OrphanCount;
	}
	return stats;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
_u32 ObjectPoolTLSIndexed<T>::ThisPoolIdx()
{
	return tls_pPool != nullptr ? tls_pPool->idx_ : kNone;
}
```

`totalCount_/freeCount_`를 owner가 비원자로 쓰는 동안 다른 스레드가 읽으믄 엄밀히는 data race다. 통계 훅은 근사값이면 충분하지만 **TSan에서는 감지된다**. TSan 빌드에서는 `GetStats()`를 부르지 않거나, 두 카운터를 `Atomic<_u32>`로 바꾸고 owner가 relaxed store 하도록 한다(§16 미결 M3).

### 6.11 `TlsPoolNewDeleteMixin<T>` (클래스 바깥 정의)

```cpp
////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void* TlsPoolNewDeleteMixin<T>::operator new(size_t _size)
{
	jc_assert_msg(_size == sizeof(T), "파생 타입은 이 풀로 할당할 수 없습니다.");
	return TPool::Pop();
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void* TlsPoolNewDeleteMixin<T>::operator new(size_t _size, int /*_blockUse*/, char const* /*_pFileName*/, int /*_lineNumber*/)
{
	return operator new(_size);			// dbg_new 호환. 풀이 이미 추적하므로 디버그 인자는 버린다.
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void TlsPoolNewDeleteMixin<T>::operator delete(void* _pObject)
{
	TPool::Push(static_cast<T*>(_pObject));
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void TlsPoolNewDeleteMixin<T>::operator delete(void* _pObject, int /*_blockUse*/, char const* /*_pFileName*/, int /*_lineNumber*/)
{
	TPool::Push(static_cast<T*>(_pObject));
}
```

`delete p`는 `~T()` 실행 후 `operator delete`를 부르므로 소멸자는 언어가 부른다. `static_cast<T*>(void*)`가 유효한 것은 `T`가 `TlsPoolNewDeleteMixin<T>`를 상속한 가장 바깥 타입이고 파생 타입은 `_size` assert로 막기 때문이다. `T`에 가상 소멸자가 있어도 되지만 그러면 `TlsPoolNode`가 offset 0이 아니게 되므로(vptr가 앞) `static_assert`에 걸린다 — **가상 함수가 있는 T는 지원하지 않는다**. 지원하려면 `poolIdx_`를 offset 8로 옮기고 링크를 offset 16으로 바꿔야 하는데, 1차에서는 하지 않는다(§16).

---

## 7. 경쟁 상황과 메모리 순서

`jc::Atomic`은 `Interlocked*` 기반이므로 모든 연산이 full barrier(seq_cst 이상)다. 아래 표의 "필요 순서"는 민상으로 나중에 `std::atomic`으로 바꿀 때의 기준이다. x64에서는 load는 어차피 `mov`이고 CAS/Exchange는 `lock` 접두어에서 차이가 없다. 단 `Store`가 `xchg`로 컬파일되는 것은 약간 버깝지만 콜드 패스에만 있다.

| # | 경쟁 상황 | 결과 | 근거 |
|---|---|---|---|
| R1 | owner가 `inbox_.Exchange(nullptr)` 하는 동안 foreign이 CAS push | 둘 중 하나가 직렬화된다. push가 먼저면 owner가 가져가고, 닦이면 새 inbox에 남는다. | Treiber 스택, push-only + 전체 pop. 개별 pop이 없으므로 ABA 없음. |
| R2 | foreign이 `alive_==1` 읽음 → owner가 `alive_=0` + 1차 drain → foreign이 CAS push | 2차 drain이 잡는다. | drain 사이 창은 수십 ns. 그보다 더 늦은 push는 R3. |
| R3 | R2의 push가 2차 drain 이후에 도착 | 고아 inbox에 잔류. 다음 `AttachPool()`의 `SweepOrphans()` 또는 `TrimThisThread()`가 회수. | 잔류 막은 이론상 무한히 일어날 수 있지만(foreign이 `alive_` 읽고 preempt 되어 1초 자는 경우) 버그는 아니다. 그 슬롯은 리스트에 있고 회수 경로가 있다. |
| R4 | 고아 번호가 재배정된 동시에 R3 push 도착 | 새 owner의 inbox로 들어가 새 owner가 재사용. | `poolIdx_ == idx_`이고 크기도 같은 `T`. 정합성 문제 없음. 통계 오차만. |
| R5 | 두 스레드가 동시에 같은 `Segment` 첫 생성 | CAS로 한 명만 이기고 패자는 자기 것을 지운다. | §6.6 |
| R6 | `CreatePool()`이 번호 발급 → 테이블 store 사이에 다른 스레드가 `PoolAt(그 번호)` | 발생 불가. 그 번호를 보유한 객체가 아직 없다. | I8. `GetStats()`는 `PoolAt(i)`가 null을 리턴할 수 있는 유일한 호출자 → **null 검사 추가 필요** (구현 시 `if (pPool == nullptr) continue;`). |
| R7 | `alive_=1` 이전에 재배정된 번호로 foreign Push 도착 | ② 경로로 힙 반납. | 안전. 사무실을 배정받았지만 아직 많이 안 있는 것. |
| R8 | `Push()`가 `PoolAt()`로 얻은 `Pool*`을 쓰는 동안 그 Pool이 사라진다 | 발생 불가. Pool은 해제되지 않는다. | D8 |
| R9 | `~Guard()` 중 자기 `pFree_`를 해제하는 동안 foreign이 CAS push | 다른 메모리(`inbox_`)이므로 충돌 없음. | I4 |
| R10 | 정적 소멸 순서: `main` 반환 후 `tls_Guard` 소멸자가 돌고 **그 다음** 전역 `PooledString`이 소멸 | `tls_pPool == nullptr`이므로 ②/③ 경로. `alive_==0`이면 힙 반납. 안전. | §6.2 설명. 그러나 **전역 `PooledString`은 금지** (§17) — 안전해도 이점이 없고 `FastTable` 등 정적 엄버가 이미 소멸된 뒤일 수도 있다(`Atomic`이 trivially destructible이면 문제 없음 — 확인 항목). |

매모리 순서 요구(나중에 `std::atomic`으로 옵시에):

| 연산 | 최소 순서 | 이유 |
|---|---|---|
| `inbox_` CAS push | release | 노드의 `pNext_` 쓰기가 push 전에 보여야 |
| `inbox_` Exchange(nullptr) | acquire | 가져온 체인의 내용을 읽기 전 |
| `alive_` Store(0) in `~Guard` | seq_cst (또한 foreign의 `alive_` Load도 seq_cst) | R2의 "읽고 → 쓰고 → drain" 사이에 개입하는 push를 2차 drain이 보도록. acq/rel만으로는 "foreign이 alive=1을 보고 owner가 drain에서 노드를 안 보는" 조합이 가능(Store-Load reorder). `Interlocked`는 full barrier라 문제 없음. |
| `FastTable/Segment` Store / Load | release / acquire | `Pool` 내용 초기화가 테이블 등록 전에 보여야 |
| `PoolCount` Increment | relaxed 가능 | 번호 고유성만 필요 |

---

## 8. 스레드 생성·종료 타임라인

```
시간 →

A 시작   A: 첫 Pop → ThisPool() → tls_pPool == nullptr → AttachPool()
              SweepOrphans() (아무것도 없음) → PopOrphan() == nullptr → CreatePool() → idx 0
              FastTable[0] = P0, P0.alive_ = 1, tls_pPool = P0, tls_Guard.idx_ = 0
              힙에서 객체 X 할당, X.poolIdx_ = 0

B 시작   B: 첫 Pop → idx 1, FastTable[1] = P1

          A: X 를 B 로 이동 (move) — 풀은 모른다

          B: Push(X) → X.poolIdx_(0) != P1.idx_(1) → PoolAt(0) = P0 → P0.alive_ == 1 → P0.inbox_ 에 CAS push

          A: Pop() → pFree_ 빔 → P0.inbox_.Exchange(nullptr) → X 회수 → 재사용

A 종료   A: ~tls_Guard → P0.alive_ = 0 → pFree_ 힙 반납 → inbox drain ×2 → tls_pPool = nullptr → PushOrphan(P0)
              (FastTable[0] 은 여전히 P0. P0 해제 안 함)

          B: Push(Y, Y.poolIdx_ == 0) → PoolAt(0) = P0 → P0.alive_ == 0 → 즉시 힙 반납

C 시작   C: 첫 Pop → AttachPool() → SweepOrphans() (P0.inbox_ 잔류 회수) → PopOrphan() = P0 → P0.alive_ = 1
              tls_pPool = P0, tls_Guard.idx_ = 0.  → C 가 idx 0 의 새 owner

          B: Push(Z, Z.poolIdx_ == 0, A 가 예전에 만든 것) → P0.alive_ == 1 → P0.inbox_ → C 가 재사용  (R4, 정상)
```

MSVC 관련 사실:
- `thread_local` 비-trivial 객체의 소멸자는 `__tlregdtor`로 등록되어 `ExitThread`/스레드 함수 반환 시 실행된다. `_beginthreadex`, `std::thread`, `jc::Thread` 모두 해당.
- `TerminateThread`로 죽이면 소멸자가 안 돈다 → `alive_`가 1로 남고 번호가 재사용되지 않는다. 크래시는 아니고 그 풀의 객체들이 잔류한다. `TerminateThread`는 어차피 금기다.
- 정적 라이브러리(jc는 static lib) 기준. DLL에 넘기면 `FastTable` 등 복사본이 모듈별로 생기므로 경계를 넘는 `Push()`가 잘못된 테이블을 본다. `JC_DLL`로 export하거나 DLL 경계를 넘기지 않아야 한다(§17).

---

## 9. `StringSlot` / `StringImpl_SSOCtxTlsPool` 적용

### 9.1 64B 슬롯 레이아웃

```cpp
// jc/Primitives/StringImpl_SSOCtxTlsPool.h

template <typename CharT>
struct StringSlot : TlsPoolNode
{
	static constexpr int kHeapHeaderSize = sizeof(CharT*) + sizeof(int) * 2;							// 16
	static constexpr int kSsoBytes       = JC_CACHE_ALIGN_SIZE - sizeof(TlsPoolNode) - kHeapHeaderSize;	// 64 - 4 - 16 = 44
	static constexpr int kSsoCapacity    = kSsoBytes / sizeof(CharT) - 1;								// char: 43, wchar_t: 21 (NUL 제외)

	// offset 0  : poolIdx_ (TlsPoolNode)   ← 풀 전용. 프리 상태에서도 보존
	// offset 4  : len_                     ← 프리 상태에서 FreeNode::reserved_ 자리. 풀이 건드리지 않음
	int		len_;
	// offset 8  : pHeap_                   ← 프리 상태에서 FreeNode::pNext_ 가 덮어쓴다. Push 전에 힙 버퍼는 이미 해제되어 있어야 한다.
	CharT*	pHeap_;
	// offset 16 : cap_                     ← 힙 모드일 때 용량(NUL 제외). SSO 모드는 pHeap_ == nullptr 로 구별
	int		cap_;
	// offset 20 : sso_[44]
	CharT	sso_[kSsoBytes / sizeof(CharT)];

	bool IsHeap() const { return pHeap_ != nullptr; }
	CharT* Buf() { return IsHeap() ? pHeap_ : sso_; }
	int Cap() const { return IsHeap() ? cap_ : kSsoCapacity; }
	void InitEmpty() { len_ = 0; pHeap_ = nullptr; cap_ = 0; sso_[0] = CharT(0); }
};
static_assert(sizeof(StringSlot<char>) == JC_CACHE_ALIGN_SIZE);
static_assert(sizeof(StringSlot<wchar_t>) == JC_CACHE_ALIGN_SIZE);
static_assert(offsetof(StringSlot<char>, pHeap_) == 8);		// 프리 상태 링크 자리와 일치 확인
```

이전 대화의 "union{ heap_ ; pNext_ }" 형태 대신 필드를 펼쳐 `pHeap_`을 정확히 offset 8에 놓는다. 풀이 링크로 쓰는 자리가 `pHeap_`이고, 프리 상태에서는 힙 버퍼가 없으므로 자연스럽게 겹친다. `len_`을 offset 4에 놓아 패딩을 없애고, S4 벤치(SSO 47)보다 4B 적은 43이 나온다(`poolIdx_` 번호 버용). S5~S7의 43과 같다.

### 9.2 `StringImpl_SSOCtxTlsPool<CharT>` — Storage 계약 구현

재설계 문서의 Storage 계약(`value_type, Data, Len, SetLen, Cap, Reserve, Assign, Clear` + 검토에서 추가된 `Adopt`)을 그대로 갖는다. 멤버는 포인터 하나.

```cpp
template <typename CharT>
class StringImpl_SSOCtxTlsPool
{
public:
	using value_type = CharT;
	using TSlot = StringSlot<CharT>;
	using TPool = ObjectPoolTLSIndexed<TSlot>;

	StringImpl_SSOCtxTlsPool() = default;									// pCtx_ == nullptr : 빈 문자열. 풀 안 건듦.
	StringImpl_SSOCtxTlsPool(const StringImpl_SSOCtxTlsPool& _other);		// _other 가 비어있으면 풀 안 건듦
	StringImpl_SSOCtxTlsPool(StringImpl_SSOCtxTlsPool&& _other) noexcept;	// 슬롯 포인터만 넘김
	~StringImpl_SSOCtxTlsPool();
	StringImpl_SSOCtxTlsPool& operator=(const StringImpl_SSOCtxTlsPool& _other);
	StringImpl_SSOCtxTlsPool& operator=(StringImpl_SSOCtxTlsPool&& _other) noexcept;

	const CharT* Data() const;			// 슬롯 없으면 EmptyStr<CharT>()
	CharT* Data();						// 쓰기 전에 Reserve 하도록 BasicString 이 보증. 슬롯 없으면 Ctx() 회득.
	int Len() const;
	void SetLen(int _len);				// Buf()[_len] = 0 포함
	int Cap() const;					// NUL 제외. 슬롯 없으면 0
	void Reserve(int _capacity);		// 축소 없음. 필요시 Ctx() 회득. SSO → 힙 승급
	void Assign(const CharT* _pStr, int _len);	// 깊은 복사. self/overlap 안전 (임시 버퍼 경유 또는 memmove)
	void Clear();						// 슬롯 유지, len_ = 0
	void Adopt(CharT* _pBuffer, int _len, int _capacity);	// 외부 힙 버퍼 소유권 인수 (Format 등)

private:
	TSlot* Ctx();						// pCtx_ 가 null 이면 TPool::Pop() + InitEmpty()
	void FreeHeapBuf();					// pHeap_ 있으면 Memory::Deallocate, 없게
	void ReleaseCtx();					// FreeHeapBuf() 후 TPool::Push(pCtx_), pCtx_ = nullptr

	TSlot* pCtx_ = nullptr;
};
```

### 9.3 `Pop` 호출 시점 표 (S4 벤치 `StrForBench_S4.inl` 매핑)

| 연산 | 풀 Pop? | S4 벤치 위치 | 이유 |
|---|---|---|---|
| 기본 생성자 | ✗ | L280 | `nullptr` = 빈 문자열. S4 Construct/0 6.28ns의 원천. |
| 내용 있는 생성자 (`const CharT*`, `StringView`) | ○ | L285/L294/L308 | 쓸 곳이 필요 |
| 복사 생성자 | src 비어있으면 ✗, 아니면 ○ | L337 | |
| 이동 생성자 | ✗ | L394 | 슬롯 포인터 steal, src는 nullptr |
| 복사 대입 | dst에 슬롯 없고 src 비어있지 않으면 ○ | L406~423 | dst에 이미 슬롯이 있으면 재사용 |
| 이동 대입 | ✗ | | dst 슬롯은 `ReleaseCtx()` 후 steal. (또는 swap — 설계 선택: **swap**. dst의 슬롯이 src로 가고 src 소멸 시 반납. 풀 반납 지연이 있지만 Push 후 재-Pop 가능성 차단) |
| 첫 쓰기 (`Reserve/Append/Format/SetLen/Data()` non-const) | ○ | L230 `Ctx()` | |
| `Clear()` | ✗ | | 슬롯 유지 (다시 카운터 안 타게) |
| 읽기 (`Data() const, Len, Cap`) | ✗ | | `EmptyStr<CharT>()` 정적 버퍼 |
| 소멸자 | ✗ | L239 `FreeCtx` | `FreeHeapBuf()` → `Push(pCtx_)` — **반드시 이 순서** (offset 8 링크가 pHeap_를 덮는다) |

### 9.4 2단계 소멸이 중요한 이유

`Push(pCtx_)` 직후 풀은 `pCtx_->pHeap_` 자리에 `pNext_`를 쓴다. 힙 버퍼를 리스트에 반납하기 전에 반드시 `Memory::Deallocate(pHeap_)` 해야 한다. 반대로 하면 힙 버퍼 주소가 사라져 리크. 검사 방법: `ReleaseCtx()`에서 `jc_assert(pCtx_->pHeap_ == nullptr)` 후 `Push`.

### 9.5 `EmptyStr<CharT>()`

```cpp
template <typename CharT>
JC_FORCEINLINE const CharT* EmptyStr()
{
	static constexpr CharT kEmpty[1] = { CharT(0) };
	return kEmpty;
}
```

`Data() const`가 슬롯 없을 때 반환. 이 포인터에 쓰기 금지 — `BasicString`은 쓰기 전에 항상 `Reserve()`(또는 non-const `Data()`)를 부른다는 계약을 재설계 문서에 반드시 추가한다(§11 F5).

---

## 10. 비용 분석

### 10.1 경로별 연산 수

| 경로 | TLS 로드 | 일반 로드/스토어 | 원자 | 락 | 예상 |
|---|---|---|---|---|---|
| `Pop` 핫 (pFree_ 있음) | 1 | 3 | 0 | 0 | ~2ns (S4 벤치 Construct/1 9.63ns에 포함된 풀 부분) |
| `Pop` inbox drain | 1 | 3 | 1 Exchange | 0 | ~20ns. inbox 가 비어 있을 때도 Exchange 1회 발생 → §16 M1 (`Load` 선행 검사) |
| `Pop` 힙 | 1 | 2 | 0 | 0 | malloc 64B ≈ 20~50ns |
| `Push` ① 내 풀 | 1 | 3 | 0 | 0 | ~2ns |
| `Push` ②③ foreign (fast table) | 1 | 3 + 1 (`alive_`) | 1 Load + CAS 1회(무경쟁) | 0 | 20~40ns. S4 CrossThread 16,009ns/1000개 ≈ 16ns/개를 약간 초과 예상 (alive_ 읽기 추가) |
| `Push` foreign (segment) | 1 | +2 | 같음 | 0 | +1~2ns |
| `Push` foreign, owner 죽음 | 1 | 3 | 1 Load | 0 | free 64B ≈ 20~40ns |
| `AttachPool` (스레드당 1회) | | | 수 회 | SpinLock 2회 | µs 급. 무시 |
| `~Guard` (스레드당 1회) | | O(프리 개수) free | 3 | 1 | 프리리스트 길이 뱀 |

### 10.2 벤치 기대값 (검증 기준)

| 벤치 | S4 (현재) | 기대 | 허용 |
|---|---|---|---|
| Construct/0 | 6.28ns | 동일 | ±5% |
| Construct/1 | 9.63ns | 동일~+0.5ns (`InitEmpty` 추가) | +10% |
| Sort | 0.500ms | 동일 | ±5% |
| MT t8 | 7.88ns | 동일 | ±10% |
| CrossThread (1000개) | 16,009ns | 16,000~20,000ns | ≤ S0 20,508 |
| OwnerExit | 1,918ns | 2,000~4,000ns (힙 반납 경로) → **개정: ~7,000ns 수용 (2026-09-06 실측 6,906ns)**. 구 허용(≤ S5 3,830)은 공유 풀 전제라 D6(스레드별 풀 + 고아 즉시 힙 반납)과 양립 불가. 객체당 malloc+free 2회가 필수이며 owner-exit는 종료 시 1회성 경로다. 수용 기준은 안전성(UAF/릭 0) + 내용 일치로 대체. | ≤ S5 3,830 → 개정됨 |
| FindCompare | 12.2ns | 동일 (풀 무관) | ±5% |
| SSO_CAP | 47 | **43** | 고정 |

마지막 항이 유일한 후퇴다: `poolIdx_` 4B를 사면서 cross-thread 안전을 얻는다. Mid 데이터셋(40B)은 여전히 SSO에 들어가므로 데이터셋 0~3 결과에는 영향 없다.

---

## 11. 수정·생성 파일 목록

경로는 `Projects/jc/Sources/jc/` 기준. 각 항의 "확인"은 그 파일 작업이 끝났다고 볼 수 있는 조건이다.

### 11.1 신규 파일

| # | 파일 | 내용 | 확인 |
|---|---|---|---|
| F1 | `Pool/ObjectPoolTLSIndexed.h` | §5 헤더 + §6 전체 함수 정의. 헤더 온리. `TlsPoolNode`도 이 파일. | 컴파일. `static_assert` 통과. |
| F2 | `Primitives/StringImpl_SSOCtxTlsPool.h` | §9 `StringSlot<CharT>`, `StringImpl_SSOCtxTlsPool<CharT>`, `EmptyStr<CharT>()`. 헤더 온리. | `static_assert(sizeof == JC_CACHE_ALIGN_SIZE)`, `offsetof(pHeap_) == 8`. |
| F3 | `Primitives/StringImpl_SSO.h` | 재설계 문서의 기본 Storage (풀 무관). 이 문서 범위 밖이지만 개발 순서상 앞에 온다. | |
| F4 | `Primitives/StringImpl_SSOCtxTlsPool.natvis` 혹은 기존 `Natvis/jc.natvis`에 추가 | `pCtx_ == nullptr` → `""`, 아니면 `pCtx_->IsHeap() ? pHeap_ : sso_`. `poolIdx_` 표시. | VS 워치창에서 PooledString 값 보임. |
| F5 | (문서) `2026-09-06-String-BasicString-재설계.md` 개정 | §4 Storage 계약에 `Adopt` 추가, "쓰기 전 Reserve 보증" 계약 추가, `Null` 폐기, `Capacity()→Cap()` 매핑, §5 `StringImpl_SSOCtxTlsPool` 설명을 "`ObjectPoolTLSIndexed<StringSlot<CharT>>` 사용, S8 정책"으로 교체, SSO 43 명시, 작업순서 4를 이 문서 §12로 연결. | |

### 11.2 수정 파일

| # | 파일 | 수정 내용 | 확인 |
|---|---|---|---|
| M1 | `Define.h` | `#define JC_CACHE_ALIGN_SIZE 64` 추가. `JC_INFINITE` 인접 위치. | 모든 프로젝트 보이는 곳. |
| M2 | `Aligner.h` | L13, L22, L40 `alignas(64)` → `alignas(JC_CACHE_ALIGN_SIZE)`. 다른 변경 없음. | `grep -n "alignas(64)"` 결과 0건. |
| M3 | `Primitives/Atomic.h` | `Atomic<T>`, `Atomic<T*>` 기본 생성자를 `constexpr Atomic() : value_{} {}`로(이미 constexpr이면 변경 없음). `Atomic<T*>`에 `Load/Store/Exchange/CompareExchange` 존재 확인(있음). `Atomic<_u32>::Increment()`가 증가 후 값을 반환하는지 확인해 `CreatePool()`의 `-1` 유지/제거. 소멸자가 trivial인지 확인(R10). | `inline static Atomic<Pool*> FastTable[256];`가 상수 초기화되는지 (`static_assert(std::is_trivially_destructible_v<Atomic<void*>>)`). |
| M4 | `Primitives/SmartPtr.h` L92, L524~526 | `IsBaseOf_v<ObjectPool<T>, T>` 검사를 `IsPooledObject_v<T>` 트레이트로 솟아내고, `ObjectPool<T>` 또는 `TlsPoolNewDeleteMixin<T>` 상속 시 true. `MakeShared` 경로가 `new`를 쓰므로 Mixin이 있으면 자동으로 풀을 탄다. **완료 (2026-09-06).** Mixin은 비-중첩 템플릿이다 (중첩 시 CRTP 충돌. §16.3). 테스트: SmartPtrTest에 trait 정적 단언 + `dbg_new/delete` 풀 재사용 + 외부 포인터 SharedPtr 경로. 네거티브 컴파일(`MakeShared<MixinType>` 차단) 확인. | 기존 `ObjectPool` 테스트 통과 (전체 371/371). |
| M5 | `Primitives/StringUtil.h/.cpp` | 재설계 문서 작업 0~1 (`tolower` unsigned cast, `Compare` unsigned/`-1,0,1`, 템플릿화). 이 문서 범위 밖. | |
| M6 | `Primitives/String.h/.cpp` | 재설계 문서 작업 2 (`BasicString<CharT, Storage>`), 그리고 `using PooledString = BasicString<char, StringImpl_SSOCtxTlsPool<char>>;` 선언. 이 문서 범위는 별칭 추가만. | |
| M7 | `Projects/jc/CMakeLists.txt` / `.vcxproj` / `.filters` | F1, F2 추가. jc는 헤더를 프로젝트 파일에 명시하므로 누락 시 IDE에서 안 보인다. | |
| M8 | `Projects/jc_gtest/Sources/jc_gtest/Pool/ObjectPoolTLSIndexedTest.cpp` (신규) | §13 V1~V9. | gtest 통과. |
| M9 | `Projects/jc_gtest/...` `PooledStringTest.cpp` (신규) + 기존 `StringTest.cpp` 매크로화 | §13 V10~V12 + 재설계 문서 B11~B13. 기존 String 기반 케이스를 고치지 않고 매크로(§19 R1)로 `String`/`PooledString` 양쪽 실행. | |
| M10 | `Projects/jc_gbench/Sources/jc_gbench/Primitives/StrForBench_S8.inl` (신규) + `StrForBench.h/.cpp` 등록 | 실제 `PooledString`을 `StrForBench` 인터페이스로 감싼 S8. 기존 S0~S7과 같은 벤치 실행. | §10.2 표 만족. |
| M12 | `Projects/jc_gbench/Sources/jc_gbench/Pool/ObjectPoolTLSIndexedBench.cpp` (신규) | 풀 단독 벤치 (§19 R2). `Pool/MemoryPoolTest.cpp`가 디렉토리 선례. | §19 벤치 표 만족. |
| M13 | 전 프로젝트 문자열 리터럴 `_T()` 전환 (별도 페이즈 P12) | §19 R5. jc 276파일 중 261개가 리터럴 포함. **AGENTS.md 제약(5파일 이상 수정 전 확인)에 따라 P12 착수 전 별도 확인 필수.** | |
| M11 | `Projects/jc_gbench/.../StrForBenchPool.h` | 변경 없음(벤치 전용 풀 유지, S4 문서 참고용). 단, 헤더 주석에 "실제 구현은 jc/Pool/ObjectPoolTLSIndexed.h" 추가. | |

### 11.3 손대지 **않는** 파일

- `Pool/ObjectPool.h`, `Pool/AccessibleObjectPool.h`, `Pool/IndexedMemoryPool.h` — 기존 동작 유지.
- `Threading/Thread.h` — `tls_uiThreadId` 등은 사용하지 않는다. 풀은 자기 TLS만 쓴다.
- `Sync/SpinLock.h` — 그대로 사용.

---

## 12. 개발 순서

각 단계는 **단계별로 컴파일되고 테스트가 통과한 상태**로 끝난다. 단계를 묶지 말 것. 재설계 문서의 작업 0~3(StringUtil, BasicString + StringImpl_SSO)은 이 문서와 **독립적으로 진행 가능**하고 단계 P6에서 합류한다.

| 단계 | 작업 | 파일 | 완료 조건 | 예상 |
|---|---|---|---|---|
| **P0** | `JC_CACHE_ALIGN_SIZE` 추가, `Aligner.h` 교체 | M1, M2 | 전체 솔루션 빌드. `grep alignas(64)` 0건. | 10분 |
| **P1** | `Atomic` constexpr 생성자 / trivially destructible / `Increment` 반환값 확인 | M3 | `static_assert` 2개 헤더에 추가해 컴파일. 기존 Atomic 테스트 통과. | 30분 |
| **P2** | `ObjectPoolTLSIndexed.h` 작성 — `TlsPoolNode`, `TlsPoolNewDeleteMixin<T>` (클래스 바깥), `Pool`, `Segment`, `Guard`, 테이블, `PoolAt/CreatePool/ThisPool/AttachPool`, `Pop/Push` (고아 경로 포함), `PopOrphan/PushOrphan/SweepOrphans/FreeChain/TrimThisThread/GetStats/ThisPoolIdx`, `~Guard` | F1, M7 | `Dummy64 : TlsPoolNode { char pad_[60]; }`로 인스턴스화 컴파일. | 반일 |
| **P3** | 풀 단독 gtest V1~V9 (§13) | M8 | 모두 통과. Debug/Push. | 반일 |
| **P4** | ASan / TSan으로 P3 재실행 (clang-cl 또는 MSVC `/fsanitize=address`; TSan은 WSL clang) | — | 보고 0건. `GetStats()` 범프리 테스트는 TSan에서 제외하거나 M3 미결 처리. | 반일 |
| **P5** | `StringImpl_SSOCtxTlsPool.h` 작성 (`StringSlot`, Storage 계약, `EmptyStr`) + natvis | F2, F4 | `static_assert` 통과. 재설계 문서의 Storage 계약 인터페이스와 시그니처 일치. | 반일 |
| **P6** | `BasicString<char, StringImpl_SSOCtxTlsPool<char>>` 인스턴스화 → `PooledString` 별칭. (재설계 작업 2가 선행되어 있어야 함) | M6 | 기존 `String` gtest를 `PooledString`으로 타입만 바꿔 돌려 통과(템플릿 테스트 `TYPED_TEST` 추천). | 1일 |
| **P7** | PooledString 풀 경계 gtest V10~V12 + 재설계 B11~B13 | M9 | 통과, ASan/TSan 통과. | 반일 |
| **P8** | 벤치 S8 추가, 실행, §10.2 표와 범교 | M10 | 허용 범위 만족. 실추하면 원인 분석 후 §16 갱신. | 반일 |
| **P9** | (선택) `SmartPtr.h` 트레이트 일반화 | M4 | 기존 SmartPtr 테스트 통과. | 반일 |
| **P10** | 문서 개정 (재설계 문서 F5, 이 문서 §16 갱신) | F5 | | 1시간 |
| **P11** | TCHAR 대응 설계 확정 (§19 R4, 미결 M7) | — | A안/B안 중 채택. P12 선수조건. | 2시간 |
| **P12** | 문자열 리터럴 `_T()` 전환 (§19 R5) | M13 | 의존성 역순(jc → jc_gbench/jc_gtest → sgcl)으로 단계별 빌드 통과. **착수 전 별도 확인 필수.** | 수일 |

순서 이유: 풀을 문자열과 묶어서 한 번에 테스트하면 버그가 어느 쪽인지 알기 어렵다. `Dummy64`로 풀만 먼저 TSan까지 통과시켜 놓고(P2~P4), 그 다음 문자열을 엉는다.

---

## 13. 검증 계획

테스트 파일: `jc_gtest/Pool/ObjectPoolTLSIndexedTest.cpp`. 테스트용 타입:

```cpp
struct Dummy64 : TlsPoolNode
{
	char pad_[JC_CACHE_ALIGN_SIZE - sizeof(TlsPoolNode)];
};
using TDummyPool = ObjectPoolTLSIndexed<Dummy64>;
```

| # | 테스트 | 방법 | 기대 |
|---|---|---|---|
| V1 | 단일 스레드 Pop/Push LIFO | Pop a,b → Push b,a → Pop → a 반환 | 주소 재사용. `GetStats().totalCount == 2`, `freeCount` 변화 일치 |
| V2 | `poolIdx_` 불변 | Pop → idx 기록 → Push → Pop → 같은 idx | I1 |
| V3 | SPSC cross-thread | A가 N=10⁴ 개 Pop → 큐로 B에 전달 → B가 Push → A가 다시 N개 Pop | A의 `totalCount` 증가 없음(inbox drain으로 회수). 누수 없음(ASan) |
| V4 | owner 종료 후 foreign Push | A가 만든 객체를 B가 보유 → A join → B Push | 크래시 없음, ASan 누수 없음. `GetStats().orphanPoolCount == 1` |
| V5 | 번호 재사용 | A 시작→종료 → C 시작 → `C의 ThisPoolIdx() == A의 idx` | 고아 pop |
| V6 | R4 시나리오 | A의 객체를 B 보유 → A 종료 → C가 A 번호 인수 → B Push → C Pop | C가 그 주소를 받음(재사용). 누수 없음 |
| V7 | 스레드 생성·종료 반복 | 1,000회 `std::thread([]{ Pop; Push; }).join()` | `PoolCount` 이 1~2에서 멈침(번호 재사용). 누수 없음 |
| V8 | 세그먼트 구간 | 300 스레드 동시 생성해 각자 Pop 보유 → 배리어 → 서로 교차 Push | idx ≥ 256 구간 경로 실행. 누수 없음. (스레드 수는 배리어로 동시 존재 보증) |
| V9 | `TrimThisThread`/`SweepOrphans` | V4 후 `SweepOrphans()` → `Dump` | 고아 inbox 비어있음 |
| V10 | PooledString 기본 | 기존 String 테스트 TYPED_TEST로 `String`, `PooledString` 동시 | 동일 결과 |
| V11 | PooledString 힙 모드 + cross-thread 소멸 | 4096B 문자열을 A에서 만들고 B에서 소멸 | ASan 누수 없음 (§9.4 2단계 소멸 검증) |
| V12 | PooledString 빈 문자열 | 기본 생성 1만 개 → `GetStats().totalCount == 0` | 기본 생성자가 Pop 안 함 |
| V13 | 재설계 B11~B13 | 재설계 문서 그대로 | |

TSan 범위: V3, V4, V6, V7, V8. `GetStats()` 호출은 TSan 빌드에서 `#ifndef JC_TSAN`으로 제외하거나 §16 M3를 적용.

---

## 14. 코딩 컨벤션 체크리스트 (jc 기준)

이 프로젝트에 별도 컨벤션 문서(`.opencode/rules/coding-conventions_cpp.md`)가 참조되지만 저장소에 없으므로 기존 코드(`ObjectPool.h`, `String.cpp`, `SpinLock.h`, `CMessage.h`, `StringView.h`)에서 도재한 규칙이다. 구현자는 PR 전에 이 표로 자기 검사한다.

| 항목 | 규칙 | 예 |
|---|---|---|
| 파일 헤더 | `/* 작성자: 윤정도 / 생성일: M/D/YYYY / ===== / 설명 */` 블록 → 번 줄 → `#pragma once` → include → `NS_JC_BEGIN` ... `NS_END` | `ObjectPool.h` L1~9 |
| 들여쓰기 | 탭 | |
| 중괄호 | Allman (`{` 별줄). 한 줄 짜리 인라인 getter는 같은 줄 허용 | `Atomic.h` |
| 함수 구분자 | 함수 정의 **매 개** 바로 앞에 `/` 88개 한 줄. 클래스 밖 정의에 적용. 클래스 안 인라인 정의에는 안 붙임 | `String.cpp` L20, 28, 43 |
| 함수/메서드 이름 | PascalCase | `Pop`, `PoolAt`, `SweepOrphans` |
| 파라미터 | `_camelCase`, 포인터는 `_pCamelCase` | `_pObject`, `_idx`, `_pHead`, `_size` |
| 로컬 변수 | camelCase, 포인터는 `p` 접두 | `pPool`, `pNode`, `overflowIdx`, `count` |
| 멤버 변수 | camelCase + `_` 접미, 포인터는 `p` 접두 | `pFree_`, `idx_`, `inbox_`, `pCtx_` |
| static 멤버 | PascalCase, 접미 없음, `inline static` | `FastTable`, `PoolCount`, `OrphanHead` (`ObjectPool.h`의 `Head`, `Lock`, `TotalCount`) |
| thread_local | `tls_` 접두 (구건) + 멤버 규칙 | `tls_pPool`, `tls_Guard` (`Thread.h`의 `tls_uiThreadId`) |
| 상수 | `k` 접두 PascalCase (`constexpr`), 매크로는 `JC_UPPER_SNAKE` | `kFastPoolCount`, `JC_CACHE_ALIGN_SIZE` |
| 타입 별칭 | `T` 접두 | `TPool`, `TLock`, `TSlot` (`ObjectPool.h`의 `TPool`, `TLock`, `TGuard`) |
| 기본 타입 | `_u32`, `_u64`, `_u8`, `size_t` | |
| 단언 | `jc_assert(expr)`, `jc_assert_msg(expr, fmt, ...)` | `Assert.h` L22, 27 |
| 로그 | `_LogWarn_(fmt, ...)` | `ObjectPool.h` |
| 힙 할당 | `Memory::Allocate<T*>(size)` / `Memory::Deallocate(p)` (raw), `dbg_new` (객체) | `Memory.h` L47, 55 |
| 락 | `JC_LIB_LOCK_GUARD(lock)` | `ILock.h` L41 |
| 예약어 | `final` 상속 금지 클래스에, `noexcept` 이동/소멸자 | `SpinLock final` |
| 주석 | 한글 허용. 왜인지를 쓴다 | |

---

## 15. 수락 기준 (Definition of Done)

1. P0~P8 완료, §13 V1~V13 모두 Debug/Push 통과.
2. ASan 0건. TSan 0건 (`GetStats` 제외 또는 원자화).
3. §10.2 벤치 허용 범위 만족. 미달 시 원인을 §16에 기록하고 보고.
4. `grep -rn "alignas(64)" Projects/jc/Sources` 0건.
5. §14 체크리스트 자기 검사 완료. 특히 88-슬래시 구분자가 모든 함수 정의 앞에 있을 것.
6. 기존 `ObjectPool<T>` 관련 테스트 그대로 통과(회귀 없음).
7. `PooledString`을 전역/static으로 선언한 곳이 없다 (`grep static PooledString`). 있어야 하면 §17 확인 후 허용.
8. 풀 단독 벤치(M12) 실행, §19 R2 표 만족.
9. TCHAR 방안(P11) 확정됨 (B안, M7). P12는 별도 착수 확인 후 진행.

---

## 16. 결정 기록 / 기각 대안 / 미결

### 16.1 기각 대안

| 대안 | 기각 이유 |
|---|---|
| 고정 배열 `kMaxPools` | 스레드 수 초과 시 오류. |
| `std::vector<Pool*>` | 재할당 시 주소 이동 → 락 없는 reader가 죽은 메모리를 본다. |
| 슬롯에 `Pool*` 8B 직접 저장 | SSO 43→39. 죽은 풀 판별을 위해 별도 구조 필요. 테이블 조회 절약은 로드 1회. |
| `virtual ~` 기반 (기존 ObjectPool 방식) | vptr 8B 손해 + offset 0 요구 충돌. |
| 상시 `pNext_` 멤버 | 살아있는 객체 8B 손해. |
| 전역 SpinLock 풀 (기존 ObjectPool 재사용) | S5가 이것. MT t8에서 경쟁. |
| 청크(포인터+오프셋) 인덱스 핸들 | 8B 포인터와 크기 같고 복잡도만 늠. |
| lock-free 인덱스 스택으로 고아 목록 | ABA 처리(태그 추가) 필요. 콜드 패스에 이득 없음. D7. |
| `std::atomic` 사용 | 컨벤션 위배. `jc::Atomic` 이 full barrier라 성능 상 x64 차이 없음. D9. 단 M1 적용 시 `Load`는 어느 쪽이든 plain mov. |
| 슬롯을 캐시라인 **정렬** (`_aligned_malloc`) | S4 벤치도 정렬 안 함. 정렬 할당은 더 느리고 이득 불분명. 2차에서 청크 할당으로 가번 자연 해결. |
| `Pop`가 생성자 호출 | String 슬롯은 trivial이라 불필요. 생성자 필요하면 `TlsPoolNewDeleteMixin<T>` + `new`. |

### 16.2 미결 (구현 중 확인·결정)

| # | 항목 | 기본 선택 | 대안 |
|---|---|---|---|
| M1 | `Pop`에서 inbox가 비었을 때도 `Exchange` 하면 `lock xchg` 발생. | `if (inbox_.Load() != nullptr) pNode = inbox_.Exchange(nullptr);` 로 바꾼다. 벤치 P8 전후로 뱄교. | 유지 |
| M2 | `PoolAt`에 `[[likely]]` (C++20) | 프로젝트 표준이 C++20 이상이면 추가 | |
| M3 | `totalCount_/freeCount_` TSan 감지 | `Atomic<_u32>` + owner `Store` — `Interlocked`에선 비용이 있으므로 TSan 빌드에서만 `GetStats` 제거가 기본 | |
| M4 | 디버그 이중 Push 감지 | `#ifdef _DEBUG` 에서 `poolIdx_` 최상위 뱄트를 "프리" 마크로. `Push` 시 설정, `Pop` 시 해제, `Push` 시 설정되어 있으면 `jc_assert_msg`. `kMaxPoolCount` < 2^31 이므로 안전. 1차에 넣을지 결정 — **넣기 추천** (`ObjectPool`의 검사와 동급 기능). | |
| M5 | `Atomic::Increment` 반환값 의미 | P1에서 확인 | |
| M6 | `Pool` 할당에 `dbg_new` vs aligned new | `alignas(64)` 타입은 C++17 aligned `operator new`가 필요하고 `dbg_new`(`_NORMAL_BLOCK` 오버로드)와 혼용 불가. §6.6처럼 `::operator new(size, align_val_t)` + placement. 누수 검사(`JC_LEAK_CHECK_ASSERT`)에 잡힐 수 있으므로 Pool은 의도적 영구 객체라고 주석. | `Memory::Allocate` 가 정렬 보증하면 그것 |
| M7 | `String`의 TCHAR 바인딩 — **B안 채택 (2026-09-06 확정)**. `using String = BasicString<TCHAR, ...>`를 내부 기본으로 쓴다. 규칙은 한 줄이다: **`String`은 모듈 안에서, `AString`/`WString`은 모듈 밖에서.** (1) 모듈 내부의 일반 함수는 `const String&` 하나로 쓴다. A/W 구분 없음. 같은 TCHAR이므로 문제없다. (2) `StringUtil`은 CharT 템플릿이라 각 TU가 알아서 인스턴스화한다. 경계 이슈 없음. (3) 모듈 경계를 넘는 비템플릿 compiled 함수의 시그니처에만 명시 타입(`AString`/`WString`)을 쓴다. 두 인스턴스 간 암묵 변환이 없으므로 불일치는 컴파일 에러가 된다. (4) TCHAR 별칭은 내부 헤더에 격리하고 공개/경계 헤더는 include하지 않는다. (5) 경계 exported 클래스는 `static_assert(sizeof(X) == N)`으로 레이아웃 변경을 컴파일타임에 검출한다. (6) CI grep으로 공개 헤더의 단독 `String` 사용을 차단한다. 선수조건: 경계 헤더 식별 → `_T()` 전환(P12) → 문자셋 변경 순서를 지킨다. | B안 | A안(기각: 내부 코드 전체에 A/W 구분이 퍼짐) |

### 16.3 구현 기록 (2026-09-06, P0~P5·M12 완료)

- include 경로: `IsBaseOf_v`는 `jc/TypeTraits/Checker.h`가 아니라 `jc/TypeTraits.h`에 있다. 풀 헤더는 `jc/Macro.h`(JC_LIB_LOCK_GUARD의 JC_CONCAT_COUNTER), `<typeinfo>`(typeid), `<type_traits>`, `<cstddef>`를 명시 포함한다.
- `Aligner.h` 실제 교체 위치는 L13 + L40 매크로 2곳이다 (M2의 L13/L22/L40과 다름).
- TSan은 수행하지 않았다. MSVC에 TSan이 없고, 코드베이스가 Windows 전용이라 WSL clang 빌드도 불가하다. 대신 MSVC ASan 독립 하네스(`PoolAsan.cpp`, 풀 코드는 테스트 TU에서 계측, jc.lib 혼합 링크는 `_DISABLE_STRING/VECTOR_ANNOTATION`으로 해결)로 UAF/OOB 14회 PASS, 0건. 하네스가 잡은 1건은 하네스 자체의 이중 Push 버그였다.
- 세그먼트 분기(idx 256+)는 테스트 미커버다. 스레드 상한(논리 프로세서/2) 안에서는 257 동시 풀에 도달할 수 없다. 코드 리뷰로 커버한다.
- 테스트 스레드 상한: 300개 스핀 배리어가 8논리 머신 전 CPU를 점유해 멈춘 전례가 있다. 이후 모든 테스트는 프로세서/2 스레드 + 블로킹 동기화(TestGate CV, promise/future)만 쓴다. V8은 CrossPushStress로 교체했다.
- LeakCheck 운용 규칙 (CRT 델타 검사는 영구 객체를 못 걸러낸다): (1) Pool·Segment·고아 락은 워밍업에서 체크포인트 밖 생성, (2) 워커 수요보다 1개 많은 여유 고아 (없으면 워커가 CreatePool해서 델타에 걸림), (3) 테스트 끝 Trim은 RAII 가드로 — Storage 파괴 후·검사 전에 실행되어야 한다. (4) 워밍업 잔여물은 워밍업 끝에서 trim (체크포인트 이전 할당을 테스트 안에서 해제하면 음수 델타로 실패).
- V4 단언 교정: 고아 재사용 체제에서는 owner 종료 후 foreign Push의 고아 수 변동이 net 0이다 (빌려쓰고 반납). 문서 V13 V4의 `orphanPoolCount == 1`은 신규 생성 가정이므로 델타 단언(`after == before`)으로 교체했다.
- V7 단언을 절대값(1~2)이 아니라 델타(`poolCount` 불변)로 교체했다. 필터 단독 실행에서도 성립한다.
- Storage 세부 결정: `Reserve`는 정확히 맞춤 (증가 정책은 BasicString 몫). 빈 src 복사 대입은 `Clear()` (슬롯 유지). 이동 대입은 swap. 겹침 `Assign`은 임시 버퍼 경로 + 범위 assert.
- M1 (inbox `Load` 선행 검사)은 미적용. §6 코드대로 무조건 `Exchange` 유지. S8 벤치 전후 비교로 결정한다.
- natvis(`StringImpl_SSOCtxTlsPool.natvis`)는 작성만 했고 라이브 디버그 세션에서 미검증이다.
- 신규 파일은 BOM(EF BB BF)을 단다. write 도구 생성 파일은 BOM이 빠져서 별도 추가했다.
- P9 (SmartPtr 트레이트 일반화) 완료. 구현 중 중첩 Mixin의 CRTP 충돌을 발견했다. `struct Bar : TlsPoolNode, ObjectPoolTLSIndexed<Bar>::NewDeleteMixin`처럼 쓰면 파생 정의 중에 풀 본체가 불완전 타입으로 인스턴스화되어 클래스 영역 static_assert에 걸린다. 그래서 Mixin을 클래스 바깥 템플릿 `TlsPoolNewDeleteMixin<T>`로 정의했다 (이 철자로만 상속). 덤으로 트레이트가 풀 본체(정적 테이블/코드)를 타입마다 인스턴스화하지 않아서 바이너리도 가볍다. `IsPooledObject_v<T>`는 `SmartPtr.h` detail에 두고 `PreventCreatingObjectPoolItem` + `SharedObject<T*>` 소멸 분기에서 쓴다. `MakeShared<MixinType>` 차단은 네거티브 컴파일로 확인 (static_assert 발동). 전체 371/371.

---

## 17. 한계와 사용 규칙

| 한계 | 설명 | 규칙 |
|---|---|---|
| 전역/static `PooledString` | 정적 소멸 순서에서 `tls_Guard`가 먼저 소멸되면 그 뒤 소멸은 힙 경로로 가서 안전하지만, 정적 엄버 소멸 이후 접근 가능성이 있다. | **금지.** 전역은 `String`(풀 무관)을 쓴다. |
| `TerminateThread` | `~Guard` 안 돌아 번호와 객체 잔류. | 금기(기존에도). |
| DLL 경계 | 모듈별 `FastTable` 복사본. | jc는 static lib. DLL로 바꾸면 `JC_DLL` export 필요. |
| 솔벌림 급 잔류 | R3의 확율적 잔류는 `SweepOrphans()`까지 유지. 스레드가 더 생기지 않으면 프로세스 종료까지. | 스레드 종료가 많은 프로세스는 주기적으로 `TrimThisThread()`. |
| 가상 함수 T | `TlsPoolNode`가 offset 0이어야 하므로 불가. | `static_assert`로 자동 거부. |
| 풀 메모리는 줄지 않는다 | 프리리스트는 owner가 살아있는 한 자라마 최대값에 뱀물린다. | 필요 시 `TrimThisThread()`. 2차: 상한 넘으면 `Push`에서 힙으로. |

---

## 18. 개발 담당 AI에게 주는 지시 요약

1. §12 순서로만 진행한다. P2 헤더는 §5/§6 코드를 그대로 옥기고, 컴파일 오류마다 수정하지마 불변 조건 I1~I9와 연산 순서(§6.5 `alive_=1` 위치, §6.9 drain 2회, §9.4 `FreeHeapBuf` → `Push`)는 절대 바꾸지 않는다.
2. 기존 파일 수정은 §11.2 표의 항목에만 한정. §11.3 파일은 손대지 않는다.
3. 컨벤션은 §14. 헤더 바깥의 모든 함수 정의 앞에 88-슬래시 구분자.
4. 새 버그나 설계 모순을 발견하면 코드로 우회하지 말고 §16.2에 항목을 적고 보고. 그 단계에서 마게 즉쿠.
5. 각 단계 끝에 완료 조건(§12 "완료 조건" 열)을 그대로 확인하고 겁증 증거(테스트 로그, 벤치 표)를 남긴다.
6. P11/P12는 별도 확인 없이 착수하지 않는다 (§19 R4/R5).

---

## 19. 추가 요구사항 반영 (2026-09-06)

영향범위가 매우 큰 작업이므로 기반(jc)부터 의존성 역순으로 오류를 잡는다: **jc → jc_gbench/jc_gtest → sgcl** 순으로 단계별 빌드 통과. 각 경계를 넘는 시점에 확인을 받는다.

### R1. 기존 String 기반 테스트의 매크로화 (M9)

- `jc_gtest/Primitives/StringTest.cpp` 등 기존 케이스는 `String` 전용으로 작성되어 있다 (`TEST(StringTest, Compare)`처럼 타입 고정, 리터럴 `""` 직접 사용).
- 케이스 본문은 고치지 않고, 타입만 바꿔 `String`/`PooledString` 양쪽으로 실행하는 매크로를 추가한다. 예시 방향 (구현 시 확정):
  ```cpp
  // PooledStringTest.cpp 또는 공용 헤더
  #define JC_STRING_TEST(_name, _body)TEST(StringTest, _name) _body TEST(PooledStringTest, _name) _body
  // 또는 TYPED_TEST (testing::Types<String, PooledString>)
  ```
- 조건: `PooledString`이 `String`의 공개 인터페이스와 동등해야 매크로화가 가능하다. 인터페이스 차이가 생기면 그 차이를 §16에 기록하고 해당 케이스는 매크로 대상에서 제외한다.
- `StringTest.cpp` 수정은 매크로 감싸기(기계적)에 한정한다. 케이스 로직 변경 금지.

### R2. 벤치마크 추가 (M10 유지 + M12 신규)

- M10 (S8 String 벤치): 기존 계획 유지. `StrForBench` 인터페이스로 감싸 S0~S7과 동일 조건 실행.
- M12 (풀 단독 벤치, 신규): `jc_gbench/Pool/ObjectPoolTLSIndexedBench.cpp`. 측정 항목:

  | 벤치 | 조건 | 기대 | 실측 (Release/x64, 2026-09-06) |
  |---|---|---|---|
  | `Pop/Push` 단일 스레드 | `Dummy64`, N=10⁶ | S4 벤치 풀 부분과 동등 (~2ns/회) | 2.9ns/회. 부합 |
  | cross-thread 반납 | A 생성 → B Push → A Pop, N=10⁴ | §10.1 foreign 경로 (20~40ns/회) | 약 26ns/회 (스레드 생성/join 포함). 부합 |
  | inbox drain | B가 N개 Push 후 A가 일괄 Pop | Exchange 1회 + 체인 연결 | 약 24ns/회. 부합 |

  - Single 벤치는 `DoNotOptimize`+`ClobberMemory` 필수. 없으면 컴파일러가 핑퐁 루프를 소거해 0.6ns/쌍이라는 거짓 수치가 나온다.

### R3. Push/Pop 명명 (반영 완료)

- D12로 결정. 이 문서 전체의 `Acquire/Release`를 `Pop/Push`로 교체했다 (내부 `ReleaseCtx()`는 StringImpl 전용 이름으로 유지).
- 이후 작성되는 코드(Storage, 벤치, 테스트)는 `Pop/Push`만 사용한다.

### R4. Unicode 전환 대비 — String의 TCHAR 대응 (P11)

- 현황 (2026-09-06 실측): `jc::String`은 `char` 고정 클래스다. jc 소스에 `TCHAR`·`_T(` 사용 0건. 프로젝트 문자셋은 혼재다:

  | 문자셋 | 프로젝트 |
  |---|---|
  | MultiByte | jc, jcr, jdbr, jnet, jnetr_common, jnet_gtest, mcp, sgf, sgfr |
  | Unicode | jc_gbench, jc_gtest, jdb, jnetr_tcpclient, jnetr_tcpserver, jnetr_udpclient_a/b, jnet_gbench, mcpr, sg, sgcc, sgcl, sge, sgs, sgs_auth, sgs_center, sgs_game, sgs_lobby, sgs_query |

- 핵심 제약: jc는 **static lib + MultiByte**로 컴파일되므로, jc 바이너리 안의 TCHAR는 항상 `char`다. 헤더에서 `using String = BasicString<TCHAR, ...>`로 두면 하위 Unicode 프로젝트의 TU에서는 `wchar_t`로 보여 lib 경계에서 같은 이름·다른 레이아웃(ODR 위반) 위험이 있다.
- P11에서 미결 M7 (A안: `String`=char 고정 + 명시적 `WString` + API 경계 변환 / B안: TCHAR 바인딩 + lib 경계 계약)을 확정한다. **미확정 상태에서 P12 착수 금지.**
- 확정 내용 (2026-09-06): **B안 채택.** 내부는 `const String&` 하나로 쓰고(StringUtil은 CharT 템플릿), 경계(비템플릿 compiled 함수)에만 `AString`/`WString` 명시를 둔다. 상세 규칙은 미결 M7 참조.
- 어느 안이든 `_T()` 전환(R5)은 선수조건이다.

### R5. 문자열 선언부 `_T()` 전환 (P12, M13)

- 규모: jc 소스 276파일 중 문자열 리터럴 포함 261파일. 하위 프로젝트(sg/sgcl/sgs 계열 244파일 등)까지 포함하면 수백 파일이다.
- Unicode 프로젝트에서 `""` 리터럴이 컴파일 오류를 내는 것은 String이 TCHAR(=wchar_t)로 바인딩된 이후다. 즉 전환 순서는 **R4 확정 → `_T()` 전환 → 문자셋 변경**이며, 순서를 뒤집으면 중간 상태가 빌드되지 않는다.
- 작업 순서 (의존성 역순, 각 단계 빌드 통과 후 다음):
  1. jc 본체 (`Projects/jc/Sources`) — 리터럴 `_T()` 감싸기.
  2. jc_gbench / jc_gtest — jc 헤더를 쓰는 첫 소비자. 오류 수정.
  3. sgcl — 최종 소비자. 오류 수정.
- **AGENTS.md 제약에 따라 P12는 착수 전 별도 확인이 필수다** (5파일 이상 수정). 단계(1)→(2)→(3) 경계마다 빌드 증거를 남기고 다음 경계 진행을 확인받는다.
