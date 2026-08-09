/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:42:00 PM
 * =====================
 * HLSL 셰이더 래퍼
 *
 * [셰이더란?]
 *  GPU에서 실행되는 작은 프로그램. DX11에서는 HLSL 언어로 작성한다.
 *  - 버텍스 셰이더(VS): 정점마다 1번 실행. 정점 위치를 화면 좌표로 변환.
 *  - 픽셀 셰이더(PS) : 픽셀마다 1번 실행. 최종 색상을 결정.
 *
 * [입력 레이아웃(Input Layout)이란?]
 *  C++의 정점 구조체(메모리 덩어리)와 VS의 입력 변수를 연결하는 "설명서".
 *  "구조체의 0바이트부터 12바이트는 POSITION, 그 다음 16바이트는 COLOR"
 *  처럼 메모리 배치를 GPU에게 알려줘야 정점 데이터를 올바로 해석한다.
 */

#pragma once

#include "jc/Math.h"

NS_SGF_BEGIN

using namespace jc;

class GraphicDevice;

class Shader
{
public:
	Shader();
	~Shader();

	// HLSL 소스 문자열을 컴파일해서 VS/PS/입력레이아웃을 만든다.
	// @param _pDevice      : 그래픽 디바이스
	// @param _szSource     : HLSL 소스 코드 문자열 (VS/PS가 함께 들어있음)
	// @param _pLayoutDescs : 정점 구조 설명 배열
	// @param _layoutCount  : 설명 배열 개수
	// @param _szVsEntry    : VS 진입 함수 이름 (기본 "VSMain")
	// @param _szPsEntry    : PS 진입 함수 이름 (기본 "PSMain")
	// @return 성공 여부 (실패 시 컴파일 오류가 디버그 출력창에 출력됨)
	bool CompileFromString(
		GraphicDevice* _pDevice,
		const char* _szSource,
		const D3D11_INPUT_ELEMENT_DESC* _pLayoutDescs,
		UINT _layoutCount,
		const char* _szVsEntry = "VSMain",
		const char* _szPsEntry = "PSMain");

	// 이 셰이더를 파이프라인에 장착한다. 그리기 직전에 호출.
	void Bind(GraphicDevice* _pDevice);

private:
	// HLSL 문자열을 바이트코드로 컴파일하는 내부 헬퍼
	// 실패 시 오류 메시지를 OutputDebugString으로 출력한다.
	static bool CompileHlsl(
		const char* _szSource,
		const char* _szEntry,
		const char* _szTarget,
		SgfComPtr<ID3DBlob>& _outBlob);

private:
	SgfComPtr<ID3D11VertexShader> m_pVertexShader;	// 정점 셰이더
	SgfComPtr<ID3D11PixelShader> m_pPixelShader;	// 픽셀 셰이더
	SgfComPtr<ID3D11InputLayout> m_pInputLayout;	// 정점 구조 설명서
};

NS_SGF_END
