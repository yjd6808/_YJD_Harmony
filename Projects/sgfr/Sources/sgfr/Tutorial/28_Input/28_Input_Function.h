/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 11:10:00 AM
 * =====================
 * 28. 입력 처리 - 보조 함수/셰이더 소스
 */

#pragma once

// 색상 정점을 그대로 통과시키는 HLSL 셰이더 소스를 반환한다. (05번과 동일 구조)
const char* InputDemoShaderSource();

// 폴링과 이벤트 방식의 차이를 콘솔에 출력한다. (학습용)
void PrintInputModelExplanation();

// 픽셀 좌표(왼쪽위 기준)를 NDC 좌표(-1~+1, y 위쪽 +)로 변환한다.
// 마우스 좌표를 화면에 그리려면 반드시 필요한 변환이다!
jc::vec2 PixelToNdc(int _pixelX, int _pixelY, int _screenWidth, int _screenHeight);
