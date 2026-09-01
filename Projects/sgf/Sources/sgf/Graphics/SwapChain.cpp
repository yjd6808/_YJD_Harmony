/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 1:00:00 AM
 * =====================
 * 스왑체인 래퍼 구현부
 */

#include "Core.h"
#include "sgf/Graphics/SwapChain.h"
#include "sgf/Graphics/GraphicContext.h"

NS_SGF_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
SwapChain::SwapChain()
	: width_(0)
	, height_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 스왑체인 소유권 인수 + 백버퍼 RTV 생성
bool SwapChain::Initialize(IDXGISwapChain* _pSwapChain, ID3D11Device* _pDevice, _s32 _width, _s32 _height)
{
	if (_pSwapChain == nullptr || _pDevice == nullptr)
	{
		return false;
	}

	// 재초기화 대비: 이미 보유 중이던 스왑체인/RTV를 먼저 정리한다.
	// (Attach는 기존 포인터를 Release하지 않으므로 그대로 두면 누수된다)
	Finalize();

	// Attach: 참조카운트를 올리지 않고 소유권만 넘겨받는다.
	// (생성 함수가 이미 참조카운트 1을 부여한 상태로 넘겨주기 때문)
	pSwapChain_.Attach(_pSwapChain);
	width_ = _width;
	height_ = _height;

	// 백버퍼 텍스처를 꺼내 그리기 대상 뷰(RTV)를 만든다.
	// 실패하면 소유한 스왑체인까지 정리해 호출자(Window)가 일관된 상태로 복구할 수 있게 한다.
	SgfComPtr<ID3D11Texture2D> pBackBuffer;
	if (FAILED(pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(pBackBuffer.GetAddressOf()))))
	{
		Finalize();
		return false;
	}
	if (FAILED(_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr,
		pRenderTargetView_.GetAddressOf())))
	{
		Finalize();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SwapChain::Finalize()
{
	pRenderTargetView_.Reset();
	pSwapChain_.Reset();
	width_ = 0;
	height_ = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 백버퍼를 화면에 표시 (더블 버퍼링의 "교체" 단계)
void SwapChain::Present(bool _vsync)
{
	if (pSwapChain_)
	{
		// 첫 인자(SyncInterval): 1이면 수직동기화 대기, 0이면 즉시 표시
		pSwapChain_->Present(_vsync ? 1 : 0, 0);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 백버퍼 전체를 그리기 영역으로 설정 - GraphicContext 캐시를 통과한다
void SwapChain::ApplyFullViewport(GraphicContext& _context) const
{
	Viewport viewport(0.0f, 0.0f, static_cast<_f32>(width_), static_cast<_f32>(height_));
	_context.SetViewport(viewport);
}

NS_SGF_END
