/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:10:00 PM
 * =====================
 * 그래픽 컨텍스트 (디바이스와 그리기 명령 분리)
 *
 * [왜 Device와 Context를 분리하나?]
 * - GraphicDevice: 리소스 생성 담당 ("공장")
 * - GraphicContext: 그리기 명령 담당 ("작업반장")
 * D3D11도 ID3D11Device / ID3D11DeviceContext로 나뉘어 있다.
 * 역할이 다르므로 클래스도 나눈다. (멀티스레드 렌더링 확장 대비)
 *
 * [바인딩 캐시 = 스테이트 캐시]
 * 같은 객체를 연속으로 다시 묶으면 D3D 호출을 생략한다.
 * GetApiCallCount / GetSkippedCallCount로 절감 효과를 직접 확인할 수 있다.
 * (튜토리얼 23장: 스테이트 캐시 Before/After)
 */

#pragma once

#include <d3d11.h>
#include "jc/Primitives/Span.h"
#include "sgf/Graphics/GraphicsEnums.h"
#include "sgf/Graphics/PipelineState.h"
#include "sgf/Graphics/Vertex.h"

#ifndef SGF_INVALID_HANDLE_DEFINED
#define SGF_INVALID_HANDLE_DEFINED
constexpr _u32 INVALID_HANDLE = 0xFFFFFFFF;
#endif

NS_SGF_BEGIN

using namespace jc;

class GraphicDevice;
class Texture;
class VertexShader;
class PixelShader;
class InputLayout;
class VertexBuffer;
class IndexBuffer;

// 뷰포트: 백버퍼의 어느 영역에 NDC로 펼칠지 (값 타입. §3.3)
struct Viewport
{
	_f32 x_ = 0.0f;
	_f32 y_ = 0.0f;
	_f32 width_ = 0.0f;
	_f32 height_ = 0.0f;
	_f32 minDepth_ = 0.0f;
	_f32 maxDepth_ = 1.0f;

	Viewport() = default;
	Viewport(_f32 _x, _f32 _y, _f32 _width, _f32 _height, _f32 _minDepth = 0.0f, _f32 _maxDepth = 1.0f)
		: x_(_x), y_(_y), width_(_width), height_(_height), minDepth_(_minDepth), maxDepth_(_maxDepth)
	{
	}
};

class GraphicContext
{
public:
	static constexpr _u32 MAX_TEXTURE_SLOTS = 8;	// 동시 바인딩 텍스처 수
	static constexpr _u32 MAX_CBUFFER_SLOTS = 8;	// 동시 바인딩 상수버퍼 수

public:
	GraphicContext();
	~GraphicContext();
	GraphicContext(const GraphicContext&) = delete;
	GraphicContext& operator=(const GraphicContext&) = delete;

	// 즉시 컨텍스트를 넘겨받는다 (공유 소유 - SgfComPtr AddRef). 디바이스가 내부 참조를 유지하므로 Release는 참조 해제일 뿐이다.
	bool InitializeImmediate(GraphicDevice* _pDevice, ID3D11DeviceContext* _pImmediate);
	// 워커 스레드용 - 직접 만들고 배타적으로 소유한다
	bool InitializeDeferred(GraphicDevice* _pDevice);
	// [삭제 예정] 하위 호환 - InitializeImmediate로 위임 (디바이스의 즉시 컨텍스트를 사용)
	// 최종 설계 §4-3에는 없는 래퍼. A단계에서는 유지하되 B에서 호출부를 InitializeImmediate로 교체 후 삭제 예정.
	bool Initialize(GraphicDevice* _pDevice);
	void Finalize();
	bool IsDeferred() const { return deferred_; }

	//////////////////////////////////////////////////////////////////////////////////////////
	// IA 단계
	void SetVertexBuffer(VertexBuffer* _pBuffer);
	void SetVertexBuffer(_u32 _handle);
	void SetIndexBuffer(IndexBuffer* _pBuffer);
	void SetIndexBuffer(_u32 _handle);
	void SetInputLayout(InputLayout* _pLayout);
	void SetPrimitiveTopology(PrimitiveTopology _topology);
	void SetTopology(PrimitiveTopology _topology) { SetPrimitiveTopology(_topology); }

	//////////////////////////////////////////////////////////////////////////////////////////
	// 셰이더 단계 — 분리형
	_u32 CreateVertexShader(const jc::String& _hlslSource, const jc::String& _entry = "VSMain");
	_u32 CreatePixelShader(const jc::String& _hlslSource, const jc::String& _entry = "PSMain");
	void SetVertexShader(VertexShader* _pShader);
	void SetPixelShader(PixelShader* _pShader);
	void SetVertexShader(_u32 _vsHandle);
	void SetPixelShader(_u32 _psHandle);
	void SetInputLayout(_u32 _vsHandle, _u32 _vbHandle);
	void SetInputLayout(_u32 _vsHandle, VertexBuffer* _pVb);
	void SetInputLayout(_u32 _vsHandle, VertexLayoutSpan _layout);
	void SetConstantBuffer(ShaderStage _stage, _u32 _slot, ID3D11Buffer* _pBuffer);
	void SetTexture(ShaderStage _stage, _u32 _slot, Texture* _pTexture);
	void SetTexture(_u32 _slot, _u32 _handle);
	void SetSampler(ShaderStage _stage, _u32 _slot, SamplerState* _pSampler);

