/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:12:00 AM
 * =====================
 * GPU 버퍼 래퍼 구현부
 */

#include "Core.h"
#include "sgf/Graphics/Buffers.h"
#include "sgf/Graphics/GraphicDevice.h"

#include <cstring>

NS_SGF_BEGIN

// ==================================================
// VertexBuffer
// ==================================================

//////////////////////////////////////////////////////////////////////////////////////////
// 생성자
VertexBuffer::VertexBuffer()
	: stride_(0)
	, count_(0)
	, dynamic_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 소멸자 (ComPtr이 자동 Release)
VertexBuffer::~VertexBuffer()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 정점 버퍼 생성
bool VertexBuffer::Create(GraphicDevice* _pDevice, const void* _pData, UINT _stride, UINT _count, bool _dynamic)
{
	stride_ = _stride;
	count_ = _count;
	dynamic_ = _dynamic;

	// 버퍼 설정: 크기/용도/CPU 접근 권한을 기술한다.
	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = _stride * _count;									// 전체 바이트 크기
	bd.Usage = _dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;	// 갱신 방식
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;							// 정점 버퍼 용도
	bd.CPUAccessFlags = _dynamic ? D3D11_CPU_ACCESS_WRITE : 0;			// CPU 쓰기 권한

	// 초기 데이터가 있으면 생성과 동시에 복사해 넣는다.
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = _pData;

	return SUCCEEDED(_pDevice->Device()->CreateBuffer(
		&bd,
		(_pData != nullptr) ? &sd : nullptr,	// 데이터 없이 빈 버퍼만 만들 수도 있다
		pBuffer_.GetAddressOf()));
}

//////////////////////////////////////////////////////////////////////////////////////////
// DYNAMIC 정점 버퍼 갱신
bool VertexBuffer::Update(GraphicDevice* _pDevice, const void* _pData, UINT _count)
{
	// DEFAULT 버퍼는 Map이 불가능하므로 방어한다.
	if (!dynamic_ || _count > count_)
	{
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT hr = _pDevice->Context()->Map(pBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (FAILED(hr))
	{
		return false;
	}
	memcpy(mapped.pData, _pData, size_t(stride_) * _count);
	_pDevice->Context()->Unmap(pBuffer_.Get(), 0);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// IA 단계 슬롯 0에 장착
void VertexBuffer::Bind(GraphicDevice* _pDevice)
{
	UINT stride = stride_;		// 정점 하나씩 건너뛸 바이트 수
	UINT offset = 0;			// 버퍼 시작 오프셋
	ID3D11Buffer* pBuffers[] = { pBuffer_.Get() };
	_pDevice->Context()->IASetVertexBuffers(0, 1, pBuffers, &stride, &offset);
}

// ==================================================
// IndexBuffer
// ==================================================

//////////////////////////////////////////////////////////////////////////////////////////
// 생성자
IndexBuffer::IndexBuffer()
	: count_(0)
	, dynamic_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 소멸자
IndexBuffer::~IndexBuffer()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 인덱스 버퍼 생성
bool IndexBuffer::Create(GraphicDevice* _pDevice, const _u32* _pIndices, UINT _count, bool _dynamic)
{
	count_ = _count;
	dynamic_ = _dynamic;

	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(_u32) * _count;
	bd.Usage = _dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;								// 인덱스 버퍼 용도
	bd.CPUAccessFlags = _dynamic ? D3D11_CPU_ACCESS_WRITE : 0;

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = _pIndices;

	return SUCCEEDED(_pDevice->Device()->CreateBuffer(
		&bd,
		(_pIndices != nullptr) ? &sd : nullptr,
		pBuffer_.GetAddressOf()));
}

//////////////////////////////////////////////////////////////////////////////////////////
// DYNAMIC 인덱스 버퍼 갱신
bool IndexBuffer::Update(GraphicDevice* _pDevice, const _u32* _pIndices, UINT _count)
{
	if (!dynamic_ || _count > count_)
	{
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT hr = _pDevice->Context()->Map(pBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (FAILED(hr))
	{
		return false;
	}
	memcpy(mapped.pData, _pIndices, sizeof(_u32) * _count);
	_pDevice->Context()->Unmap(pBuffer_.Get(), 0);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// IA 단계에 장착 (32비트 인덱스 형식 고정)
void IndexBuffer::Bind(GraphicDevice* _pDevice)
{
	_pDevice->Context()->IASetIndexBuffer(pBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);
}

NS_SGF_END
