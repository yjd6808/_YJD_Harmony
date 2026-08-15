# 2026-08-15-29-scene-blank-fix.md

## 작업 이름
29번 씬(SceneGraph) 화면 안 나오는 문제 수정

## 증상
- sgfr 29번(씬 & 사운드) 실행 시 게임 창 클라이언트 영역 99.99%가 클리어 컬러만 보임
- 게임 창 제목이 "2"로 비정상 (정상: "29. 씬 & 사운드 - SPACE로 씬 교체...")
- 픽셀 분석 결과: 씬 내용물(태양/지구/달/궤도)이 전혀 렌더링되지 않음

## 확인된 사실
- exe는 최신 소스 반영 (08-15 13:25 빌드)
- Camera::SetOrthographic2D / Rebuild2D / jc::mat4::OrthographicOffCenterLH 수학은
  행벡터(row-major, v' = v*M) 규약과 일치 (정상)
- Director::Render → Renderer2D::Begin(viewProjection) → DrawSprite의 경로는 정상
- ConstantBuffer는 raw memcpy · HLSL row_major 선언과 일치 (정상)
- Renderer2D::Flush의 드로우 콜 경로 정상 (뷰포트/RT 바인딩 정상)

## 미검증 (원인 후보)
1. OnEnter/OnRender가 실제로 호출되는가?
2. 카메라 행렬 값 자체가 이상한가?
3. 텍스처 생성 실패로 DrawSprite가 무시되고 DrawLine만 있는데도 안 보이는가?
   (DrawLine은 흰색 텍스처 → pWhiteTexture_ 생성 실패 시 안 그려짐)
4. InputManager 핸들링 또는 게임 루프 자체가 도는가?

## 진단 방법
29_SceneGraph_Main.cpp에 임시 로그 계측 추가:
- OnEnter: 매트릭스 덤프 + 텍스처 생성 결과
- OnRender 첫 N프레임: 카메라 VP 행렬 + DrawSprite/DrawLine 호출 여부
- 파일 로그 출력 (콘솔 미의존)

## 수정 방향
계측 결과로 원인 확정 후:
- 프레임워크 버그면 sgf 쪽 수정
- 튜토리얼 코드 버그면 29_SceneGraph 수정
- 파일 5개 미만 수정 원칙 준수

## 검증
- 빌드(Debug x64) 후 실행 → 픽셀 분석으로 태양(노랑)/지구(파랑)/달(회색) 존재 확인
- 계측 코드 제거 후 최종 빌드