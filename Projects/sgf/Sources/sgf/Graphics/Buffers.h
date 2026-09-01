/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:10:00 AM
 * =====================
 * GPU 버퍼 래퍼 (정점/인덱스/상수 버퍼)
 */

#pragma once

#include "jc/Math.h"
#include "jc/Primitives/Span.h"
#include "sgf/Graphics/GraphicsEnums.h"
#include "sgf/Graphics/VertexDeclaration.h"
#include "sgf/Graphics/IResource.h"

NS_SGF_BEGIN
class GraphicDevice;
class GraphicContext;
NS_SGF_END

#include "sgf/Graphics/GraphicDevice.h"

NS_SGF_BEGIN

//////////////////////////////////////////////////////////////////////////////////
class VertexBuffer : public ResourceBase
{
	SGF_RESOURCE_TYPE(rtVertexBuffer)
public:
	VertexBuffer();
	~VertexBuffer();

	bool Create(GraphicDevice* _pDevice, const void* _pData, UINT _count, const VertexDeclaration* _pDecl, ResourceUsage _usage = ResourceUsage::ruDefault);
	bool Update(GraphicContext& _context, const void* _pData, UINT _count);
	void Bind(GraphicContext& _context);

	UINT Count() const { return count_; }
	UINT Stride() const { return stride_; }
	const VertexDeclaration* Decl() const { return pDecl_; }
	ID3D11Buffer* Raw() const { return pBuffer_.Get(); }

private:
	SgfComPtr<ID3D11Buffer> pBuffer_;
	UINT stride_;
	UINT count_;
	ResourceUsage usage_;
	const VertexDeclaration* pDecl_;
};

//////////////////////////////////////////////////////////////////////////////////
class IndexBuffer : public ResourceBase
{
	SGF_RESOURCE_TYPE(rtIndexBuffer)
public:
	IndexBuffer();
	~IndexBuffer();

	bool Create(GraphicDevice* _pDevice, const _u32* _pIndices, UINT _count, ResourceUsage _usage = ResourceUsage::ruDefault);
	bool Update(GraphicContext& _context, const _u32* _pIndices, UINT _count);
	void Bind(GraphicContext& _context);

	UINT Count() const { return count_; }
	ID3D11Buffer* Raw() const { return pBuffer_.Get(); }

private:
	SgfComPtr<ID3D11Buffer> pBuffer_;
	UINT count_;
	ResourceUsage usage_;
};

//////////////////////////////////////////////////////////////////////////////////
template <typename T>
class ConstantBuffer
{
public:
	ConstantBuffer() : pBuffer_(nullptr) {}
	~ConstantBuffer() {}

	bool Create(GraphicDevice* _pDevice)
	{
		static_assert(sizeof(T) % 16 == 0, "상수 버퍼 구조체는 16바이트 배수여야 합니다.");
		pBuffer_.Reset();
		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(T);
		bd.Usage = ToD3D11(ResourceUsage::ruDynamic);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		return SUCCEEDED(_pDevice->Device()->CreateBuffer(&bd, nullptr, pBuffer_.GetAddressOf()));
	}

	void UpdateAndBind(GraphicContext& _context, const T& _data, UINT _slot = 0)
	{
		_context.UpdateBuffer(pBuffer_.Get(), &_data, sizeof(T));
		_context.SetConstantBuffer(ShaderStage::ssVertex, _slot, pBuffer_.Get());
		_context.SetConstantBuffer(ShaderStage::ssPixel, _slot, pBuffer_.Get());
	}

	void Update(GraphicContext& _context, const T& _data)
	{
		_context.UpdateBuffer(pBuffer_.Get(), &_data, sizeof(T));
	}

	ID3D11Buffer* Raw() const { return pBuffer_.Get(); }

private:
	SgfComPtr<ID3D11Buffer> pBuffer_;
};

NS_SGF_END
