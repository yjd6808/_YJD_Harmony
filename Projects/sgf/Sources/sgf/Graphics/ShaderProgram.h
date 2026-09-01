/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:30:00 PM
 * =====================
 * 셰이더 계층: VertexShader / PixelShader
 *
 * [왜 VS가 바이트코드를 들고 있나?]
 * 입력 레이아웃을 만들려면 "이 VS의 입력 서명과 맞는지" 검증이 필요해서
 * D3D11이 VS 바이트코드를 요구한다. 그래서 VertexShader가 blob을 보관한다.
 *
 * [입력 시그니처]
 * VertexShader는 D3DReflect로 입력 시그니처를 추출해 보관한다.
 * (선언 × 시그니처) 조합으로 InputLayout을 캐시하는 데 사용된다.
 */

#pragma once

#include "sgf/Graphics/IResource.h"
#include "sgf/Graphics/GraphicsEnums.h"
#include "jc/Container/Vector.h"

NS_SGF_BEGIN

using namespace jc;

class GraphicDevice;

// 버텍스 셰이더 (정점마다 1번 실행. 위치 변환 담당)
class VertexShader : public ResourceBase
{
	SGF_RESOURCE_TYPE(rtVertexShader)

public:
	// VS 입력 시그니처 요소 — 리플렉션으로 1회 추출 (SV_* 시스템 값 제외)
	struct SignatureElement
	{
		char semanticName_[20] = {};	// "POSITION", "TEXCOORD" ...
		_u32 semanticIndex_ = 0;
	};

public:
	VertexShader();
	~VertexShader();

	// HLSL 소스 문자열을 컴파일해서 VS를 만든다.
	bool InitializeFromSource(GraphicDevice* _pDevice, const jc::String& _szSource, const jc::String& _szEntry = "VSMain");

	// HLSL 파일을 읽어서 컴파일한다.
	bool InitializeFromFile(GraphicDevice* _pDevice, const jc::String& _szFilePath, const jc::String& _szEntry = "VSMain");

	void Finalize();

	bool IsValid() const { return pShader_ != nullptr; }
	ID3D11VertexShader* Raw() const { return pShader_.Get(); }

	// 입력 레이아웃 생성용 바이트코드
	ID3DBlob* Bytecode() const { return pBytecode_.Get(); }

	const jc::Vector<SignatureElement>& InputSignature() const { return inputSignature_; }
	_u64 InputSignatureHash() const { return signatureHash_; }

private:
	bool _BuildInputSignature();

private:
	SgfComPtr<ID3D11VertexShader> pShader_;	// VS 객체
	SgfComPtr<ID3DBlob> pBytecode_;			// 컴파일 결과 (InputLayout 생성에 필요)
	jc::Vector<SignatureElement> inputSignature_;	// 소비하는 시맨틱 목록
	_u64 signatureHash_ = 0;						// 캐시 키의 절반
};

// 픽셀 셰이더 (픽셀마다 1번 실행. 최종 색 결정)
class PixelShader : public ResourceBase
{
	SGF_RESOURCE_TYPE(rtPixelShader)

public:
	PixelShader();
	~PixelShader();

	bool InitializeFromSource(GraphicDevice* _pDevice, const jc::String& _szSource, const jc::String& _szEntry = "PSMain");
	bool InitializeFromFile(GraphicDevice* _pDevice, const jc::String& _szFilePath, const jc::String& _szEntry = "PSMain");

	void Finalize();

	bool IsValid() const { return pShader_ != nullptr; }
	ID3D11PixelShader* Raw() const { return pShader_.Get(); }

private:
	SgfComPtr<ID3D11PixelShader> pShader_;	// PS 객체
};

NS_SGF_END
