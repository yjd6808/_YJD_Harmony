/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:38:00 PM
 * 수정일: 8/9/2026 9:30:00 AM (멀티 윈도우 - 표면은 Window 소유, 디바이스는 앱에 1개)
 * =====================
 * D3D11 그래픽 디바이스 (파사드)
 *
 * [구조 - D3D 개체 뭉침 해소]
 * 기존에는 디바이스/스왕체인/깊이버퍼/상태객체가 전부 이 클래스 안에 있었다.
 * 이제 역할별로 분리하고, GraphicDevice는 이들을 조립해 기존과 똑같은
 * 간편 API를 제공하는 "파사드(facade)"가 되었다.
 * - SwapChain: 백버퍼 교체(Present) + 백버퍼 RTV
 * - DepthStencilSurface: 깊이/스텐실 버퍼 + DSV
 * - RenderStates: 블렌드/깊이/래스터라이저/샘플러 상태 캐시
 * - GraphicDevice: ID3D11Device/Context 소유 + 부품 조립 + 간편 API
 *
 * [구조 - 멀티 윈도우]
 * 디바이스(공장)는 앱 전체에 1개면 충분하지만, 백버퍼는 창마다 필요하다.
 * 그래서 표면(스왕체인+깊이버퍼) 소유권을 Window로 옮겼다.
 * - Initialize(): 디바이스만 생성 (Application 경로)
 * - Window::CreateSurface(): 창마다 자기 표면 생성
 * - BeginFrame(Window*, color): 해당 창의 표면을 묶고 지운다
 * - EndFrame(Window*, vsync): 해당 창의 백버퍼를 표시한다
 * 기존 튜토리얼(1~13, 15~22장) 호환을 위해 단일 창용 구버전 API도 그대로 남겨둔다.
 * - Initialize(HWND, w, h) / BeginFrame(color) / EndFrame(vsync)
 *
 * [파사드(facade) 패턴이란?]
 * 여러 부품을 감싸서 단순한 창구 하나로 노출하는 설계.
 * 사용하는 쪽 코드는 바뀌지 않고, 내부는 부품별로 작고 이해하기 쉬워진다.
 */

#pragma once

#include "jc/Math.h"
#include "sgf/Graphics/RenderStates.h"
#include "sgf/Graphics/SwapChain.h"
#include "sgf/Graphics/DepthStencilSurface.h"
#include "sgf/Graphics/GraphicContext.h"

NS_SGF_BEGIN

using namespace jc;

class RenderTarget;
class Window;

class GraphicDevice
{
public:
	// 하위 호환: 기존 GraphicDevice::BlendMode::bmAlpha 같은 표기를 그대로 쓸 수 있게
	// 네임스페이스로 옮겨진 열거형(RenderStates.h)에 별칭을 붙인다.
	using BlendMode = ::sgf::BlendMode;
	using SamplerFilter = ::sgf::SamplerFilter;
	using SamplerAddress = ::sgf::SamplerAddress;
	using CullMode = ::sgf::CullMode;

public:
	GraphicDevice();
	~GraphicDevice();

	// 디바이스/컨텍스트와 상태 캐시만 초기화한다. (표면 없음)
	// 멀티 윈도우 경로: 이후 각 Window가 CreateSurface로 자기 표면을 만든다.
	bool Initialize();

	// [하위 호환] 디바이스 + 지정 창의 내장 표면까지 한 번에 초기화한다.
	// (1~13, 15~22장 튜토리얼이 사용하는 단일 창 경로)
	// @param _hWnd: 출력 대상 윈도우 핸들
	// @param _width: 백버퍼 가로 크기
	// @param _height: 백버퍼 세로 크기
	bool Initialize(HWND _hWnd, _s32 _width, _s32 _height);

	// 모든 D3D 객체를 해제한다. (ComPtr이므로 자동 Release)
	void Finalize();

	// 지정 윈도우의 표면을 그리기 대상으로 묶고 지운다.
	// @param _pWindow: 그릴 대상 창 (CreateSurface가 되어 있어야 함)
	// @param _clearColor: 화면을 채울 배경색
	void BeginFrame(Window* _pWindow, const color& _clearColor);

	// 지정 윈도우의 백버퍼를 화면에 표시한다.
	void EndFrame(Window* _pWindow, bool _vsync = true);

	// [하위 호환] 내장 표면(단일 창 경로)을 지우고 그릴 준비를 한다.
	void BeginFrame(const color& _clearColor);

	// [하위 호환] 내장 표면의 백버퍼를 화면에 표시한다.
	void EndFrame(bool _vsync = true);

	// 알파 블렌딩(반투명 합성) 켜기/끄기
	void SetAlphaBlending(bool _enable);

