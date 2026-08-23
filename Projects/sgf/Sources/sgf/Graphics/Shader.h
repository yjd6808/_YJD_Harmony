/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:42:00 PM
 * =====================
 * HLSL 셰이더 래퍼 — 레거시 호환용 (BatchRenderer는 핸들 분리형으로 이전)
 *
 * [셰이더란?]
 * GPU에서 실행되는 작은 프로그램. DX11에서는 HLSL 언어로 작성한다.
 * - 버텍스 셰이더(VS): 정점마다 1번 실행. 정점 위치를 화면 좌표로 변환.
 * - 픽셀 셰이더(PS): 픽셀마다 1번 실행. 최종 색상을 결정.
 *
 * [입력 레이아웃(Input Layout)이란?]
 * C++의 정점 구조체(메모리 덩어리)와 VS의 입력 변수를 연결하는 "설명서".
 */

#pragma once

#include "jc/Math.h"
#include "sgf/Graphics/Vertex.h"

NS_SGF_BEGIN

using namespace jc;

class GraphicDevice;
class GraphicContext;

class Shader
{
public:
	Shader();
	~Shader();

	// HLSL 소스 문자열을 컴파일해서 VS/PS/입력레이아웃을 만든다.
	bool CompileFromString(
		GraphicDevice* _pDevice,
		const jc::String& _szSource,
		VertexLayoutSpan _layout,
		const jc::String& _szVsEntry = "VSMain",
		const jc::String& _szPsEntry = "PSMain");

	void Bind(GraphicContext& _context);
	void Bind(GraphicDevice* _pDevice);

private:
	static bool CompileHlsl(
		const jc::String& _szSource,
		const jc::String& _szEntry,
		const jc::String& _szTarget,
		SgfComPtr<ID3DBlob>& _outBlob);

private:
	SgfComPtr<ID3D11VertexShader> pVertexShader_;
	SgfComPtr<ID3D11PixelShader> pPixelShader_;
	SgfComPtr<ID3D11InputLayout> pInputLayout_;
};

NS_SGF_END
