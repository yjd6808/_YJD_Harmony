/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:50:00 AM
 * =====================
 * 15. PNG 텍스처 그리기 - 보조 함수/셰이더 소스
 */

#pragma once

// UV 좌표계의 원리를 콘솔에 출력한다. (학습용)
void PrintUvExplanation();

// 체커보드 무늬 픽셀(RGBA)을 직접 만들어 텍스처를 생성한다.
// PNG 파일이 없을 때도 튜토리얼이 항상 동작하도록 하는 대체 경로다.
bool CreateCheckerboardTexture(sgf::GraphicDevice* _pDevice, sgf::Texture* _pOutTexture);
