/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:30:00 PM
 * =====================
 * 셰이더 계층: VertexShader / PixelShader / InputLayout
 *
 * [Shader와의 차이]
 * Shader는 VS+PS+입력레이아웃을 한 덩어리로 묶었다. (간단하지만 재사용 불가)
 * 세 개를 분리해서 "같은 VS + 다른 PS" 같은 조합이 가능하다.
 * 셰이더는 IResource를 상속받아 ResourceMgr에 등록/공유된다.
 *
 * [왜 VS가 바이트코드를 들고 있나?]
 * 입력 레이아웃을 만들려면 "이 VS의 입력 서명과 맞는지" 검증이 필요해서
 * D3D11이 VS 바이트코드를 요구한다. 그래서 VertexShader가 blob을 보관한다.
 */

#pragma once

#include "sgf/Graphics/IResource.h"
#include "sgf/Graphics/GraphicsEnums.h"
#include "sgf/Graphics/Vertex.h"

NS_SGF_BEGIN

using namespace jc;

class GraphicDevice;

// 버텍스 셰이더 (정점마다 1번 실행. 위치 변환 담당)
class VertexShader : public ResourceBase
{
	SGF_RESOURCE_TYPE(rtVertexShader)

public:
	VertexShader();
	~VertexShader();

	// HLSL 소스 문자열을 컴파일해서 VS를 만든다.
	// @param _szSource: HLSL 소스 코드 문자열
	// @param _szEntry: 진입 함수 이름 (기본 "VSMain")
	// @return 성공 여부 (실패 시 오류가 디버그 출력창에 출력됨)
	bool InitializeFromSource(GraphicDevice* _pDevice, const jc::String& _szSource, const jc::String& _szEntry = "VSMain");

	// HLSL 파일을 읽어서 컴파일한다. (수도코드의 VertexShader.init("x.hlsl", "VSMain") 대응)
	bool InitializeFromFile(GraphicDevice* _pDevice, const jc::String& _szFilePath, const jc::String& _szEntry = "VSMain");

	void Finalize();

	bool IsValid() const { return pShader_ != nullptr; }
	ID3D11VertexShader* Raw() const { return pShader_.Get(); }

	// 입력 레이아웃 생성용 바이트코드
	ID3DBlob* Bytecode() const { return pBytecode_.Get(); }

private:
	SgfComPtr<ID3D11VertexShader> pShader_;	// VS 객체
	SgfComPtr<ID3DBlob> pBytecode_;			// 컴파일 결과 (InputLayout 생성에 필요)
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

// 입력 레이아웃 (C++ 정점 구조체 <-> VS 입력 변수 연결 설명서)
class InputLayout : public ResourceBase
{
	SGF_RESOURCE_TYPE(rtInputLayout)

public:
	InputLayout();
	~InputLayout();

	// 정점 구조 설명 배열 + VS 바이트코드로 레이아웃을 만든다.
	// @param _layout: 정점 구조 설명 뷰 (VertexPC::Layout() 등)
	// @param _pVs: 서명 검증에 쓸 버텍스 셰이더 (컴파일 완료 상태여야 함)
	bool Initialize(GraphicDevice* _pDevice, VertexLayoutSpan _layout, VertexShader* _pVs);

	void Finalize();

	bool IsValid() const { return pLayout_ != nullptr; }
	ID3D11InputLayout* Raw() const { return pLayout_.Get(); }

private:
	SgfComPtr<ID3D11InputLayout> pLayout_;	// 레이아웃 객체
};

NS_SGF_END
