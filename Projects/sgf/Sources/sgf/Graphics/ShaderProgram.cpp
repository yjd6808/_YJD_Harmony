/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:30:00 PM
 * =====================
 * 셰이더 계층 구현부
 */

#include "Core.h"
#include "sgf/Graphics/ShaderProgram.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "sgf/Graphics/VertexDeclaration.h"
#include "jc/Hasher.h"

#include <cstdio>
#include <cstring>

NS_SGF_BEGIN

using namespace jc;

namespace
{
	//////////////////////////////////////////////////////////////////////////////////////////
	// HLSL 문자열을 바이트코드로 컴파일한다. 실패 시 오류를 디버그 출력창으로 보낸다.
	bool CompileHlsl(const jc::String& _szSource, const jc::String& _szEntry, const jc::String& _szTarget, SgfComPtr<ID3DBlob>& _outBlob)
	{
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		SgfComPtr<ID3DBlob> pErrorBlob;
		HRESULT hr = D3DCompile(
			_szSource.Source(), _szSource.Length(),
			nullptr, nullptr, nullptr,
			_szEntry.Source(), _szTarget.Source(),
			flags, 0,
			_outBlob.GetAddressOf(), pErrorBlob.GetAddressOf());

		if (FAILED(hr))
		{
			if (pErrorBlob != nullptr)
			{
				OutputDebugStringA(static_cast<const char*>(pErrorBlob->GetBufferPointer()));
			}
			return false;
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// 파일 전체를 읽어 String으로 반환한다. 실패 시 빈 문자열.
	// (std:: 금지 규칙에 따라 CRT + jc::String 사용. FR-26)
	String ReadTextFile(const jc::String& _szFilePath)
	{
		FILE* pFile = nullptr;
		if (fopen_s(&pFile, _szFilePath.Source(), "rb") != 0 || pFile == nullptr)
		{
			return String{};
		}

		fseek(pFile, 0, SEEK_END);
		const long size = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		if (size <= 0)
		{
			fclose(pFile);
			return String{};
		}

		// 널 종단 포함 버퍼를 직접 만든다.
		_s8* pBuffer = dbg_new _s8[size + 1];
		const size_t readSize = fread(pBuffer, 1, static_cast<size_t>(size), pFile);
		fclose(pFile);
		pBuffer[readSize] = '\0';

		String result(pBuffer);
		delete[] pBuffer;
		return result;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
VertexShader::VertexShader()
{
}

////////////////////////////////////////////////////////////////////////////////////////
VertexShader::~VertexShader()
{
	Finalize();
}

////////////////////////////////////////////////////////////////////////////////////////
bool VertexShader::InitializeFromSource(GraphicDevice* _pDevice, const jc::String& _szSource, const jc::String& _szEntry)
{
	Finalize();

	if (!CompileHlsl(_szSource, _szEntry, "vs_5_0", pBytecode_))
	{
		return false;
	}

	if (FAILED(_pDevice->Device()->CreateVertexShader(
		pBytecode_->GetBufferPointer(), pBytecode_->GetBufferSize(), nullptr, pShader_.GetAddressOf())))
	{
		pBytecode_.Reset();
		return false;
	}

	// 입력 시그니처 추출 — 실패 시 리소스 해제 후 실패 반환
	if (!_BuildInputSignature())
	{
		pShader_.Reset();
		pBytecode_.Reset();
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////
bool VertexShader::InitializeFromFile(GraphicDevice* _pDevice, const jc::String& _szFilePath, const jc::String& _szEntry)
{
	String source = ReadTextFile(_szFilePath);
	if (source.Length() == 0)
	{
		return false;
	}

	if (!InitializeFromSource(_pDevice, source, _szEntry))
	{
		return false;
	}

	SetDebugName(_szFilePath);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////
void VertexShader::Finalize()
{
	pShader_.Reset();
	pBytecode_.Reset();
	inputSignature_.Clear();
	signatureHash_ = 0;
}

////////////////////////////////////////////////////////////////////////////////////////
bool VertexShader::_BuildInputSignature()
{
	inputSignature_.Clear();
	signatureHash_ = 0;

	if (pBytecode_ == nullptr)
	{
		return false;
	}

	SgfComPtr<ID3D11ShaderReflection> pReflect;
	HRESULT hr = D3DReflect(pBytecode_->GetBufferPointer(), pBytecode_->GetBufferSize(),
		IID_PPV_ARGS(pReflect.GetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_SHADER_DESC desc = {};
	pReflect->GetDesc(&desc);

	jc::HashBuilder<jc::HashAlgorithm::Fnv1a64> builder;
	for (UINT i = 0; i < desc.InputParameters; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC param = {};
		pReflect->GetInputParameterDesc(i, &param);

		// SV_* 시스템 값은 IA 입력이 아니므로 제외
		if (param.SystemValueType != D3D_NAME_UNDEFINED)
		{
			continue;
		}

		SignatureElement element = {};
		strncpy_s(element.semanticName_, param.SemanticName, _TRUNCATE);
		element.semanticIndex_ = param.SemanticIndex;
		inputSignature_.PushBack(element);

		// 시맨틱 이름 + 인덱스로 해시 누적 — Hasher<->HashBuilder로 Fnv1a64 통일
		builder.AppendString(element.semanticName_, strlen(element.semanticName_));
		builder.AppendIntegral(element.semanticIndex_);

		// 추가 확산 (per-element, 기존 로직 유지)
		{
			_u64 h = builder.Digest();
			h ^= (h >> 33);
			h *= 0xff51afd7ed558ccdull;
			builder.Reset(h);
		}
	}
	signatureHash_ = builder.Digest();
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////
PixelShader::PixelShader()
{
}

////////////////////////////////////////////////////////////////////////////////////////
PixelShader::~PixelShader()
{
	Finalize();
}

////////////////////////////////////////////////////////////////////////////////////////
bool PixelShader::InitializeFromSource(GraphicDevice* _pDevice, const jc::String& _szSource, const jc::String& _szEntry)
{
	Finalize();

	SgfComPtr<ID3DBlob> pBytecode;
	jc::String psTarget = "ps_5_0";
	if (!CompileHlsl(_szSource, _szEntry, psTarget, pBytecode))
	{
		return false;
	}

	return SUCCEEDED(_pDevice->Device()->CreatePixelShader(
		pBytecode->GetBufferPointer(), pBytecode->GetBufferSize(), nullptr, pShader_.GetAddressOf()));
}

////////////////////////////////////////////////////////////////////////////////////////
bool PixelShader::InitializeFromFile(GraphicDevice* _pDevice, const jc::String& _szFilePath, const jc::String& _szEntry)
{
	String source = ReadTextFile(_szFilePath);
	if (source.Length() == 0)
	{
		return false;
	}

	if (!InitializeFromSource(_pDevice, source, _szEntry))
	{
		return false;
	}

	SetDebugName(_szFilePath);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////
void PixelShader::Finalize()
{
	pShader_.Reset();
}

NS_SGF_END
