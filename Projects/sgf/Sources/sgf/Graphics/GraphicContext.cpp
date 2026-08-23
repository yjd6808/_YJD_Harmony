/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:20:00 PM
 * =====================
 * 그래픽 컨텍스트 구현부
 *
 * [캐시 규칙]
 * 모든 Set 함수는 "마지막으로 묶은 D3D 객체"와 비교해 같으면 생략한다.
 * 생략 여부는 skippedCallCount_로 집계되어 튜토리얼 23장에서 직접 확인할 수 있다.
 */

#include "Core.h"
#include "sgf/Graphics/GraphicContext.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "sgf/Graphics/Buffers.h"
#include "sgf/Graphics/Texture.h"
#include "sgf/Graphics/ShaderProgram.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
GraphicContext::GraphicContext()
	: pContext_(nullptr)
	, pDevice_(nullptr)
	, deferred_(false)
	, pCachedRtv_(nullptr)
	, pCachedDsv_(nullptr)
	, cachedRtvCount_(0)
{
	InvalidateCache();
	apiCallCount_ = 0;
	skippedCallCount_ = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
GraphicContext::~GraphicContext()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GraphicContext::InitializeImmediate(GraphicDevice* _pDevice, ID3D11DeviceContext* _pImmediate)
{
	if (_pDevice == nullptr || _pImmediate == nullptr)
	{
		return false;
	}

	pDevice_ = _pDevice;
	deferred_ = false;
	pContext_ = _pImmediate; // SgfComPtr AddRef - 디바이스가 내부 참조를 유지하므로 공유 소유
	InvalidateCache();
	ResetStats();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GraphicContext::InitializeDeferred(GraphicDevice* _pDevice)
{
	if (_pDevice == nullptr || _pDevice->Device() == nullptr)
	{
		return false;
	}

	pDevice_ = _pDevice;
	deferred_ = true;
	HRESULT hr = _pDevice->Device()->CreateDeferredContext(0, pContext_.GetAddressOf());
	if (FAILED(hr))
	{
		pDevice_ = nullptr;
		deferred_ = false;
		return false;
	}

	InvalidateCache();
	ResetStats();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GraphicContext::Initialize(GraphicDevice* _pDevice)
{
	// 하위 호환 - InitializeImmediate로 위임
	if (_pDevice == nullptr)
	{
		return false;
	}

	return InitializeImmediate(_pDevice, _pDevice->Context().Raw());
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::Finalize()
{
	for (_s32 i = 0; i < ownedInputLayouts_.Size(); ++i)
	{
		delete ownedInputLayouts_[i];
	}
	ownedInputLayouts_.Clear();

	if (pContext_ != nullptr)
	{
		// ClearState 후 참조 해제. 즉시 컨텍스트는 Device가 내부 참조를 유지하므로 실제 소멸은 Device 소멸 시.
		pContext_->ClearState();
		pContext_.Reset();
	}

	pDevice_ = nullptr;
	deferred_ = false;
	InvalidateCache();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 캐시를 모두 비운다. 외부에서 상태를 바꿨을 수 있는 경계(BeginFrame 등)에서 호출.
void GraphicContext::InvalidateCache()
{
	pCachedVertexBuffer_ = nullptr;
	pCachedIndexBuffer_ = nullptr;
	pCachedInputLayout_ = nullptr;
	pCachedVs_ = nullptr;
	pCachedPs_ = nullptr;
	pCachedRasterizer_ = nullptr;
	pCachedBlend_ = nullptr;
	pCachedDepth_ = nullptr;
	pCachedRtv_ = nullptr;
	pCachedDsv_ = nullptr;
	cachedRtvCount_ = 0;
	memset(pCachedSrvs_, 0, sizeof(pCachedSrvs_));
	memset(pCachedSamplers_, 0, sizeof(pCachedSamplers_));
	memset(pCachedCbuffers_, 0, sizeof(pCachedCbuffers_));
	cachedTopology_ = PrimitiveTopology::Max;	// "알 수 없음" 표시
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::ResetStats()
{
	apiCallCount_ = 0;
	skippedCallCount_ = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetVertexBuffer(VertexBuffer* _pBuffer)
{
	ID3D11Buffer* pRaw = (_pBuffer != nullptr) ? _pBuffer->Raw() : nullptr;
	if (pRaw == pCachedVertexBuffer_)
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedVertexBuffer_ = pRaw;
	apiCallCount_ += 1;

	UINT stride = (_pBuffer != nullptr) ? _pBuffer->Stride() : 0;
	UINT offset = 0;
	pContext_->IASetVertexBuffers(0, 1, &pRaw, &stride, &offset);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetIndexBuffer(IndexBuffer* _pBuffer)
{
	ID3D11Buffer* pRaw = (_pBuffer != nullptr) ? _pBuffer->Raw() : nullptr;
	if (pRaw == pCachedIndexBuffer_)
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedIndexBuffer_ = pRaw;
	apiCallCount_ += 1;
	pContext_->IASetIndexBuffer(pRaw, DXGI_FORMAT_R32_UINT, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetInputLayout(InputLayout* _pLayout)
{
	ID3D11InputLayout* pRaw = (_pLayout != nullptr) ? _pLayout->Raw() : nullptr;
	if (pRaw == pCachedInputLayout_)
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedInputLayout_ = pRaw;
	apiCallCount_ += 1;
	pContext_->IASetInputLayout(pRaw);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetPrimitiveTopology(PrimitiveTopology _topology)
{
	if (_topology == cachedTopology_)
	{
		skippedCallCount_ += 1;
		return;
	}

	cachedTopology_ = _topology;
	apiCallCount_ += 1;
	pContext_->IASetPrimitiveTopology(ToD3D11(_topology));
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetVertexShader(VertexShader* _pShader)
{
	ID3D11VertexShader* pRaw = (_pShader != nullptr) ? _pShader->Raw() : nullptr;
	if (pRaw == pCachedVs_)
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedVs_ = pRaw;
	apiCallCount_ += 1;
	pContext_->VSSetShader(pRaw, nullptr, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetPixelShader(PixelShader* _pShader)
{
	ID3D11PixelShader* pRaw = (_pShader != nullptr) ? _pShader->Raw() : nullptr;
	if (pRaw == pCachedPs_)
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedPs_ = pRaw;
	apiCallCount_ += 1;
	pContext_->PSSetShader(pRaw, nullptr, 0);
}

////////////////////////////////////////////////////////////////////////////////////////
_u32 GraphicContext::CreateVertexShader(const jc::String& _hlslSource, const jc::String& _entry)
{
	if (pDevice_ == nullptr) return INVALID_HANDLE;
	return pDevice_->CreateVertexShader(_hlslSource, _entry);
}

////////////////////////////////////////////////////////////////////////////////////////
_u32 GraphicContext::CreatePixelShader(const jc::String& _hlslSource, const jc::String& _entry)
{
	if (pDevice_ == nullptr) return INVALID_HANDLE;
	return pDevice_->CreatePixelShader(_hlslSource, _entry);
}

//////////////////////////////////////////////////////////////////////////////////////////
// ★ 상수버퍼 바인딩 — 셰이더가 읽을 데이터 보관함을 특정 스테이지/슬롯에 장착한다.
void GraphicContext::SetConstantBuffer(ShaderStage _stage, _u32 _slot, ID3D11Buffer* _pBuffer)
{
	jc_assert_msg(_slot < MAX_CBUFFER_SLOTS, "상수버퍼 슬롯 범위를 벗어났습니다.");

	const _s32 stageIndex = static_cast<_s32>(_stage);
	if (_pBuffer == pCachedCbuffers_[stageIndex][_slot])
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedCbuffers_[stageIndex][_slot] = _pBuffer;
	apiCallCount_ += 1;

	ID3D11Buffer* pBuffers[] = { _pBuffer };
	if (_stage == ShaderStage::ssVertex)
	{
		pContext_->VSSetConstantBuffers(_slot, 1, pBuffers);
	}
	else
	{
		pContext_->PSSetConstantBuffers(_slot, 1, pBuffers);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetTexture(ShaderStage _stage, _u32 _slot, Texture* _pTexture)
{
	jc_assert_msg(_slot < MAX_TEXTURE_SLOTS, "텍스처 슬롯 범위를 벗어났습니다.");

	ID3D11ShaderResourceView* pSrv = (_pTexture != nullptr) ? _pTexture->Srv() : nullptr;
	const _s32 stageIndex = static_cast<_s32>(_stage);
	if (pSrv == pCachedSrvs_[stageIndex][_slot])
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedSrvs_[stageIndex][_slot] = pSrv;
	apiCallCount_ += 1;

	ID3D11ShaderResourceView* pSrvs[] = { pSrv };
	if (_stage == ShaderStage::ssVertex)
	{
		pContext_->VSSetShaderResources(_slot, 1, pSrvs);
	}
	else
	{
		pContext_->PSSetShaderResources(_slot, 1, pSrvs);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetSampler(ShaderStage _stage, _u32 _slot, SamplerState* _pSampler)
{
	jc_assert_msg(_slot < MAX_TEXTURE_SLOTS, "샘플러 슬롯 범위를 벗어났습니다.");

	ID3D11SamplerState* pRaw = (_pSampler != nullptr) ? _pSampler->Raw() : nullptr;
	const _s32 stageIndex = static_cast<_s32>(_stage);
	if (pRaw == pCachedSamplers_[stageIndex][_slot])
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedSamplers_[stageIndex][_slot] = pRaw;
	apiCallCount_ += 1;

	ID3D11SamplerState* pSamplers[] = { pRaw };
	if (_stage == ShaderStage::ssVertex)
	{
		pContext_->VSSetSamplers(_slot, 1, pSamplers);
	}
	else
	{
		pContext_->PSSetSamplers(_slot, 1, pSamplers);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// Raw 오버로드 — 같은 캐시 필드(pCachedSamplers_) 공유
void GraphicContext::SetSamplerRaw(ShaderStage _stage, _u32 _slot, ID3D11SamplerState* _pRaw)
{
	jc_assert_msg(_slot < MAX_TEXTURE_SLOTS, "샘플러 슬롯 범위를 벗어났습니다.");

	const _s32 stageIndex = static_cast<_s32>(_stage);
	if (_pRaw == pCachedSamplers_[stageIndex][_slot])
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedSamplers_[stageIndex][_slot] = _pRaw;
	apiCallCount_ += 1;

	ID3D11SamplerState* pSamplers[] = { _pRaw };
	if (_stage == ShaderStage::ssVertex)
	{
		pContext_->VSSetSamplers(_slot, 1, pSamplers);
	}
	else
	{
		pContext_->PSSetSamplers(_slot, 1, pSamplers);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 뷰포트는 값이 작아 비교 비용이 더 크므로 캐시 없이 항상 적용한다.
void GraphicContext::SetViewport(const Viewport& _viewport)
{
	D3D11_VIEWPORT vp;
	vp.TopLeftX = _viewport.x_;
	vp.TopLeftY = _viewport.y_;
	vp.Width = _viewport.width_;
	vp.Height = _viewport.height_;
	vp.MinDepth = _viewport.minDepth_;
	vp.MaxDepth = _viewport.maxDepth_;

	apiCallCount_ += 1;
	pContext_->RSSetViewports(1, &vp);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetRasterizerState(RasterizerState* _pState)
{
	ID3D11RasterizerState* pRaw = (_pState != nullptr) ? _pState->Raw() : nullptr;
	if (pRaw == pCachedRasterizer_)
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedRasterizer_ = pRaw;
	apiCallCount_ += 1;
	pContext_->RSSetState(pRaw);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Raw 오버로드 — RenderStates 풀에서 나온 원시 포인터를 직접 받는다.
void GraphicContext::SetRasterizerStateRaw(ID3D11RasterizerState* _pRaw)
{
	if (_pRaw == pCachedRasterizer_)
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedRasterizer_ = _pRaw;
	apiCallCount_ += 1;
	pContext_->RSSetState(_pRaw);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetBlendState(BlendState* _pState)
{
	ID3D11BlendState* pRaw = (_pState != nullptr) ? _pState->Raw() : nullptr;
	if (pRaw == pCachedBlend_)
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedBlend_ = pRaw;
	apiCallCount_ += 1;
	pContext_->OMSetBlendState(pRaw, nullptr, 0xFFFFFFFF);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Raw 오버로드 — 같은 캐시 필드(pCachedBlend_) 공유
void GraphicContext::SetBlendStateRaw(ID3D11BlendState* _pRaw)
{
	if (_pRaw == pCachedBlend_)
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedBlend_ = _pRaw;
	apiCallCount_ += 1;
	pContext_->OMSetBlendState(_pRaw, nullptr, 0xFFFFFFFF);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetDepthStencilState(DepthStencilState* _pState)
{
	ID3D11DepthStencilState* pRaw = (_pState != nullptr) ? _pState->Raw() : nullptr;
	if (pRaw == pCachedDepth_)
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedDepth_ = pRaw;
	apiCallCount_ += 1;
	pContext_->OMSetDepthStencilState(pRaw, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Raw 오버로드 — 같은 캐시 필드(pCachedDepth_) 공유
void GraphicContext::SetDepthStencilStateRaw(ID3D11DepthStencilState* _pRaw)
{
	if (_pRaw == pCachedDepth_)
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedDepth_ = _pRaw;
	apiCallCount_ += 1;
	pContext_->OMSetDepthStencilState(_pRaw, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetRenderTargets(ID3D11RenderTargetView* const* _ppRtvs, _u32 _count, ID3D11DepthStencilView* _pDsv)
{
	ID3D11RenderTargetView* pFirstRtv = (_count > 0 && _ppRtvs != nullptr) ? _ppRtvs[0] : nullptr;
	if (pFirstRtv == pCachedRtv_ && _pDsv == pCachedDsv_ && _count == cachedRtvCount_)
	{
		skippedCallCount_ += 1;
		return;
	}

	pCachedRtv_ = pFirstRtv;
	pCachedDsv_ = _pDsv;
	cachedRtvCount_ = _count;
	apiCallCount_ += 1;
	pContext_->OMSetRenderTargets(_count, _ppRtvs, _pDsv);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::ClearRenderTarget(ID3D11RenderTargetView* _pRtv, const color& _color)
{
	if (_pRtv == nullptr)
	{
		return;
	}

	apiCallCount_ += 1;
	const _f32 clearColor[4] = { _color.Rf(), _color.Gf(), _color.Bf(), _color.Af() };
	pContext_->ClearRenderTargetView(_pRtv, clearColor);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::ClearDepthStencil(ID3D11DepthStencilView* _pDsv, _f32 _depth, _u8 _stencil)
{
	if (_pDsv == nullptr)
	{
		return;
	}

	apiCallCount_ += 1;
	pContext_->ClearDepthStencilView(_pDsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, _depth, _stencil);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::ClearState()
{
	apiCallCount_ += 1;
	pContext_->ClearState();
	InvalidateCache();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GraphicContext::UpdateBuffer(ID3D11Buffer* _pBuffer, const void* _pData, _u32 _size)
{
	if (_pBuffer == nullptr || _pData == nullptr || _size == 0)
	{
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT hr = pContext_->Map(_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (FAILED(hr))
	{
		return false;
	}

	memcpy(mapped.pData, _pData, _size);
	pContext_->Unmap(_pBuffer, 0);
	apiCallCount_ += 1;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetRasterizer(CullMode _cull, FillMode _fill, FrontFace _front)
{
	if (pDevice_ == nullptr) { return; }
	SetRasterizerStateRaw(pDevice_->States().GetRasterizerState(_cull, _fill, _front));
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetBlend(BlendMode _mode)
{
	if (pDevice_ == nullptr) { return; }
	SetBlendStateRaw(pDevice_->States().GetBlendState(_mode));
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetDepth(DepthMode _mode)
{
	if (pDevice_ == nullptr) { return; }
	SetDepthStencilStateRaw(pDevice_->States().GetDepthState(_mode));
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetSampler(FilterMode _filter, AddressMode _address, _u32 _slot)
{
	if (pDevice_ == nullptr) { return; }
	SetSamplerRaw(ShaderStage::ssPixel, _slot, pDevice_->States().GetSamplerState(_filter, _address, _address));
}

//////////////////////////////////////////////////////////////////////////////////////////
// ★ 드로우콜 — "지금 바인딩된 상태로 정점 N개를 그려라"는 GPU 명령의 발사 버튼.
void GraphicContext::SetVertexShader(_u32 _vsHandle)
{
	if (pDevice_ == nullptr) return;
	VertexShader* pVs = pDevice_->ResolveVertexShader(_vsHandle);
	if (pVs == nullptr) return;
	SetVertexShader(pVs);
}

////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetPixelShader(_u32 _psHandle)
{
	if (pDevice_ == nullptr) return;
	PixelShader* pPs = pDevice_->ResolvePixelShader(_psHandle);
	if (pPs == nullptr) return;
	SetPixelShader(pPs);
}

////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetInputLayout(_u32 _vsHandle, _u32 _vbHandle)
{
	if (pDevice_ == nullptr) return;
	VertexShader* pVs = pDevice_->ResolveVertexShader(_vsHandle);
	GraphicDevice::VertexBufferSlot* pVbSlot = pDevice_->ResolveVertexBuffer(_vbHandle);
	jc_assert_msg(pVs != nullptr, "SetInputLayout: 유효하지 않은 VS 핸들");
	jc_assert_msg(pVbSlot != nullptr && pVbSlot->pBuffer != nullptr, "SetInputLayout: 유효하지 않은 VB 핸들");
	if (pVs == nullptr || pVbSlot == nullptr || pVbSlot->pBuffer == nullptr) return;
	VertexLayoutSpan layout = pVbSlot->pBuffer->Layout();
	InputLayout* pLayout = new InputLayout();
	if (!pLayout->Initialize(pDevice_, layout, pVs))
	{
		jc_assert_msg(false, "SetInputLayout: InputLayout 생성 실패 (VS와 Vertex 레이아웃 불일치)");
		delete pLayout;
		return;
	}
	SetInputLayout(pLayout);
	ownedInputLayouts_.PushBack(pLayout);
}

void GraphicContext::SetInputLayout(_u32 _vsHandle, VertexBuffer* _pVb)
{
	if (pDevice_ == nullptr || _pVb == nullptr) return;
	VertexShader* pVs = pDevice_->ResolveVertexShader(_vsHandle);
	jc_assert_msg(pVs != nullptr, "SetInputLayout: 유효하지 않은 VS 핸들");
	if (pVs == nullptr) return;
	VertexLayoutSpan layout = _pVb->Layout();
	InputLayout* pLayout = new InputLayout();
	if (!pLayout->Initialize(pDevice_, layout, pVs))
	{
		jc_assert_msg(false, "SetInputLayout: InputLayout 생성 실패 (VS와 Vertex 레이아웃 불일치)");
		delete pLayout;
		return;
	}
	SetInputLayout(pLayout);
	ownedInputLayouts_.PushBack(pLayout);
}

void GraphicContext::SetInputLayout(_u32 _vsHandle, VertexLayoutSpan _layout)
{
	if (pDevice_ == nullptr) return;
	VertexShader* pVs = pDevice_->ResolveVertexShader(_vsHandle);
	jc_assert_msg(pVs != nullptr, "SetInputLayout: 유효하지 않은 VS 핸들");
	if (pVs == nullptr) return;
	InputLayout* pLayout = new InputLayout();
	if (!pLayout->Initialize(pDevice_, _layout, pVs))
	{
		jc_assert_msg(false, "SetInputLayout: InputLayout 생성 실패 (VS와 Vertex 레이아웃 불일치)");
		delete pLayout;
		return;
	}
	SetInputLayout(pLayout);
	ownedInputLayouts_.PushBack(pLayout);
}

////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetVertexBuffer(_u32 _handle)
{
	if (pDevice_ == nullptr)
	{
		return;
	}
	GraphicDevice::VertexBufferSlot* pSlot = pDevice_->ResolveVertexBuffer(_handle);
	if (pSlot == nullptr)
	{
		return;
	}
	SetVertexBuffer(pSlot->pBuffer);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetIndexBuffer(_u32 _handle)
{
	if (pDevice_ == nullptr)
	{
		return;
	}
	IndexBuffer* pBuffer = pDevice_->ResolveIndexBuffer(_handle);
	if (pBuffer == nullptr)
	{
		return;
	}
	SetIndexBuffer(pBuffer);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::SetTexture(_u32 _slot, _u32 _handle)
{
	if (pDevice_ == nullptr)
	{
		return;
	}
	Texture* pTexture = pDevice_->ResolveTexture(_handle);
	SetTexture(ShaderStage::ssPixel, _slot, pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::Draw(_u32 _vertexCount, _u32 _startVertex)
{
	jc_assert_msg(pCachedInputLayout_ != nullptr, "InputLayout 미바인딩: Draw 전에 gc.SetInputLayout(vsHandle, vbHandle) 또는 gc.SetInputLayout(pLayout)을 호출하세요.");
	apiCallCount_ += 1;
	pContext_->Draw(_vertexCount, _startVertex);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::DrawIndexed(_u32 _indexCount, _u32 _startIndex, _s32 _baseVertex)
{
	jc_assert_msg(pCachedInputLayout_ != nullptr, "InputLayout 미바인딩: DrawIndexed 전에 gc.SetInputLayout(vsHandle, vbHandle) 또는 gc.SetInputLayout(pLayout)을 호출하세요.");
	apiCallCount_ += 1;
	pContext_->DrawIndexed(_indexCount, _startIndex, _baseVertex);
}

NS_SGF_END
