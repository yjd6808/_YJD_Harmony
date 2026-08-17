/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:30:00 PM
 * =====================
 * 셰이더 계층 구현부
 */

#include "Core.h"
#include "sgf/Graphics/ShaderProgram.h"
#include "sgf/Graphics/GraphicDevice.h"

#include <cstdio>

NS_SGF_BEGIN

using namespace jc;

namespace
{
	//////////////////////////////////////////////////////////////////////////////////////////
	// HLSL 문자열을 바이트코드로 컴파일한다. 실패 시 오류를 디버그 출력창으로 보낸다.
	bool CompileHlsl(const char* _szSource, const char* _szEntry, const char* _szTarget, SgfComPtr<ID3DBlob>& _outBlob)
	{
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		SgfComPtr<ID3DBlob> pErrorBlob;
		HRESULT hr = D3DCompile(
			_szSource, strlen(_szSource),
			nullptr, nullptr, nullptr,
			_szEntry, _szTarget,
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
	String ReadTextFile(const char* _szFilePath)
	{
		FILE* pFile = nullptr;
		if (fopen_s(&pFile, _szFilePath, "rb") != 0 || pFile == nullptr)
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

//////////////////////////////////////////////////////////////////////////////////////////
VertexShader::VertexShader()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
VertexShader::~VertexShader()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool VertexShader::InitializeFromSource(GraphicDevice* _pDevice, const char* _szSource, const char* _szEntry)
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
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool VertexShader::InitializeFromFile(GraphicDevice* _pDevice, const char* _szFilePath, const char* _szEntry)
{
	String source = ReadTextFile(_szFilePath);
	if (source.Length() == 0)
	{
		return false;
	}

	if (!InitializeFromSource(_pDevice, source.Source(), _szEntry))
	{
		return false;
	}

	SetDebugName(_szFilePath);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void VertexShader::Finalize()
{
	pShader_.Reset();
	pBytecode_.Reset();
}

//////////////////////////////////////////////////////////////////////////////////////////
PixelShader::PixelShader()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
PixelShader::~PixelShader()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PixelShader::InitializeFromSource(GraphicDevice* _pDevice, const char* _szSource, const char* _szEntry)
{
	Finalize();

	SgfComPtr<ID3DBlob> pBytecode;
	if (!CompileHlsl(_szSource, _szEntry, "ps_5_0", pBytecode))
	{
		return false;
	}

	return SUCCEEDED(_pDevice->Device()->CreatePixelShader(
		pBytecode->GetBufferPointer(), pBytecode->GetBufferSize(), nullptr, pShader_.GetAddressOf()));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PixelShader::InitializeFromFile(GraphicDevice* _pDevice, const char* _szFilePath, const char* _szEntry)
{
	String source = ReadTextFile(_szFilePath);
	if (source.Length() == 0)
	{
		return false;
	}

	if (!InitializeFromSource(_pDevice, source.Source(), _szEntry))
	{
		return false;
	}

	SetDebugName(_szFilePath);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PixelShader::Finalize()
{
	pShader_.Reset();
}

//////////////////////////////////////////////////////////////////////////////////////////
InputLayout::InputLayout()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
InputLayout::~InputLayout()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool InputLayout::Initialize(GraphicDevice* _pDevice, const D3D11_INPUT_ELEMENT_DESC* _pDescs, UINT _count, VertexShader* _pVs)
{
	jc_assert_msg(_pVs != nullptr && _pVs->Bytecode() != nullptr, "입력 레이아웃은 컴파일된 버텍스 셰이더가 필요합니다.");

	Finalize();

	ID3DBlob* pBytecode = _pVs->Bytecode();
	return SUCCEEDED(_pDevice->Device()->CreateInputLayout(
		_pDescs, _count,
		pBytecode->GetBufferPointer(), pBytecode->GetBufferSize(),
		pLayout_.GetAddressOf()));
}

//////////////////////////////////////////////////////////////////////////////////////////
void InputLayout::Finalize()
{
	pLayout_.Reset();
}

NS_SGF_END
