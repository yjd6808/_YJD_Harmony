# 09. 정점 선언과 InputLayout 해시 캐시 — 몸과 옷의 맞춤 재봉

> **이 문서 하나로**: 정점 구조(VertexPC/PTC/PNT)가 어떻게 한 곳에서 선언되는지, InputLayout이 왜 Draw 직전에야 만들어지는지(지연 결합), 그리고 그 비용을 해시 캐시가 어떻게 0으로 만드는지를 실제 코드와 함께 이해합니다. (2026-08-23 재설계 반영)
>
> 관련 소스: `Graphics/VertexDeclaration.h`, `Graphics/Vertex.h`, `Graphics/GraphicDevice.h`, `Graphics/GraphicContext.h`

---

## 1. 문제의 출발 — 같은 정보를 세 번 적던 시절

"정점 하나에 무엇이 들어있는가(위치? 색? UV?)"라는 정보는 원래 세 곳에 흩어져 있었습니다.

```cpp
// Graphics/VertexDeclaration.h
/*
 * [왜 만드는가?]
 * 정점 포맷 정보가 세 곳에 중복되어 있었다.
 * 1. C++ 구조체 (VertexPC 등)
 * 2. HLSL 입력 구조체 (struct VSInput)
 * 3. D3D11_INPUT_ELEMENT_DESC 배열 (InputLayout 생성용)
 * 하나를 고치면 셈을 같이 고쳐야 하고, 깜빡하면 화면이 깨진다.
 *
 * VertexDeclaration은 이 중 3번을 "정점 구조체 옆 한 줄 선언"으로 통합한다.
 * (1은 C++이므로 남지만, 2와 3의 불일치는 런타임 검증으로 잡는다)
 */
```

이제 정점 구조체는 자기 설명서를 들고 다닙니다. 실제 선언은 이렇게 생겼습니다.

```cpp
// Graphics/Vertex.h — 위치+색 정점 (디버그 도형용)
struct VertexPC
{
	vec3 position_;
	color color_;

	static const VertexDeclaration* Decl()
	{
		static const VertexDeclaration* s_pDecl = VertexDeclaration::GetOrCreate({
			{ VertexSemantic::Position, VertexElementFormat::Float3 },
			{ VertexSemantic::Color,    VertexElementFormat::Color32 },
		});
		return s_pDecl;
	}
};
```

읽는 법: "VertexPC는 Position을 float 3개로, Color를 32비트로 가진다"를 구조체 바로 옆에서 선언합니다. 오프셋/스트라이드 계산은 VertexDeclaration이 자동으로 합니다. 사용 중인 정점 3종은:

| 정점 | 구성 | 용도 |
|---|---|---|
| **VertexPC** | Position + Color | 디버그 도형 (3D 그리드/축/큐브) |
| **VertexPTC** | Position + TexCoord + Color | 2D 전반 (도형/스프라이트) |
| **VertexPNT** | Position + Normal + TexCoord | 3D 메시 (조명 계산에 법선 필요) |

---

## 2. 인턴(interning) — 같은 선언은 세상에 하나뿐

`GetOrCreate`라는 이름이 힌트입니다. 같은 구성의 선언을 두 번 요청하면 **같은 포인터**가 돌아옵니다(문자열 인턴과 같은 원리). 덕분에:

- "이 두 메시의 정점 형식이 같은가?"가 포인터 비교 한 번으로 끝납니다.
- 선언 객체가 전역에 딱 한 벌씩만 존재해 메모리도 절약됩니다.
- 뒤에 나올 캐시 키(declHash)도 안정적으로 만들 수 있습니다.

---

## 3. InputLayout — 몸과 옷을 잇는 맞춤 재봉

DX11의 InputLayout은 "정점 버퍼의 메모리 배치(몸)"와 "버텍스 셰이더의 입력 시그니처(옷)"를 이어주는 객체입니다. 핵심은 **둘의 조합**으로 결정된다는 점입니다.

- 같은 메시를 다른 셰이더로 그리면 → 다른 InputLayout이 필요할 수 있다
- 다른 메시라도 정점 선언이 같고 셰이더 입력이 같으면 → 같은 InputLayout을 쓸 수 있다

그래서 "메시가 InputLayout을 소유한다"(예전 구조)는 어색합니다. 메시는 옷의 사정을 모릅니다. 2026-08-23 재설계에서 이 소유권을 엔진 중앙으로 옮겼습니다.

```cpp
// Graphics/GraphicContext.h
/*
 * [InputLayout 지연 결합]
 * IASetVertexDecl(decl) + VSSetShader(shader)만 기록해두고,
 * Draw 직전에 (decl, shader 시그니처) 조합으로 InputLayout을 찾는다.
 * - 메시는 이제 셰이더를 몰라도 된다 (재사용성)
 * - 실제 객체는 GraphicDevice의 중앙 캐시가 (declHash, sigHash) 키로 보관
 * - 같은 조합은 한 번만 생성되고 이후 재사용된다
 */
```

흐름을 그림으로 보면:

