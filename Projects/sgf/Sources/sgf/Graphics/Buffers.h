/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:10:00 AM
 * =====================
 * GPU 버퍼 래퍼 (정점/인덱스/상수 버퍼)
 *
 * [GPU 버퍼란?]
 * GPU가 직접 읽을 수 있는 메모리 덩어리.
 * CPU 메모리(일반 배열)에 있는 데이터는 GPU가 읽을 수 없으므로
 * 반드시 버퍼를 만들어서 복사해 넣어야 한다.
 *
 * - 정점 버퍼(Vertex Buffer): 정점 배열을 담는다.
 * - 인덱스 버퍼(Index Buffer): 정점 번호 배열을 담는다. 정점 재사용으로 메모리 절약.
 * (사각형 = 정점 4개 + 인덱스 6개(삼각형 2개). 인덱스 없이면 정점 6개 필요)
 * - 상수 버퍼(Constant Buffer): 셰이더의 전역변수(변환 행렬 등)를 담는다.
 *
 * [Usage 종류] (ResourceUsage enum, GraphicsEnums.h)
 * - ruDefault: GPU 전용. 만들 때 데이터를 넣고 이후 변경 안 함. 가장 빠름. (Mesh 등 정적 리소스)
 * - ruImmutable: 생성 시 데이터 필수, 이후 변경 불가. 가장 빠름. (Texture 등 불변 리소스)
 * - ruDynamic: CPU가 매 프레임 갱신 가능. Map(DISCARD)/Unmap으로 쓴다. (상수버퍼, 배치용)
 * - ruStaging: CPU 읽기/쓰기 가능, GPU 복사 전용. (Readback용, 현재 미사용)
 */

#pragma once

#include "jc/Math.h"
#include "jc/Primitives/Span.h"
#include "sgf/Graphics/GraphicsEnums.h"
#include "sgf/Graphics/Vertex.h"

NS_SGF_BEGIN
class GraphicDevice;
class GraphicContext;
NS_SGF_END

#include "sgf/Graphics/GraphicDevice.h"

NS_SGF_BEGIN

// 정점 버퍼 래퍼
class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	// 정점 버퍼를 생성한다.
	// @param _pDevice: 그래픽 디바이스
	// @param _pData: 초기 정점 데이터 (ruDynamic이면 nullptr 가능)
	// @param _stride: 정점 하나의 바이트 크기 (sizeof(VertexPC) 등)
	// @param _count: 정점 개수
	// @param _layout: 정점 레이아웃 뷰 (VertexPTC::Layout() 등, Span 복사 보관)
	// @param _usage: 리소스 사용 방식 (기본 ruDefault, 매 프레임 갱신 시 ruDynamic)
	// @return 성공 여부
	bool Create(GraphicDevice* _pDevice, const void* _pData, UINT _stride, UINT _count, VertexLayoutSpan _layout, ResourceUsage _usage = ResourceUsage::ruDefault);
	bool Create(GraphicDevice* _pDevice, const void* _pData, UINT _stride, UINT _count, ResourceUsage _usage = ResourceUsage::ruDefault);

	// DYNAMIC 버퍼의 내용을 새 데이터로 교체한다.
	// Map(DISCARD)은 "이전 내용은 버릴 테니 새 메모리 주세요"라는 의미로,
	// GPU가 이전 내용을 읽는 중이어도 멈추지 않고 진행된다. (빠름)
	// @param _count: 복사할 정점 개수 (Create 때의 count 이하여야 함)
	bool Update(GraphicContext& _context, const void* _pData, UINT _count);

	// IA 단계 슬롯 0에 이 정점 버퍼를 장착한다. 캐시를 통과한다.
	void Bind(GraphicContext& _context);

	UINT Count() const { return count_; }
	UINT Stride() const { return stride_; }
	VertexLayoutSpan Layout() const { return layout_; }

	// GraphicContext 바인딩 캐시용 원본 핸들
	ID3D11Buffer* Raw() const { return pBuffer_.Get(); }

private:
	SgfComPtr<ID3D11Buffer> pBuffer_;	// GPU 버퍼 객체
	UINT stride_;						// 정점 1개 바이트 크기
	UINT count_;						// 정점 개수
	ResourceUsage usage_;				// 리소스 사용 방식 (ruDynamic이면 Map 가능)
	VertexLayoutSpan layout_;			// 정점 레이아웃 뷰 (non-owning, static 수명)
};

