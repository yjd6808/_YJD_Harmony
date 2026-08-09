# 03. D3D11이 제공하는 멀티스레드 도구들

> D3D11은 멀티스레드 렌더링을 위해 공식 도구를 이미 준비해두었습니다.
> 새로 발명할 필요 없이 규칙만 지키면 됩니다. 이 문서는 그 도구들의 사용법과 규칙을 정리합니다.

---

## 1. 가장 중요한 규칙 한 줄 요약

- `ID3D11Device` (생성 담당) : **여러 스레드에서 동시 호출해도 안전** (내부에 자물쇠 내장)
- `ID3D11DeviceContext` (명령 담당) : **절대 안전하지 않음**. 한 시점에 한 스레드만.

즉, “텍스처 만들기는 아무 스레드나 OK, 그리기 명령은 한 줄로 서야 한다”입니다.

sgf v2 기준으로 보면:
- `GraphicDevice::Device()`를 쓰는 코드(Texture::CreateFromMemory, 버퍼 생성 등) → 로딩 스레드로 빼도 됨
- `GraphicDevice::Context()`를 쓰는 코드(Renderer2D::Flush, BeginFrame/EndFrame 등) → 한 스레드 전담 필요

---

## 2. 도구 1: 지연 컨텍스트 (Deferred Context)

### 개념
즉시 컨텍스트의 “녹음기 버전”입니다. 명령을 내려도 GPU로 가지 않고 녹음(커맨드 리스트)만 됩니다.

```cpp
// 생성 (스레드마다 하나씩 만들 수 있다)
ID3D11DeviceContext* pDeferred = nullptr;
pDevice->CreateDeferredContext(0, &pDeferred);

// 워커 스레드: 보통 컨텍스트처럼 명령을 내린다 (녹음만 됨)
pDeferred->OMSetRenderTargets(...);
pDeferred->DrawIndexed(...);

// 녹음 완료 → 주문서 묶음 완성
ID3D11CommandList* pCommandList = nullptr;
pDeferred->FinishCommandList(FALSE, &pCommandList);

// 렌더 스레드(즉시 컨텍스트 소유자): 재생
pImmediate->ExecuteCommandList(pCommandList, FALSE);
pCommandList->Release();
```

### 주의점
1. 지연 컨텍스트는 “상태가 백지”에서 시작합니다. 렌더타겟/뷰포트 등을 매번 다시 설정해야 합니다.
2. 드라이버가 진짜 병렬 최적화를 해주는지는 제조사마다 다릅니다(D3D11의 유명한 한계).
   그래서 요즘 엔진은 D3D12/Vulkan으로 가지만, **구조 연습으로는 D3D11 방식이 훨씬 안전하고 쉽습니다.**
3. `Map(D3D11_MAP_WRITE_DISCARD)`는 지연 컨텍스트에서도 됩니다. 단, 같은 버퍼를 두 스레드가 동시에 Map하면 안 됩니다.

---

## 3. 도구 2: 질의 함수로 지원 여부 확인

```cpp
D3D11_FEATURE_DATA_THREADING threading = {};
pDevice->CheckFeatureSupport(D3D11_FEATURE_THREADING, &threading, sizeof(threading));
// threading.DriverConcurrentCreates : 리소스 병렬 생성을 드라이버가 진짜 병렬로 처리하는가
// threading.DriverCommandLists     : 커맨드 리스트를 드라이버가 네이티브로 지원하는가
```
둘 다 FALSE여도 동작은 합니다(런타임이 에뮬레이션). 다만 성능 이득이 줄어듭니다.

---

## 4. 도구 3: 우리가 직접 만드는 “렌더 큐” (더 중요!)

D3D 도구보다 중요한 것이 엔진 쪽 설계입니다. sgf처럼 2D 배치 렌더러를 쓰는 엔진은
굳이 지연 컨텍스트까지 가지 않아도, “그릴 목록(렌더 커맨드)”을 직접 정의해 큐로 주고받는 방식이 더 간단합니다.

```cpp
// 게임 로직이 만드는 “그림 주문서” - D3D 객체가 전혀 없다!
struct RenderCommand
{
    enum class Type { Sprite, Rect, Line, Circle };
    Type type;
    Texture* pTexture;   // 수명 관리 주의 (05 문서 참고)
    Vec2 center;
    Vec2 size;
    Color color;
    _f32 radian;
};
```

- 메인 스레드: `RenderCommand`를 벡터에 기록만 함 (D3D 호출 0번)
- 렌더 스레드: 벡터를 받아 지금의 `Renderer2D::DrawXXX`를 대신 호출

이 방식의 장점: D3D 지식이 필요 없고, 지연 컨텍스트 드라이버 복불복을 안 타며, 디버깅도 쉽습니다.
sgf의 1차 목표로 이 방식을 추천합니다. (04 문서의 단계 3)

---

## 5. 동기화 도구: Windows 기본기

jc 라이브러리에 스레드 유틸이 추가되기 전까지는 Win32 API를 직접 쓰거나 jc에 래핑을 추가합니다.

| 도구 | 용도 | 비유 |
|---|---|---|
| `CreateThread` / `std::thread` 금지 → jc 래퍼 권장 | 일꾼 고용 | 채용 공고 |
| `CRITICAL_SECTION` | 짧은 임계 영역 잠금 | 화장실 열쇠 |
| `CONDITION_VARIABLE` | “새 주문 있을 때까지 재워두기” | 벨 누르면 깨워주는 벨보이 |
| `Interlocked*` 계열 | 카운터/플래그 원자적 조작 | 끊기지 않는 한 방 |
| `SetEvent`/`WaitForSingleObject` | 프레임 경계 동기화 | 출발 신호총 |

## 6. 다음 문서

`04_StepByStepMigrationPlan.md`에서 이 도구들로 sgf를 실제로 개조하는 5단계 계획을 제시합니다.