```
Mesh::Bind
  └─ context.IASetVertexDecl(pDecl)     "내 몸은 이렇게 생겼다"만 기록
Material 바인드
  └─ context.VSSetShader(pShader)       "이 옷을 입는다"만 기록
context.Draw(...)
  └─ _ResolveInputLayout()              그때야 둘의 조합을 확정
       ├─ key = (decl 해시, 셰이더 입력 시그니처 해시)
       ├─ GraphicDevice 캐시에 있으면 → 재사용 (생성 비용 0)
       └─ 없으면 → CreateInputLayout 1회 후 캐시 등록
```

왜 "지연"이 중요할까요? 바인드 순서에 자유가 생깁니다. 메시를 먼저 묶든 셰이더를 먼저 묶든, Draw 직전에 둘 다 확정되어 있기만 하면 됩니다. 또 같은 메시를 연속으로 다른 셰이더로 그리는 경우(그림자 패스 등)에도 메시 쪽 코드는 아무 것도 몰라도 됩니다.

---

## 4. 비용 분석 — 매 Draw마다 해시를 계산하는 건 안 느린가?

아닙니다. 각 단계의 비용을 분해하면:

1. **decl 해시**: VertexDeclaration은 인턴되어 있으므로 생성 시점에 해시를 미리 계산해 보관합니다. 조회는 멤버 읽기 1번.
2. **셰이더 시그니처 해시**: 셰이더 컴파일 시점에 1회 계산해 Shader 객체에 보관.
3. **캐시 조회**: (declHash, sigHash) 키로 해시맵 조회 1번.
4. **바인딩 캐시**: 직전 Draw와 같은 InputLayout이면 IASetInputLayout 호출 자체를 생략. (GraphicContext의 일반 바인딩 캐시와 같은 원리)

즉 정상 경로의 비용은 "정수 두 개로 해시맵 한 번 찾기"이고, 실제 CreateInputLayout(비싼 작업)은 조합당 평생 1번입니다. 일반적인 게임에서 조합 수는 기껏해야 수십 개이므로 캐시는 작게 유지됩니다.

> **관찰 팁**: GraphicContext는 `GetApiCallCount()` / `GetSkippedCallCount()` 통계를 제공합니다. 한 프레임에 실제 DX11 API가 몇 번 불렸고 캐시가 몇 번 아꼈는지 숨자로 확인할 수 있습니다. (sgfr 23장 스테이트 캐시 편과 같은 주제)

---

## 5. 재설계 전후 비교 — 무엇이 좋아졌나

| 항목 | 재설계 전 | 재설계 후 (2026-08-23) |
|---|---|---|
| InputLayout 소유 | Mesh가 소유 (VS와 짝지어 생성) | GraphicDevice 중앙 캐시 |
| Mesh와 셰이더 관계 | Mesh가 VS를 알아야 함 (결합) | Mesh는 셰이더를 모름 (분리) |
| 같은 decl+셰이더 조합 | 메시마다 중복 생성 가능 | 전역 1개 재사용 |
| 메시 하나를 여러 셰이더로 | 메시 복제 필요 | 그냥 됨 (Draw 시점 resolve) |

핵심을 한 줄로 줄이면: **"몸(메시)과 옷(셰이더)은 서로를 몰라야 하고, 맞춤 재봉(InputLayout)은 입는 순간 필요하면 만들되 한 번 만든 건 옷장(캐시)에 걸어둔다"**입니다.

---

## 6. 새 정점 타입을 추가하려면 (실전 가이드)

예를 들어 스키닝용 VertexPNTW(가중치 추가)가 필요하다면:

1. `Vertex.h`에 구조체를 추가하고 같은 패턴으로 `Decl()`을 작성합니다. (VertexSemantic에 없는 의미라면 enum에 추가)
2. 셰이더 HLSL의 VSInput을 같은 구성으로 작성합니다.
3. 끝. InputLayout은 첫 Draw 때 자동 생성됩니다. 구성이 어긋나면 런타임 검증이 즉시 알려줍니다.

예전 구조라면 D3D11_INPUT_ELEMENT_DESC 배열 작성 + 메시/셰이더 짝 관리까지 해야 했던 일입니다.

---

## 7. 요약 — 기억할 다섯 문장

1. 정점 포맷 정보는 원래 C++/HLSL/DESC 배열 세 곳에 중복됐고, VertexDeclaration이 이를 **구조체 옆 한 줄 선언**으로 통합했다.
2. 같은 구성의 선언은 **인턴**되어 전역에 하나뿐이다 — 비교는 포인터 비교로 끝난다.
3. InputLayout은 **메시의 decl × 셰이더의 입력 시그니처** 조합으로 결정되므로 어느 한쪽이 소유하면 안 된다.
4. 그래서 sgf는 **Draw 직전 지연 결합** + **(declHash, sigHash) 중앙 캐시**로 풀었다 — 조합당 생성은 평생 1번, 이후는 재사용.
5. 새 정점 타입 추가는 구조체 + Decl() + HLSL VSInput만 맞추면 끝 — InputLayout은 자동으로 따라온다.
