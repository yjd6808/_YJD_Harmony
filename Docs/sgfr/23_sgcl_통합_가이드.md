# 23. sgcl 통합 가이드 — Cocos2d-x에서 sgf로 갈아타기

> 대상 독자: sgcl(Cocos2d-x 기반 게임 클라이언트)을 sgf 엔진으로 교체하려는 미래의 나
> 전제: 00~22번 문서를 모두 이해한 상태

---

## 1. 큰 그림: 무엇이 같고 무엇이 다른가

sgf는 처음부터 sgcl 교체를 염두에 두고 Cocos2d-x와 큰 구조를 맞춰 설계했습니다.

### 개념 대응표

| Cocos2d-x | sgf | 비고 |
|---|---|---|
| `AppDelegate` | `Application` | 상속해서 OnUpdate/OnRender 훅 사용 가능 |
| `Director::getInstance()` | `Director::Get()` | jc::SingletonStatic 기반 |
| `Scene` | `Scene` | OnEnter/OnExit/OnUpdate/OnRender |
| `Node` | `Node` | 계층 변환 동일 (07/14번) |
| `Sprite` | `Sprite` | 텍스처 붙은 Node |
| `Camera` | `Camera` | Scene이 보유 |
| `EventListener` | `jc::Event` 구독 | InputManager의 이벤트 (13번) |
| `runWithScene/replaceScene` | `RunScene/ReplaceScene` | 동일한 의미 |
| `scheduleUpdate` | `Scene::OnUpdate(_f32 _dt)` | 자동 호출됨 |

### 의도적으로 다르게 만든 것

| 항목 | Cocos2d-x | sgf | 이유 |
|---|---|---|---|
| 렌더링 경로 | addChild 필수 | 씬 그래프 **또는** OnRender 즉시 모드 | 사용자 요구사항. 디버그/이펙트에 유연 |
| 메모리 관리 | Ref 참조카운트 + autorelease | 명시적 소유 (AddChild가 소유권 인수) | 단순함 우선, 추적 쉬움 |
| 액션 시스템 | Action/Sequence/... | 없음. OnUpdate에서 직접 | 필요해지면 추가 구현 과제로 |
| 멀티플랫폼 | iOS/Android/... | Windows + DX11 전용 | 학습 목적 + 명확한 범위 |

---

## 2. 단계별 이전 전략

한 번에 갈아끼우려 하지 마세요. 추천 순서:

### 1단계: 앵커 타입 별칭 정리

sgcl 코드가 cocos2d 타입을 직접 쓰는 곳을 using/typedef로 모으세요.

```cpp
// 이전 준비 헤더 (예시)
using GameScene  = cocos2d::Scene;   // 나중에 sgf::Scene으로 교체
using GameNode   = cocos2d::Node;
using GameSprite = cocos2d::Sprite;
```

교체 지점이 한 파일로 모이면 이전 난이도가 극적으로 내려갑니다.

### 2단계: 입력/루프 경로 교체

AppDelegate → Application 상속타입으로 교체하고, EventListener 등록부를 InputManager 이벤트 구독으로 바꿉니다. (13번 문서)

### 3단계: 씬 단위 이식

가장 단순한 씬(로딩, 타이틀)부터 sgf::Scene으로 옮기며 패턴을 확립하세요.

- `init()` → `OnEnter()`
- `update(float)` → `OnUpdate(_f32)`
- `addChild` 계층은 그대로 옮김 (개념 동일)
- 좌표계 주의: Cocos2d-x는 왼쪽 아래 원점, sgf 기본 2D 카메라는 화면 중앙 원점입니다. `Camera::SetPosition2D`로 맞추거나 변환 함수를 한 겹 두세요 (14번 6장 공식)

### 4단계: 리소스 파이프라인

PNG는 `Texture::LoadFromFile`(11번), 아이콘/UI는 SVG(12번)로 점진 교체. 스프라이트 시트가 필요해지면 `Sprite`에 UV 영역 지정 기능을 추가하는 것이 첫 확장 과제가 될 것입니다.

---

## 3. 빌드 연결 방법

sgf는 jc와 동일한 빌드 체계를 따릅니다.

1. 솔루션에 `Projects/sgf/Project/sgf.vcxproj` 추가 (정적 라이브러리)
2. 사용하는 프로젝트(sgcl 등)에서 `Props/sgf.props` import
   - include 경로와 `sgf.lib;d3d11.lib;d3dcompiler.lib;dxgi.lib;ole32.lib` 링크가 자동 설정됨
3. 빌드 순서: jc → sgf → (sgfr | sgcl)
4. 코드에서는 `#include <sgf/SgfHeader.h>` 하나로 전체 사용 가능

sgfr의 `Project/sgfr.vcxproj`가 "sgf를 사용하는 응용 프로젝트" 설정의 살아있는 예제이므로, sgcl 연결 시 그대로 참고하면 됩니다.

---

## 4. 확장 로드맵 제안 (학습 과제)

난이도 순으로, sgf에 직접 추가해볼 만한 기능들:

1. **스프라이트 시트 UV** — Sprite에 소스 사각형 지정 (11번 UV 응용)
2. **텍스트 렌더링** — 폰트 아틀라스 텍스처 + 글자당 사각형 (12번 CreateFromMemory 응용)
3. **액션 시스템** — MoveTo/FadeIn 같은 시간 기반 보간 (01번 lerp + 13번 dt)
4. **렌더 타겟** — 화면을 텍스처로 그려 후처리 효과 (03번 RTV 응용)
5. **3D 메시 로딩** — OBJ 파서 + VertexPNT (09/10번 응용)

각 과제는 괄호 안의 기초 개념만 조합하면 됩니다. 여기까지 왔다면 이미 재료는 다 갖춘 셀입니다.

---

## 5. 마무리

이 시리즈의 목표는 "DX11 외우기"가 아니라 **그림 한 장이 화면에 나오기까지의 모든 단계를 스스로 설명할 수 있게 되는 것**이었습니다.

이제 다음 질문에 답할 수 있다면 성공입니다.

1. 스프라이트 하나가 화면에 나오기까지 거치는 파이프라인 단계는? (04번)
2. 부모 노드가 움직이면 자식이 따라 움직이는 수학적 이유는? (07/14번)
3. 투명 PNG가 제대로 그려지려면 무엇을 켜고 어떤 순서로 그려야 하는가? (09/11번)
4. 카메라 이동은 실제로 무엇을 움직이는 것인가? (08번)

행복한 엔진 개발 되세요! 🎉
