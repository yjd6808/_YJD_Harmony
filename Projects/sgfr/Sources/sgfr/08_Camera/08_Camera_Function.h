/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:20:00 AM
 * =====================
 * 08. 카메라 좌표계 - 보조 함수/셰이더 소스
 */

#pragma once

// 변환 행렬 상수 버퍼를 사용하는 HLSL 셰이더 소스를 반환한다. (07번과 동일 구조)
const char* CameraShaderSource();

// 카메라(뷰 행렬)의 원리를 콘솔에 출력한다. (학습용)
void PrintCameraExplanation();
