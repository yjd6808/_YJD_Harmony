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
#include "sgf/Graphics/VertexDeclaration.h"
#include "sgf/Graphics/Vertex.h"
#include "sgf/Graphics/RenderStates.h"
#include "sgf/Graphics/SwapChain.h"
#include "sgf/Graphics/DepthStencilSurface.h"
#include "sgf/Graphics/GraphicContext.h"
#include "sgf/Graphics/IResource.h"
#include "sgf/Graphics/IResourceRegistry.h"

NS_SGF_BEGIN

using namespace jc;

class RenderTarget;
class Window;
class VertexBuffer;
class IndexBuffer;
class Texture;
class VertexShader;
class PixelShader;
class VertexDeclaration;

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

	// ---- 리소스 레지스트리
	// C안: 반환 키는 _u64 Resource Key [type 8][gen 24][index 32], 실패 시 INVALID_RESOURCE_KEY(0)
	_u64 CreateVertexShader(const jc::String& _hlslSource, const jc::String& _entry = "VSMain");
	_u64 CreatePixelShader(const jc::String& _hlslSource, const jc::String& _entry = "PSMain");
	template <typename TVertex>
	_u64 CreateVertexBuffer(const TVertex* _pVertices, _u32 _count, ResourceUsage _usage = ResourceUsage::ruDefault)
	{
		jc_assert(IsValid());
		return CreateVertexBufferInternal(_pVertices, _count, _usage, TVertex::Decl());
	}
	_u64 CreateIndexBuffer(const _u32* _pIndices, _u32 _count, ResourceUsage _usage = ResourceUsage::ruDefault);
	_u64 CreateTexture(_s32 _width, _s32 _height, const _u8* _pPixels, PixelFormat _format = PixelFormat::pfRgba8);

	// 레지스트리 주입 (ResourceMgr::Initialize가 호출)
	void SetResourceRegistry(IResourceRegistry* _pRegistry) { pRegistry_ = _pRegistry; }
	IResourceRegistry* GetResourceRegistry() const { return pRegistry_; }

	// ---- InputLayout 캐시 (Device 소유 — 선언 × VS 시그니처 조합) ----
	ID3D11InputLayout* GetOrCreateInputLayout(const VertexDeclaration* _pDecl, VertexShader* _pVs);

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

	// ---- 레지스트리 조회 (GraphicContext 전용) — _u64 키 ----
	VertexShader* ResolveVertexShader(_u64 _key);
	PixelShader* ResolvePixelShader(_u64 _key);
	VertexBuffer* ResolveVertexBuffer(_u64 _key);
	IndexBuffer* ResolveIndexBuffer(_u64 _key);
	Texture* ResolveTexture(_u64 _key);

private:
	bool CreateDeviceOnly();
	_u64 CreateVertexBufferInternal(const void* _pVertices, _u32 _count, ResourceUsage _usage, const VertexDeclaration* _pDecl);
	bool EnsureRegistry() const;

	template <typename T>
	T* ResolveTemplate(_u64 _key);

private:
	SgfComPtr<ID3D11Device> pDevice_;
	GraphicContext context_;
	SwapChain swapChain_;
	DepthStencilSurface depthSurface_;
	RenderStates states_;
	IResourceRegistry* pRegistry_ = nullptr; // 소유하지 않음 — ResourceMgr이 주입

	// InputLayout 캐시 — 키 = (선언 해시 × VS 시그니처 해시)
	struct InputLayoutEntry
	{
		_u64 declHash_ = 0;
		_u64 sigHash_ = 0;
		SgfComPtr<ID3D11InputLayout> pLayout_;
	};
	jc::Vector<InputLayoutEntry> inputLayoutCache_;
};

NS_SGF_END
