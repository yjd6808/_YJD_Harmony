/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:12:00 AM
 * =====================
 * GPU 버퍼 래퍼 구현부
 */

#include "Core.h"
#include "sgf/Graphics/Buffers.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "sgf/Graphics/GraphicContext.h"

#include <cstring>

NS_SGF_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
// VertexBuffer

//////////////////////////////////////////////////////////////////////////////////////////
// 생성자
VertexBuffer::VertexBuffer()
	: stride_(0)
	, count_(0)
	, usage_(ResourceUsage::ruDefault)
	, layout_()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 소멸자 (ComPtr이 자동 Release)
VertexBuffer::~VertexBuffer()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 정점 버퍼 생성
bool VertexBuffer::Create(GraphicDevice* _pDevice, const void* _pData, UINT _stride, UINT _count, VertexLayoutSpan _layout, ResourceUsage _usage)
{
	stride_ = _stride;
	count_ = _count;
	usage_ = _usage;
	layout_ = _layout;

	// 버퍼 설정: 크기/용도/CPU 접근 권한을 기술한다.
	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = _stride * _count;									// 전체 바이트 크기
	bd.Usage = ToD3D11(_usage);											// 갱신 방식 (ResourceUsage → D3D11_USAGE)
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;							// 정점 버퍼 용도
	bd.CPUAccessFlags = (_usage == ResourceUsage::ruDynamic) ? D3D11_CPU_ACCESS_WRITE : 0;	// CPU 쓰기 권한 (ruDynamic만 Map 가능)

	// 초기 데이터가 있으면 생성과 동시에 복사해 넣는다.
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = _pData;

	return SUCCEEDED(_pDevice->Device()->CreateBuffer(
		&bd,
		(_pData != nullptr) ? &sd : nullptr,	// 데이터 없이 빈 버퍼만 만들 수도 있다
		pBuffer_.GetAddressOf()));
}

bool VertexBuffer::Create(GraphicDevice* _pDevice, const void* _pData, UINT _stride, UINT _count, ResourceUsage _usage)
{
	return Create(_pDevice, _pData, _stride, _count, VertexLayoutSpan{}, _usage);
}

//////////////////////////////////////////////////////////////////////////////////////////
// DYNAMIC 정점 버퍼 갱신 - GraphicContext 캐시를 통과한다
bool VertexBuffer::Update(GraphicContext& _context, const void* _pData, UINT _count)
{
	if (usage_ != ResourceUsage::ruDynamic || _count > count_)
	{
		return false;
	}

	return _context.UpdateBuffer(pBuffer_.Get(), _pData, stride_ * _count);
}

//////////////////////////////////////////////////////////////////////////////////////////
// IA 단계 슬롯 0에 장착 - 캐시를 통과한다
void VertexBuffer::Bind(GraphicContext& _context)
{
	_context.SetVertexBuffer(this);
}

//////////////////////////////////////////////////////////////////////////////////////////
// IndexBuffer

//////////////////////////////////////////////////////////////////////////////////////////
// 생성자
IndexBuffer::IndexBuffer()
	: count_(0)
	, usage_(ResourceUsage::ruDefault)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 소멸자
IndexBuffer::~IndexBuffer()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 인덱스 버퍼 생성
bool IndexBuffer::Create(GraphicDevice* _pDevice, const _u32* _pIndices, UINT _count, ResourceUsage _usage)
{
	count_ = _count;
	usage_ = _usage;

	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(_u32) * _count;
	bd.Usage = ToD3D11(_usage);											// 갱신 방식 (ResourceUsage → D3D11_USAGE)
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;								// 인덱스 버퍼 용도
	bd.CPUAccessFlags = (_usage == ResourceUsage::ruDynamic) ? D3D11_CPU_ACCESS_WRITE : 0;	// CPU 쓰기 권한 (ruDynamic만 Map 가능)

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = _pIndices;

	return SUCCEEDED(_pDevice->Device()->CreateBuffer(
		&bd,
		(_pIndices != nullptr) ? &sd : nullptr,
		pBuffer_.GetAddressOf()));
}

//////////////////////////////////////////////////////////////////////////////////////////
// DYNAMIC 인덱스 버퍼 갱신 - GraphicContext 캐시를 통과한다
bool IndexBuffer::Update(GraphicContext& _context, const _u32* _pIndices, UINT _count)
{
	if (usage_ != ResourceUsage::ruDynamic || _count > count_)
	{
		return false;
	}

	return _context.UpdateBuffer(pBuffer_.Get(), _pIndices, sizeof(_u32) * _count);
}

//////////////////////////////////////////////////////////////////////////////////////////
// IA 단계에 장착 (32비트 인덱스 형식 고정) - 캐시를 통과한다
void IndexBuffer::Bind(GraphicContext& _context)
{
	_context.SetIndexBuffer(this);
}

NS_SGF_END
