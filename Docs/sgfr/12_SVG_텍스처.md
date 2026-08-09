# 12. SVG 텍스처 — 벡터 이미지를 래스터로 구워 그리기

> 관련 코드: `Projects/sgf/Sources/sgf/Graphics/Texture.h/.cpp`(LoadFromSvgFile), `Projects/sgf/Sources/sgf/_Extern/nanosvg/README.md`, `Projects/sgfr/Sources/sgfr/12_SvgTextureDraw/`
> 실행: sgfr 메뉴에서 **12번**

---

## 1. 래스터 vs 벡터 — 이미지의 두 종류

| | 래스터 (PNG, JPG) | 벡터 (SVG) |
|---|---|---|
| 저장 방식 | 픽셀 색 배열 | "원을 그려라", "선을 그어라" 같은 **그리기 명령서** |
| 확대하면 | 계단현상으로 깨짐 | 몇 배로 키워도 선명 |
| 용도 | 사진, 복잡한 그림 | 아이콘, UI, 로고 |

GPU는 픽셀 배열만 이해하므로, SVG는 사용 직전에 원하는 크기의 픽셀 배열로 변환(**래스터화**)해야 합니다.

> 핵심 장점: 같은 SVG 파일을 scale 1로 구우면 작고 선명하게, scale 4로 구우면 크고 선명하게! PNG를 4배 확대하면 흐릿해지는 것과 결정적 차이입니다.

---

## 2. nanosvg — 단일 헤더 SVG 래스터라이저

sgf는 [nanosvg](https://github.com/memononen/nanosvg)를 사용합니다. 헤더 파일 2개만 복사하면 되는 초경량 라이브러리입니다.

### 설치 방법 (수동 1분 컷)

`Projects/sgf/Sources/sgf/_Extern/nanosvg/README.md`에 안내되어 있듯이, GitHub에서 다음 두 파일을 받아 같은 폴더에 넣으면 됩니다.

- `nanosvg.h` — SVG 파싱 (XML → 곶선 목록)
- `nanosvgrast.h` — 래스터화 (곶선 → RGBA 픽셀)

> sgf의 `Texture.cpp`는 `__has_include`로 파일 존재를 감지합니다. 없으면 SVG 기능만 비활성화될 뿐 나머지 엔진은 정상 빌드됩니다. (12번 튜토리얼은 설치 안내를 출력)

---

## 3. LoadFromSvgFile의 내부 흐름

```cpp
texture.LoadFromSvgFile(&device, "icon.svg", 4.0f); // 4배 크기로 래스터화
```

내부에서 일어나는 일:

```
SVG 파일(텍스트/XML)
  → nsvgParseFromFile()  : 곶선(베지어 공선) 목록으로 파싱
  → nsvgRasterize()      : 원하는 scale로 픽셀 배열 생성 (안티앨리어싱 포함)
  → CreateFromMemory()   : 11번과 동일하게 GPU 텍스처 + SRV 생성
```

즉 **11번과의 차이는 "픽셀 배열을 어디서 얻느냐"뿐**이고, GPU 쪽 과정(텍스처 생성, UV, 샘플링, 블렌딩)은 완전히 동일합니다. 이걸 깨닫는 것이 이번 튜토리얼의 진짜 목표입니다.

### CreateFromMemory — 메모리 픽셀로 텍스처 만들기

```cpp
// 직접 계산한 RGBA 바이트 배열로도 텍스처를 만들 수 있다
texture.CreateFromMemory(&device, pixels, width, height);
```

절차적 텍스처(체커보드, 그라데이션, 노이즈), 폰트 아틀라스 등 활용처가 많습니다. 14번 튜토리얼의 원형 행성 텍스처도 이 함수로 만듭니다.

---

## 4. 튜토리얼(12번)에서 직접 확인할 것

1. nanosvg가 설치되어 있으면: 프로그램이 생성한 SVG(도형 조합)를 서로 다른 scale로 래스터화해 나란히 그립니다. 확대해도 선명한 쪽(큰 scale)과 계단현상 쪽(작은 scale을 크게 그린 것)을 비교하세요.
2. 설치되어 있지 않으면: 콘솔에 설치 안내가 출력되고 대체 텍스처(체커보드)로 동작합니다.

---

## 5. 정리

| 개념 | 한 줄 요약 |
|---|---|
| 벡터 이미지 | 그리기 명령서. 확대해도 안 깨짐 |
| 래스터화 | 명령서를 원하는 크기의 픽셀로 국는 과정 |
| nanosvg | 헤더 2개짜리 SVG 파서+래스터라이저 |
| CreateFromMemory | 메모리 픽셀 배열 → GPU 텍스처 |
| GPU 관점 | 일단 텍스처가 되면 PNG든 SVG든 차이 없음 |

다음 문서: **13. 입력 처리** — 키보드/마우스를 제대로 다루는 법을 배웁니다.