	// 깊이 테스트 켜기/끄기 (3D는 켜고, 2D는 보통 끄고 그리는 순서로 적층)
	void SetDepthTest(bool _enable);

	// 블렌드 모드를 설정한다. SetAlphaBlending보다 세분화된 버전.
	void SetBlendMode(BlendMode _mode);

	// 샘플러(필터 + 주소 모드)를 지정 슬롯에 설정한다.
	void SetSampler(SamplerFilter _filter, SamplerAddress _address, UINT _slot = 0);

	// 와이어프레임(면 대신 선만 그리기) 켜기/끄기. 현재 컬링 모드는 유지된다.
	void SetWireframe(bool _enable);

	// 컬링 모드를 설정한다. 현재 와이어프레임 여부는 유지된다.
	void SetCullMode(CullMode _mode);

	// 그리기 대상을 교체한다. (25~27번 튜토리얼: 렌더 타깃/후처리/그림자 맵)
	// @param _pTarget: 바꿀 대상. nullptr이면 "현재 창의 백버퍼"로 복귀한다.
	// (BeginFrame(Window*)로 그리던 중이면 그 창의 표면으로,
	// 구버전 경로면 내장 표면으로 복귀한다)
	void SetRenderTarget(RenderTarget* _pTarget);

	// 서브 윈도우용 스왕체인을 만들어준다. (Window::CreateSurface가 호출)
	// 디바이스를 만든 DXGI 팩토리를 역추적(QueryInterface)해서 사용한다.
	// @param _ppOutSwapChain: 생성된 스왕체인 (소유권은 호출자에게)
	bool CreateSwapChainForWindow(HWND _hWnd, _s32 _width, _s32 _height, IDXGISwapChain** _ppOutSwapChain);

	ID3D11Device* Device() const { return pDevice_.Get(); }
	ID3D11DeviceContext* Context() const { return pContext_.Get(); }

	// 현재 그리기 대상 표면의 크기 (BeginFrame에서 갱신된다)
	_s32 Width() const { return width_; }
	_s32 Height() const { return height_; }

	////////////////////////////////////////////////////////////////////////////////////////
	// 부품 직접 접근 (고급 제어/멀티스레드 연구용)
	// [주의] 이 둘은 "구버전 단일 창 경로"의 내장 표면이다.
	// 멀티 윈도우 경로에서는 Window::GetSwapChain()을 사용하라.
	SwapChain& GetSwapChain() { return swapChain_; }
	DepthStencilSurface& GetDepthSurface() { return depthSurface_; }
	RenderStates& States() { return states_; }

	// 그리기 명령 창구. 바인딩 캐시(스테이트 캐시)를 내장한다.
	GraphicContext& GetContext() { return context_; }

private:
	// 디바이스만 생성한다. (공통 경로)
	bool CreateDeviceOnly();

	// 디바이스/컨텍스트와 스왕체인을 생성해 내장 SwapChain 부품에 넘긴다. (구버전 경로)
	bool CreateDeviceAndSwapChain(HWND _hWnd);

	// 지정 창의 표면(백버퍼 RTV + 깊이 DSV)을 그리기 대상으로 묶고 뷰포트를 화면 크기로 채운다.
	// (BeginFrame(Window*)와 SetRenderTarget(nullptr)의 공통 헬퍼)
	void BindWindowSurface(Window* _pWindow);

	// 내장 표면(백버퍼 RTV + 깊이 DSV)을 그리기 대상으로 묶고 뷰포트를 화면 크기로 채운다.
	// (구버전 경로: BeginFrame(color)와 SetRenderTarget(nullptr)의 공통 헬퍼)
	void BindBackBufferSurface();

	// 현재 와이어프레임/컬링 설정에 맞는 래스터라이저 상태를 적용한다.
	void ApplyRasterizerState();

private:
	SgfComPtr<ID3D11Device> pDevice_;			// 리소스 생성 담당 ("공장")
	SgfComPtr<ID3D11DeviceContext> pContext_;	// 그리기 명령 담당 ("작업반장")

	SwapChain swapChain_;				// [구버전 경로 전용] 내장 백버퍼 표면
	DepthStencilSurface depthSurface_;	// [구버전 경로 전용] 내장 깊이 버퍼
	RenderStates states_;		// 상태 객체 캐시
	GraphicContext context_;	// 그리기 명령 창구 (바인딩 캐시 포함)

	Window* pBoundWindow_;	// 현재 BeginFrame(Window*)로 묶인 창 (구버전 경로면 nullptr)
	bool wireframe_;		// 현재 와이어프레임 여부
	CullMode cullMode_;		// 현재 컬링 모드
	_s32 width_;			// 현재 그리기 대상 가로
	_s32 height_;			// 현재 그리기 대상 세로
};

NS_SGF_END
