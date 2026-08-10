/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:50:00 PM
 * =====================
 * 30. 스프라이트 애니메이션 - 보조 함수/셰이더 소스
 */

#pragma once

// 스프라이트 시트/프레임 애니메이션/이징의 원리를 콘솔에 출력한다. (학습용)
void PrintSpriteAnimationExplanation();

// 256x256 RGBA 버퍼에 4x4 = 16프레임짜리 스프라이트 시트를 절차적으로 그려 채운다.
// 각 프레임(64x64)에는 크기가 맥동이고 색이 변하는 원 + 회전하는 하이라이트 점을 그린다.
// @param _pOutRgba : 256*256*4 바이트 출력 버퍼
void FillSpriteSheetPixels(_u8* _pOutRgba);

// 스프라이트 사각형 정점 4개를 채운다. (동적 버퍼용, 매 프레임 갱신)
// @param _center   : NDC 중심 위치
// @param _halfSize : NDC 반 크기
// @param _uvOffset : 시트에서 이 프레임이 시작하는 UV (왕위 기준)
// @param _uvScale  : 프레임 하나가 차지하는 UV 비율 (4x4 시트라면 0.25)
void FillSpriteQuad(sgf::VertexPTC* _pOutVertices4, const jc::vec2& _center, const jc::vec2& _halfSize,
	const jc::vec2& _uvOffset, const jc::vec2& _uvScale);

// 진행도 t(0~1)에 이진(Easing) 함수를 적용해 보간 공식용 진행도로 변환한다.
// @param _t    : 0~1 선형 진행도
// @param _mode : 0=선형, 1=부드럽게(ease-in-out), 2=바운스
_f32 ApplyEasing(_f32 _t, _s32 _mode);
