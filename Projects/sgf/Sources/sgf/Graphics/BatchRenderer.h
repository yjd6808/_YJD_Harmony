/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 10:30:00 AM
 * =====================
 * 배치 렌더러 공용 베이스 클래스
 *
 * [왜 만드는가?]
 * Renderer2D와 Renderer3D는 "그릴 때 바로 그리지 않고 CPU 배열에 정점을
 * 모아두었다가 한 번에 GPU로 보내는" 배치(batch) 렌더링 원리가 동일하다.
 * 따라서 아래 공통 흐름을 베이스로 끌어올려 중복을 제거한다.
 *
 * 공통 흐름:
 * - Initialize: 셰이더 컴파일(파생이 소스/선언 제공) + 상수 버퍼 생성
 * - Begin: 뷰프로젝션 저장 + 배치 초기화(파생 훅 OnBegin)
 * - End: 남은 배치를 모두 그림(파생 Flush) + 마무리
 * - Flush: 모아둔 배치를 즉시 GPU로 전송 (파생 구현)
 */

#pragma once

#include "jc/Math.h"
#include "sgf/Graphics/VertexDeclaration.h"
#include "sgf/Graphics/Buffers.h"
#include "sgf/Graphics/ShaderProgram.h"

NS_SGF_BEGIN

using namespace jc;

class GraphicDevice;

class BatchRenderer
{
public:
	BatchRenderer();
	virtual ~BatchRenderer();

	bool Initialize(GraphicDevice* _pDevice);
	virtual void Finalize();

	void Begin(const mat4& _viewProjection);
	void End();
	virtual void Flush() = 0;

protected:
	////////////////////////////////////////////////////////////////////////////////////////
	// 파생 훅
	virtual const char* ShaderSource() const = 0;
	virtual const VertexDeclaration* VertexDecl() const = 0;
	virtual bool CreateBatchResources(GraphicDevice* _pDevice) = 0;
	virtual void OnBegin() = 0;

	void ApplyFrameStates();

protected:
	GraphicDevice* pDevice_ = nullptr;
	VertexShader vs_;	// 버텍스 셰이더 (분리형)
	PixelShader ps_;	// 픽셀 셰이더 (분리형)
	ConstantBuffer<mat4> cbFrame_;	// 뷰프로젝션 행렬용 상수 버퍼
	mat4 viewProjection_;			// Begin에서 받은 행렬
	bool begun_ = false;			// Begin~End 사이인가?
};

NS_SGF_END