	//////////////////////////////////////////////////////////////////////////////////////////
	// RS/OM 단계
	void SetViewport(const Viewport& _viewport);
	void SetRasterizerState(RasterizerState* _pState);
	void SetBlendState(BlendState* _pState);
	void SetDepthStencilState(DepthStencilState* _pState);
	void SetRenderTargets(ID3D11RenderTargetView* const* _ppRtvs, _u32 _count, ID3D11DepthStencilView* _pDsv);
	void ClearRenderTarget(ID3D11RenderTargetView* _pRtv, const color& _color);
	void ClearDepthStencil(ID3D11DepthStencilView* _pDsv, _f32 _depth = 1.0f, _u8 _stencil = 0);
	void ClearState();
	bool UpdateBuffer(ID3D11Buffer* _pBuffer, const void* _pData, _u32 _size);

	//////////////////////////////////////////////////////////////////////////////////////////
	// RS/OM 단계 — Raw 오버로드 (RenderStates 풀에서 나온 원시 D3D 포인터를 직접 받는다)
	void SetRasterizerStateRaw(ID3D11RasterizerState* _pRaw);
	void SetBlendStateRaw(ID3D11BlendState* _pRaw);
	void SetDepthStencilStateRaw(ID3D11DepthStencilState* _pRaw);
	void SetSamplerRaw(ShaderStage _stage, _u32 _slot, ID3D11SamplerState* _pRaw);

	// RS/OM 단계 — enum 설탕 (내부에서 RenderStates 조회, 캐시 공유)
	void SetRasterizer(CullMode _cull, FillMode _fill = FillMode::fmSolid, FrontFace _front = FrontFace::ffClockwise);
	void SetBlend(BlendMode _mode);
	void SetDepth(DepthMode _mode);
	void SetSampler(FilterMode _filter, AddressMode _address, _u32 _slot = 0);

	//////////////////////////////////////////////////////////////////////////////////////////
	// 그리기
	void Draw(_u32 _vertexCount, _u32 _startVertex = 0);
	void DrawIndexed(_u32 _indexCount, _u32 _startIndex = 0, _s32 _baseVertex = 0);

	//////////////////////////////////////////////////////////////////////////////////////////
	// 캐시 관리/진단
	// 프레임 시작 시 호출. 외부(BeginFrame 등)에서 상태가 바뀌었을 수 있으므로 캐시를 비운다.
	void InvalidateCache();
	_u64 GetApiCallCount() const { return apiCallCount_; }			// 실제 D3D 호출 수
	_u64 GetSkippedCallCount() const { return skippedCallCount_; }	// 캐시로 생략된 호출 수
	void ResetStats();

	ID3D11DeviceContext* Raw() const { return pContext_.Get(); }

private:
	SgfComPtr<ID3D11DeviceContext> pContext_;	// 즉시/지연 컨텍스트 (SgfComPtr 공유/배타 소유)
	GraphicDevice* pDevice_ = nullptr;			// 생성에 사용한 디바이스 (소유하지 않음)
	bool deferred_ = false;					// true면 deferred (배타 소유), false면 immediate (공유 소유)

	// 렌더 타깃 캐시 (OMSetRenderTargets 왕복 방지)
	ID3D11RenderTargetView* pCachedRtv_ = nullptr;
	ID3D11DepthStencilView* pCachedDsv_ = nullptr;
	_u32 cachedRtvCount_ = 0;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 바인딩 캐시 (마지막으로 묶은 객체 기억)
	ID3D11Buffer* pCachedVertexBuffer_;
	ID3D11Buffer* pCachedIndexBuffer_;
	ID3D11InputLayout* pCachedInputLayout_;
	ID3D11VertexShader* pCachedVs_;
	ID3D11PixelShader* pCachedPs_;
	ID3D11RasterizerState* pCachedRasterizer_;
	ID3D11BlendState* pCachedBlend_;
	ID3D11DepthStencilState* pCachedDepth_;
	ID3D11ShaderResourceView* pCachedSrvs_[static_cast<_s32>(ShaderStage::Max)][MAX_TEXTURE_SLOTS];
	ID3D11SamplerState* pCachedSamplers_[static_cast<_s32>(ShaderStage::Max)][MAX_TEXTURE_SLOTS];
	ID3D11Buffer* pCachedCbuffers_[static_cast<_s32>(ShaderStage::Max)][MAX_CBUFFER_SLOTS];
	PrimitiveTopology cachedTopology_;

	jc::Vector<InputLayout*> ownedInputLayouts_; // SetInputLayout(handle,handle)이 생성한 InputLayout 소유 (Finalize에서 해제)

	// InputLayout은 반드시 수동으로 지정해야 한다. Draw 전 미바인딩 시 assert.

	//////////////////////////////////////////////////////////////////////////////////////////
	// 진단 카운터
	_u64 apiCallCount_;		// 실제로 내려간 D3D 호출 수
	_u64 skippedCallCount_;	// 캐시 덕분에 생략된 호출 수
};

NS_SGF_END
