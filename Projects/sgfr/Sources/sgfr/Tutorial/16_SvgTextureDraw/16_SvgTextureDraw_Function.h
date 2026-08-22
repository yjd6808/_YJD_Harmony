/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 11:00:00 AM
 * =====================
 * 16. SVG 텍스처 그리기 - 보조 함수
 */

#pragma once

// 벡터 그래픽 vs 래스터 그래픽 차이를 콘솔에 출력한다. (학습용)
void PrintVectorVsRasterExplanation();

// 예제 SVG 파일(sample.svg, 실행 폴더 기준)이 없으면 생성해준다.
// 반환값: 파일이 존재하거나 생성에 성공하면 true
bool EnsureSampleSvgFile(const char* _szFilePath);

// nanosvg가 없을 때 대체용으로 그라데이션 원 텍스처를 직접 만든다.
// (SVG 래스터화 결과와 비슷한 부드러운 원을 CPU로 직접 그린다)
bool CreateFallbackCircleTexture(sgf::GraphicDevice* _pDevice, sgf::Texture* _pOutTexture);
