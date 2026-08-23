/*
 * 작성자: 윤정도
 * =====================
 * 디바이스 = 수명 · 생성/소유 · 화면 표면 · 프레임 괄호
 * 컨텍스트 = 바인딩 · 상태 · 드로우            (GraphicContext.h)
 */

#pragma once

#include "jc/Math.h"
#include "jc/Container/Vector.h"
#include "jc/Primitives/Span.h"
#include "sgf/Graphics/Vertex.h"
#include "sgf/Graphics/RenderStates.h"
#include "sgf/Graphics/SwapChain.h"
#include "sgf/Graphics/DepthStencilSurface.h"
#include "sgf/Graphics/GraphicContext.h"

NS_SGF_BEGIN

using namespace jc;

#ifndef SGF_INVALID_HANDLE_DEFINED
#define SGF_INVALID_HANDLE_DEFINED
constexpr _u32 INVALID_HANDLE = 0xFFFFFFFF;
#endif

class RenderTarget;
class Window;
class VertexBuffer;
class IndexBuffer;
class Texture;
class VertexShader;
class PixelShader;
class InputLayout;

class GraphicDevice
{
public:
	GraphicDevice();
	~GraphicDevice();
	GraphicDevice(const GraphicDevice&) = delete;
	GraphicDevice& operator=(const GraphicDevice&) = delete;

	static GraphicDevice& Get();				// 잘 알려진 기본 인스턴스 (Meyers static, 지연 초기화 없음)

	bool IsValid() const { return pDevice_ != nullptr; }

	// ---- 수명 ----
	bool Initialize();
	void Finalize();

	// ---- 화면 표면 ----
	bool CreateSwapChain(HWND _hWnd, _s32 _width, _s32 _height, PixelFormat _format = PixelFormat::pfRgba8);
	bool CreateSwapChainForWindow(HWND _hWnd, _s32 _width, _s32 _height, PixelFormat _format, IDXGISwapChain** _ppOutSwapChain);

	// ---- 리소스 레지스트리 (핸들 = Vector 인덱스, 생성 = 즉시 current) ----
	struct VertexBufferSlot { VertexBuffer* pBuffer = nullptr; };

	_u32 CreateVertexShader(const jc::String& _hlslSource, const jc::String& _entry = "VSMain");
	_u32 CreatePixelShader(const jc::String& _hlslSource, const jc::String& _entry = "PSMain");
	template <typename TVertex>
	_u32 CreateVertexBuffer(const TVertex* _pVertices, _u32 _count, ResourceUsage _usage = ResourceUsage::ruDefault)
	{
		jc_assert(IsValid());
		return CreateVertexBufferInternal(_pVertices, sizeof(TVertex), _count, _usage, TVertex::Layout());
	}
	_u32 CreateIndexBuffer(const _u32* _pIndices, _u32 _count, ResourceUsage _usage = ResourceUsage::ruDefault);
	_u32 CreateTexture(_s32 _width, _s32 _height, const _u8* _pPixels, PixelFormat _format = PixelFormat::pfRgba8);

	// ---- 프레임 괄호 (표면 소유자) ----
	void BeginFrame(const color& _clearColor);
	void BeginFrame(SwapChain& _swapChain, DepthStencilSurface* _pDepth, const color& _clearColor);
	void Present(bool _vsync);

	void SetRenderTarget(RenderTarget* _pTarget);

	// ---- 접근자 ----
	ID3D11Device* Device() const { return pDevice_.Get(); }
	GraphicContext& Context() { return context_; }
	const GraphicContext& Context() const { return context_; }
	RenderStates& States() { return states_; }
	SwapChain& GetSwapChain() { return swapChain_; }
	DepthStencilSurface& GetDepthSurface() { return depthSurface_; }
	void SetDebugName(ID3D11DeviceChild* _pChild, const jc::String& _name);

	// ---- 레지스트리 조회 (GraphicContext 전용) ----
	VertexShader* ResolveVertexShader(_u32 _handle);
	PixelShader* ResolvePixelShader(_u32 _handle);
	VertexBufferSlot* ResolveVertexBuffer(_u32 _handle);
	IndexBuffer* ResolveIndexBuffer(_u32 _handle);
	Texture* ResolveTexture(_u32 _handle);

private:
	bool CreateDeviceOnly();
	_u32 CreateVertexBufferInternal(const void* _pVertices, UINT _stride, _u32 _count, ResourceUsage _usage, VertexLayoutSpan _layout);

private:
	SgfComPtr<ID3D11Device> pDevice_;
	GraphicContext context_;
	SwapChain swapChain_;
	DepthStencilSurface depthSurface_;
	RenderStates states_;

	jc::Vector<VertexShader*> vertexShaders_;
	jc::Vector<PixelShader*> pixelShaders_;
	jc::Vector<VertexBufferSlot> vertexBuffers_;
	jc::Vector<IndexBuffer*> indexBuffers_;
	jc::Vector<Texture*> textures_;
};

NS_SGF_END
