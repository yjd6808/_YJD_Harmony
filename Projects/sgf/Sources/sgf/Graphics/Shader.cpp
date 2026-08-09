/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:42:00 PM
 * =====================
 * HLSL 셰이더 래퍼 구현부
 */

#include "Core.h"
#include "sgf/Graphics/Shader.h"
#include "sgf/Graphics/GraphicDevice.h"

#include <cstring>

NS_SGF_BEGIN

// 생성자
Shader::Shader()
{
}

// 소멸자 (ComPtr이 자동 Release)
Shader::~Shader()
{
}

// HLSL 문자열 -> 바이트코드 컴파일 헬퍼
// @param _szTarget : 컴파일 대상 프로파일. "vs_5_0"=DX11 버텍스, "ps_5_0"=DX11 픽셀
bool Shader::CompileHlsl(
	const char* _szSource,
	const char* _szEntry,
	const char* _szTarget,
	SgfComPtr<ID3DBlob>& _outBlob)
{
	// 디버그 빌드에서는 셰이더에 디버그 정보를 넣고 최적화를 끈다.
	// (RenderDoc 같은 도구로 셰이더를 디버깅할 수 있게 됨)
	UINT compileFlags = 0;
#if defined(_DEBUG)
	compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	SgfComPtr<ID3DBlob> pErrorBlob;
	HRESULT hr = D3DCompile(
		_szSource,					// 소스 코드
		strlen(_szSource),			// 소스 길이
		nullptr,					// 소스 이름 (오류 메시지용, 생략)
		nullptr,					// 매크로 정의 없음
		nullptr,					// include 처리기 없음 (문자열 컴파일이므로)
		_szEntry,					// 진입 함수 이름
		_szTarget,					// 프로파일
		compileFlags,
		0,
		_outBlob.GetAddressOf(),
		pErrorBlob.GetAddressOf());

	if (FAILED(hr))
	{
		// 컴파일 오류 메시지를 Visual Studio 출력창에 보여준다.
		if (pErrorBlob != nullptr)
		{
			OutputDebugStringA(static_cast<const char*>(pErrorBlob->GetBufferPointer()));
		}
		return false;
	}
	return true;
}

// HLSL 소스를 컴파일해서 VS/PS/입력레이아웃 생성
bool Shader::CompileFromString(
	GraphicDevice* _pDevice,
	const char* _szSource,
	const D3D11_INPUT_ELEMENT_DESC* _pLayoutDescs,
	UINT _layoutCount,
	const char* _szVsEntry,
	const char* _szPsEntry)
{
	ID3D11Device* pDevice = _pDevice->Device();

	// 재컴파일(핫 리로드) 대비: 기존 셰이더/레이아웃을 먼저 정리한다.
	// (GetAddressOf로 덮어쓰면 기존 객체가 누수된다)
	m_pVertexShader.Reset();
	m_pPixelShader.Reset();
	m_pInputLayout.Reset();

	// 1. 버텍스 셰이더 컴파일 + 생성
	SgfComPtr<ID3DBlob> pVsBlob;
	if (!CompileHlsl(_szSource, _szVsEntry, "vs_5_0", pVsBlob))
	{
		return false;
	}
	HRESULT hr = pDevice->CreateVertexShader(
		pVsBlob->GetBufferPointer(), pVsBlob->GetBufferSize(),
		nullptr, m_pVertexShader.GetAddressOf());
	if (FAILED(hr)) { return false; }

	// 2. 픽셀 셰이더 컴파일 + 생성
	SgfComPtr<ID3DBlob> pPsBlob;
	if (!CompileHlsl(_szSource, _szPsEntry, "ps_5_0", pPsBlob))
	{
		return false;
	}
	hr = pDevice->CreatePixelShader(
		pPsBlob->GetBufferPointer(), pPsBlob->GetBufferSize(),
		nullptr, m_pPixelShader.GetAddressOf());
	if (FAILED(hr)) { return false; }

	// 3. 입력 레이아웃 생성
	// VS 바이트코드를 같이 넘기는 이유: 구조체 설명과 VS 입력이 서로 맞는지
	// D3D가 교차 검증하기 위해서다. 안 맞으면 여기서 실패한다.
	hr = pDevice->CreateInputLayout(
		_pLayoutDescs, _layoutCount,
		pVsBlob->GetBufferPointer(), pVsBlob->GetBufferSize(),
		m_pInputLayout.GetAddressOf());
	return SUCCEEDED(hr);
}

// 셰이더와 입력레이아웃을 파이프라인에 장착
void Shader::Bind(GraphicDevice* _pDevice)
{
	ID3D11DeviceContext* pContext = _pDevice->Context();
	pContext->IASetInputLayout(m_pInputLayout.Get());			// IA 단계: 정점 해석 방법
	pContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);	// VS 단계
	pContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);	// PS 단계
}

NS_SGF_END