// 인덱스 버퍼 래퍼 (32비트 인덱스 고정)
class IndexBuffer
{
public:
	IndexBuffer();
	~IndexBuffer();

	// 인덱스 버퍼를 생성한다.
	// @param _pIndices: 인덱스 배열 (정점 번호들)
	// @param _count: 인덱스 개수 (삼각형 수 x 3)
	// @param _usage: 리소스 사용 방식 (기본 ruDefault, 매 프레임 갱신 시 ruDynamic)
	bool Create(GraphicDevice* _pDevice, const _u32* _pIndices, UINT _count, ResourceUsage _usage = ResourceUsage::ruDefault);

	// DYNAMIC 인덱스 버퍼 갱신
	bool Update(GraphicContext& _context, const _u32* _pIndices, UINT _count);

	// IA 단계에 인덱스 버퍼를 장착한다. 캐시를 통과한다.
	void Bind(GraphicContext& _context);

	UINT Count() const { return count_; }

	// GraphicContext 바인딩 캐시용 원본 핸들
	ID3D11Buffer* Raw() const { return pBuffer_.Get(); }

private:
	SgfComPtr<ID3D11Buffer> pBuffer_;	// GPU 버퍼 객체
	UINT count_;						// 인덱스 개수
	ResourceUsage usage_;				// 리소스 사용 방식 (ruDynamic이면 Map 가능)
};

// 상수 버퍼 래퍼 (템플릿으로 구조체 타입을 고정)
// [사용 규칙]
// 1. 구조체 크기는 16바이트 배수여야 한다. (HLSL cbuffer 규칙)
// 2. 행렬은 우리 엔진이 행우선(row-major)이므로 HLSL 쪽도
// row_major float4x4로 선언해서 전치(transpose) 없이 그대로 복사한다.
template <typename T>
class ConstantBuffer
{
public:
	ConstantBuffer() : pBuffer_(nullptr) {}
	~ConstantBuffer() {}

	// 상수 버퍼 생성. 항상 ruDynamic으로 만든다. (매 프레임 갱신하는 용도이므로)
	bool Create(GraphicDevice* _pDevice)
	{
		static_assert(sizeof(T) % 16 == 0, "상수 버퍼 구조체는 16바이트 배수여야 합니다.");

		// 재생성 대비: 기존 버퍼를 먼저 정리한다. (GetAddressOf 덮어쓰기 누수 방지)
		pBuffer_.Reset();

		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(T);
		bd.Usage = ToD3D11(ResourceUsage::ruDynamic);		// CPU 갱신 가능 (D3D11_USAGE_DYNAMIC)
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		// 상수 버퍼 용도
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// CPU 쓰기 허용
		return SUCCEEDED(_pDevice->Device()->CreateBuffer(&bd, nullptr, pBuffer_.GetAddressOf()));
	}

	// 상수 버퍼 내용을 갱신하고 VS/PS 양쪽 슬롯에 장착한다.
	// @param _data: 셰이더에 넘길 값 (구조체 통째로 복사)
	// @param _slot: 셰이더의 register(b0), register(b1)... 번호
	void UpdateAndBind(GraphicContext& _context, const T& _data, UINT _slot = 0)
	{
		_context.UpdateBuffer(pBuffer_.Get(), &_data, sizeof(T));
		_context.SetConstantBuffer(ShaderStage::ssVertex, _slot, pBuffer_.Get());
		_context.SetConstantBuffer(ShaderStage::ssPixel, _slot, pBuffer_.Get());
	}

	// 내용만 갱신한다. 바인딩은 GraphicContext::SetConstantBuffer로 별도 수행.
	void Update(GraphicContext& _context, const T& _data)
	{
		_context.UpdateBuffer(pBuffer_.Get(), &_data, sizeof(T));
	}

	// GraphicContext::SetConstantBuffer에 넘길 원본 핸들
	ID3D11Buffer* Raw() const { return pBuffer_.Get(); }

private:
	SgfComPtr<ID3D11Buffer> pBuffer_;	// GPU 버퍼 객체
};

NS_SGF_END
