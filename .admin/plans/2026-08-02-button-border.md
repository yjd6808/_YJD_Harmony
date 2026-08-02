# Plan: 버튼 border 미표시 조사/수정 (2026-08-02)

## 목표
- 사용자 리포트: "버튼에 border 적용 안 된 것 같다."
- flat 테마(engine-default)에서 버튼 테두리가 보이지 않는 원인을 규명하고 수정.

## 배경 (기존 조사)
- 버튼 텍스처는 160x44로 베이크, slice=(8,8,8,8)로 바인딩됨 — 인셋 정상.
- `UIGradientRasterizer::RenderBorder`(L132)가 geometryBorderWidth 만큼 테두리 그림.
  색은 metalHighlight/metalBorder/metalShadow 혼합 (flat 테마 gold.border=#A98A4A).
- `UITextureBaker::Bake`(L189): `scaled.geometryBorderWidth *= ss`(ss=4) — 스케일 정상.
- engine-default.json geometry.borderWidth=1 확인.
- UIStyleResolver::ApplyStateModifiers가 metal/borderWidth 정상 채움.
- 미결: 베이크 텍스처에 테두리가 실제 그려졌는지 미확인.

## 가설
- (a) 베이크 텍스처에 테두리가 실제로 없음 (렌더링/다운샘플 문제).
- (b) 테두리는 있으나 1px + 금색(#A98A4A)이 flat 배경에서 너무 희미해 인식 안 됨.
- (c) 테두리는 있으나 런타임 9-slice 표시 단계에서 사라짐.

## 수정 방향 (진단 우선)
1. 임시 진단 코드로 베이크된 버튼 텍스처의 엣지 픽셀을 로그로 출력
   → 테두리 유무/색상 확인.
2. 테두리 없으면: RenderBorder/다운샘플/스케일 로직 수정.
3. 테두리 있으면: flat 테마는 금색이 아닌 회색/단색 계열로 시각 조정.
   (경우에 따라 geometry.borderWidth 또는 metal.border 색 변경)
4. 빌드 후 로그/육안 검증.

## 검증 기준
- [ ] 버튼 베이크 텍스처 엣지에 테두리 픽셀 존재 (로그로 확인)
- [ ] 사용자 육안에서 테두리 인지 가능
- [ ] 빌드 EXIT=0, 기존 entries=80 유지

## 참고 파일
- UITextureBaker.cpp (Bake, DownsampleLanczos)
- UIGradientRasterizer.cpp (RenderBorder L132)
- UITextureBakeService.cpp (BuildTextureSet, supersample=4)
- UIStyleResolver.cpp (ApplyStateModifiers)
- engine-default.json (flat 테마)
