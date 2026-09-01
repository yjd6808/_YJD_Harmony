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

namespace
{
	inline bool CreateD3DBuffer(GraphicDevice* _pDevice, UINT _byteWidth, D3D11_BIND_FLAG _bindFlag, ResourceUsage _usage, const void* _pData, SgfComPtr<ID3D11Buffer>& _outBuffer)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = _byteWidth;
		bd.Usage = ToD3D11(_usage);
		bd.BindFlags = _bindFlag;
		bd.CPUAccessFlags = (_usage == ResourceUsage::ruDynamic) ? D3D11_CPU_ACCESS_WRITE : 0;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = _pData;

		return SUCCEEDED(_pDevice->Device()->CreateBuffer(&bd, (_pData != nullptr) ? &sd : nullptr, _outBuffer.GetAddressOf()));
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// VertexBuffer

////////////////////////////////////////////////////////////////////////////////////////
// 생성자
VertexBuffer::VertexBuffer()
	: stride_(0)
	, count_(0)
	, usage_(ResourceUsage::ruDefault)
	, pDecl_(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////////////
// 소멸자 (ComPtr이 자동 Release)
VertexBuffer::~VertexBuffer()
{
}

////////////////////////////////////////////////////////////////////////////////////////
// 정점 버퍼 생성
bool VertexBuffer::Create(GraphicDevice* _pDevice, const void* _pData, UINT _count, const VertexDeclaration* _pDecl, ResourceUsage _usage)
{
	jc_assert_msg(_pDecl != nullptr, "정점 버퍼에는 VertexDeclaration이 필요합니다.");

	stride_ = _pDecl->Stride(0);
	count_ = _count;
	usage_ = _usage;
	pDecl_ = _pDecl;

	return CreateD3DBuffer(_pDevice, stride_ * _count, D3D11_BIND_VERTEX_BUFFER, _usage, _pData, pBuffer_);
}

////////////////////////////////////////////////////////////////////////////////////////
// DYNAMIC 정점 버퍼 갱신 - GraphicContext 캐시를 통과한다
bool VertexBuffer::Update(GraphicContext& _context, const void* _pData, UINT _count)
{
	if (usage_ != ResourceUsage::ruDynamic || _count > count_)
	{
		return false;
	}

	return _context.UpdateBuffer(pBuffer_.Get(), _pData, stride_ * _count);
}

////////////////////////////////////////////////////////////////////////////////////////
// IA 단계 슬롯 0에 장착 - 캐시를 통과한다
void VertexBuffer::Bind(GraphicContext& _context)
{
	_context.SetVertexBuffer(this);
}

////////////////////////////////////////////////////////////////////////////////////////
// IndexBuffer

////////////////////////////////////////////////////////////////////////////////////////
// 생성자
IndexBuffer::IndexBuffer()
	: count_(0)
	, usage_(ResourceUsage::ruDefault)
{
}

////////////////////////////////////////////////////////////////////////////////////////
// 소멸자
IndexBuffer::~IndexBuffer()
{
}

////////////////////////////////////////////////////////////////////////////////////////
// 인덱스 버퍼 생성
bool IndexBuffer::Create(GraphicDevice* _pDevice, const _u32* _pIndices, UINT _count, ResourceUsage _usage)
{
	count_ = _count;
	usage_ = _usage;

	return CreateD3DBuffer(_pDevice, sizeof(_u32) * _count, D3D11_BIND_INDEX_BUFFER, _usage, _pIndices, pBuffer_);
}

////////////////////////////////////////////////////////////////////////////////////////
// DYNAMIC 인덱스 버퍼 갱신 - GraphicContext 캐시를 통과한다
bool IndexBuffer::Update(GraphicContext& _context, const _u32* _pIndices, UINT _count)
{
	if (usage_ != ResourceUsage::ruDynamic || _count > count_)
	{
		return false;
	}

	return _context.UpdateBuffer(pBuffer_.Get(), _pIndices, sizeof(_u32) * _count);
}

////////////////////////////////////////////////////////////////////////////////////////
// IA 단계에 장착 (32비트 인덱스 형식 고정) - 캐시를 통과한다
void IndexBuffer::Bind(GraphicContext& _context)
{
	_context.SetIndexBuffer(this);
}

NS_SGF_END
