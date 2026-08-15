/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:20:00 PM
 * =====================
 * 그래픽 컨텍스트 구현부
 *
 * [캐시 규칙]
 *  모든 Set 함수는 "마지막으로 묶은 D3D 객체"와 비교해 같으면 생략한다.
 *  생략 여부는 skippedCallCount_로 집계되어 튜토리얼 23장에서 직접 확인할 수 있다.
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
{
	InvalidateCache();
	apiCallCount_ = 0;
	skippedCallCount_ = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GraphicContext::Initialize(GraphicDevice* _pDevice)
{
	if (_pDevice == nullptr || _pDevice->Context() == nullptr)
	{
		return false;
	}

	pContext_ = _pDevice->Context();
	InvalidateCache();
	ResetStats();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::Finalize()
{
	pContext_ = nullptr;
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

//////////////////////////////////////////////////////////////////////////////////////////
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
void GraphicContext::Draw(_u32 _vertexCount, _u32 _startVertex)
{
	apiCallCount_ += 1;
	pContext_->Draw(_vertexCount, _startVertex);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GraphicContext::DrawIndexed(_u32 _indexCount, _u32 _startIndex, _s32 _baseVertex)
{
	apiCallCount_ += 1;
	pContext_->DrawIndexed(_indexCount, _startIndex, _baseVertex);
}

NS_SGF_END
