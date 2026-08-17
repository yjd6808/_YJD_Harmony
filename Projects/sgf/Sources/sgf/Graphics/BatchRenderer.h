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
 * - Initialize: 셰이더 컴파일(파생이 소스/레이아웃 제공) + 상수 버퍼 생성
 * - Begin: 뷰프로젝션 저장 + 배치 초기화(파생 훅 OnBegin)
 * - End: 남은 배치를 모두 그림(파생 Flush) + 마무리
 * - Flush: 모아둔 배치를 즉시 GPU로 전송 (파생 구현)
 *
 * 파생 클래스는 정점 버퍼/인덱스 버퍼/텍스처 등 "자신만의 리소스"와
 * 실제 드로우 콜만 구현하면 된다.
 */

#pragma once

#include "jc/Math.h"
#include "sgf/Graphics/Shader.h"
#include "sgf/Graphics/Buffers.h"

NS_SGF_BEGIN

using namespace jc;

class GraphicDevice;

class BatchRenderer
{
public:
	BatchRenderer();
	virtual ~BatchRenderer();

	// 공통 초기화: 파생 훅으로 셰이더를 컴파일하고 상수 버퍼를 만든 뒤
	// CreateBatchResources로 파생 전용 리소스(정점/인덱스 버퍼 등)를 생성한다.
	bool Initialize(GraphicDevice* _pDevice);

	// 공통 해제. 파생 전용 리소스가 있으면 override 후 Base::Finalize()를 호출한다.
	virtual void Finalize();

	// 배치 시작. 뷰프로젝션 행렬을 저장하고 파생의 OnBegin()으로 배치를 초기화한다.
	// @param _viewProjection: 카메라의 View x Projection 행렬
	void Begin(const mat4& _viewProjection);

	// 배치 종료. 모아둔 정점을 모두 그린다.
	void End();

	// 모아둔 배치를 즉시 GPU로 보낸다. (버퍼가 가득 찼을 때도 파생이 호출)
	virtual void Flush() = 0;

protected:
	// === 파생 훅 ===

	// 내장 HLSL 셰이더 소스 문자열 반환
	virtual const char* ShaderSource() const = 0;

	// 셰이더에 맞는 정점 레이아웃 반환
	virtual const D3D11_INPUT_ELEMENT_DESC* VertexLayout(UINT* _outCount) const = 0;

	// 셰이더/상수 버퍼 생성 후 호출된다. 정점 버퍼, 인덱스 버퍼, 텍스처 등
	// 파생 전용 리소스를 여기서 만든다.
	virtual bool CreateBatchResources(GraphicDevice* _pDevice) = 0;

	// Begin() 공통 처리 직후 호출된다. CPU 배치 버퍼를 비우고,
	// 파생 전용 상태(깊이/블렌드 등)를 설정한다.
	virtual void OnBegin() = 0;

	// 셰이더 바인딩 + 뷰프로젝션 상수 버퍼 갱신/바인딩.
	// 파생의 Flush() 구현에서 드로우 콜 직전에 호출한다.
	void ApplyFrameStates();

protected:
	GraphicDevice* pDevice_;			// 그래픽 디바이스 (소유하지 않음)
	Shader shader_;					// 파생이 제공한 셰이더
	ConstantBuffer<mat4> cbFrame_;		// 뷰프로젝션 행렬용 상수 버퍼
	mat4 viewProjection_;				// Begin에서 받은 행렬
	bool begun_;						// Begin~End 사이인가?
};

NS_SGF_END
