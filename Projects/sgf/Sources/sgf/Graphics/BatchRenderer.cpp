/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 10:30:00 AM
 * =====================
 * 배치 렌더러 공용 베이스 클래스 구현부
 */

#include "Core.h"
#include "sgf/Graphics/BatchRenderer.h"
#include "sgf/Graphics/GraphicDevice.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
BatchRenderer::BatchRenderer()
	: pDevice_(nullptr)
	, viewProjection_(mat4::Identity())
	, begun_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
BatchRenderer::~BatchRenderer()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 공통 초기화: 셰이더 컴파일 + 상수 버퍼 생성 + 파생 전용 리소스 생성
bool BatchRenderer::Initialize(GraphicDevice* _pDevice)
{
	pDevice_ = _pDevice;

	// 1. 파생이 내려준 셰이더 소스/정점 레이아웃으로 컴파일
	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pLayoutDescs = VertexLayout(&layoutCount);
	if (!shader_.CompileFromString(_pDevice, ShaderSource(), pLayoutDescs, layoutCount))
	{
		return false;
	}

	// 2. 뷰프로젝션 행렬용 상수 버퍼
	if (!cbFrame_.Create(_pDevice))
	{
		return false;
	}

	// 3. 파생 전용 리소스 (정점/인덱스 버퍼, 텍스처 등)
	return CreateBatchResources(_pDevice);
}

//////////////////////////////////////////////////////////////////////////////////////////
void BatchRenderer::Finalize()
{
	// 셰이더/상수 버퍼는 각자의 소멸자(ComPtr)가 GPU 리소스를 알아서 해제한다.
	begun_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 배치 시작: 행렬 저장 + 파생의 OnBegin()으로 배치 초기화
void BatchRenderer::Begin(const mat4& _viewProjection)
{
	jc_assert(!begun_);
	begun_ = true;
	viewProjection_ = _viewProjection;
	OnBegin();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 배치 종료: 남은 배치를 모두 그린다.
void BatchRenderer::End()
{
	jc_assert(begun_);
	Flush();
	begun_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 공통 파이프라인 구성: 셰이더 바인딩 + 뷰프로젝션 상수 버퍼 갱신/바인딩
void BatchRenderer::ApplyFrameStates()
{
	shader_.Bind(pDevice_);
	cbFrame_.UpdateAndBind(pDevice_, viewProjection_, 0);
}

NS_SGF_END
