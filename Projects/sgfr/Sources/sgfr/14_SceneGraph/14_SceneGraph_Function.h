/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 11:20:00 AM
 * =====================
 * 14. 씬 그래프 - 보조 함수
 */

#pragma once

// 씬 그래프의 개념과 sgf/Cocos2d-x 차이를 콘솔에 출력한다. (학습용)
void PrintSceneGraphExplanation();

// 단색 원 텍스처를 CPU로 직접 만든다. (경계 부드러움 포함)
// 행성 스프라이트용 텍스처를 외부 파일 없이 준비하기 위한 함수다.
// @param _pDevice     : 그래픽 디바이스
// @param _pOutTexture : 결과 텍스처
// @param _size        : 텍스처 한 변 픽셀 수
// @param _color       : 원 색상
bool CreateCircleTexture(sgf::GraphicDevice* _pDevice, sgf::Texture* _pOutTexture, int _size, const jc::Color& _color);
