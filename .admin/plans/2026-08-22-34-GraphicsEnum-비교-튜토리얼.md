# 34 GraphicsEnum 한눈에 비교 튜토리얼 플랜 (2026-08-22)

## 목표
sgfr 34번 튜토리얼 34_GraphicsEnum_한눈에_비교 / 34_GraphicsEnumDiff_Main 생성.
GraphicsEnums.h의 상태별 차이를 좌(Before=디폴트) / 우(After=선택 enum) 분할 뷰로 시각 비교.
방향키 좌/우로 After 파이프라인 상태를 실시간 교체, 콘솔에 각 enum별 설명 출력.

## 설계
- 카테고리: BlendMode(4), FilterMode(3), AddressMode(4), CullMode(3), FillMode(2), DepthMode(3), FrontFace(2), Topology(5) = 8개
- 입력: Up/Down 또는 1~8로 카테고리 전환, Left/Right로 값 순회, ESC 종료
- 렌더: Window+GraphicDevice 레거시 경로(BeginFrame/EndFrame), 상태는 device.SetBlendMode/SetSampler/SetCullMode/SetWireframe/SetDepthTest / context IASetPrimitiveTopology 로 직접 교체
- Before 고정 = 엔진 디폴트 (bmAlpha 대 bmNone 등 문서에 명시), After = 선택값
- 시각 자원:
  - Blend: 반투명 3원(소프트서클 텍스처) 겹침
  - Filter/Address: 32x32 체커 텍스처 확대+UV 0..3 분할 셰이더(gSplitPixelX)로 s0/s1 비교
  - Cull/Fill/FrontFace/Depth: 색큐브 + 큐브 2개 나란히(좌 기준, 우 선택), Depth는 Z 겹침
  - Topology: 포인트/라인/트라이앵글용 정점 버퍼

## 파일
- Sources/sgfr/Tutorial/34_GraphicsEnum_한눈에_비교/34_GraphicsEnumDiff_Main.h
- Sources/sgfr/Tutorial/34_GraphicsEnum_한눈에_비교/34_GraphicsEnumDiff_Main.cpp
- TutorialRegistry.cpp 배열 추가 + include
- Project/sgfr.vcxproj, sgfr.vcxproj.filters 필터/컴파일 항목 추가

## 검증
- 빌드: Scripts\BuildProject\sgfr.bat -C Debug -P x64
- 실행 눈확인: 좌 고정 / 우 가변, 좌우키 반영, 타이틀/콘솔 가이드 출력
