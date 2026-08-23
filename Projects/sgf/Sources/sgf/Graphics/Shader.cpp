/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:42:00 PM
 * =====================
 * HLSL 셰이더 래퍼 구현부 — 레거시 호환용
 */

#include "Core.h"
#include "sgf/Graphics/Shader.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "sgf/Graphics/GraphicContext.h"

#include <cstring>

NS_SGF_BEGIN

Shader::Shader() {}
Shader::~Shader() {}

bool Shader::CompileHlsl(
	const jc::String& _szSource,
	const jc::String& _szEntry,
	const jc::String& _szTarget,
	SgfComPtr<ID3DBlob>& _outBlob)
{
	UINT compileFlags = 0;
#if defined(_DEBUG)
	compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	SgfComPtr<ID3DBlob> pErrorBlob;
	HRESULT hr = D3DCompile(
		_szSource.Source(),
		_szSource.Length(),
		nullptr,
		nullptr,
		nullptr,
		_szEntry.Source(),
		_szTarget.Source(),
		compileFlags,
		0,
		_outBlob.GetAddressOf(),
		pErrorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		if (pErrorBlob != nullptr)
			OutputDebugStringA(static_cast<const char*>(pErrorBlob->GetBufferPointer()));
		return false;
	}
	return true;
}

bool Shader::CompileFromString(
	GraphicDevice* _pDevice,
	const jc::String& _szSource,
	VertexLayoutSpan _layout,
	const jc::String& _szVsEntry,
	const jc::String& _szPsEntry)
{
	ID3D11Device* pDevice = _pDevice->Device();
	pVertexShader_.Reset();
	pPixelShader_.Reset();
	pInputLayout_.Reset();
	SgfComPtr<ID3DBlob> pVsBlob;
	if (!CompileHlsl(_szSource, _szVsEntry, "vs_5_0", pVsBlob)) return false;
	HRESULT hr = pDevice->CreateVertexShader(pVsBlob->GetBufferPointer(), pVsBlob->GetBufferSize(), nullptr, pVertexShader_.GetAddressOf());
	if (FAILED(hr)) return false;
	SgfComPtr<ID3DBlob> pPsBlob;
	if (!CompileHlsl(_szSource, _szPsEntry, "ps_5_0", pPsBlob)) return false;
	hr = pDevice->CreatePixelShader(pPsBlob->GetBufferPointer(), pPsBlob->GetBufferSize(), nullptr, pPixelShader_.GetAddressOf());
	if (FAILED(hr)) return false;
	hr = pDevice->CreateInputLayout(_layout.Data(), _layout.Size(), pVsBlob->GetBufferPointer(), pVsBlob->GetBufferSize(), pInputLayout_.GetAddressOf());
	return SUCCEEDED(hr);
}

void Shader::Bind(GraphicContext& _context)
{
	ID3D11DeviceContext* pContext = _context.Raw();
	pContext->IASetInputLayout(pInputLayout_.Get());
	pContext->VSSetShader(pVertexShader_.Get(), nullptr, 0);
	pContext->PSSetShader(pPixelShader_.Get(), nullptr, 0);
}
void Shader::Bind(GraphicDevice* _pDevice)
{
	if (_pDevice == nullptr) return;
	Bind(_pDevice->Context());
}

NS_SGF_END
