/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 1:00:00 AM
 * =====================
 * 스왑체인 래퍼 구현부
 */

#include "Core.h"
#include "sgf/Graphics/SwapChain.h"

NS_SGF_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
SwapChain::SwapChain()
	: m_Width(0)
	, m_Height(0)
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
	m_pSwapChain.Attach(_pSwapChain);
	m_Width = _width;
	m_Height = _height;

	// 백버퍼 텍스처를 꺼내 그리기 대상 뷰(RTV)를 만든다.
	// 실패하면 소유한 스왑체인까지 정리해 호출자(Window)가 일관된 상태로 복구할 수 있게 한다.
	SgfComPtr<ID3D11Texture2D> pBackBuffer;
	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(pBackBuffer.GetAddressOf()))))
	{
		Finalize();
		return false;
	}
	if (FAILED(_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr,
		m_pRenderTargetView.GetAddressOf())))
	{
		Finalize();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SwapChain::Finalize()
{
	m_pRenderTargetView.Reset();
	m_pSwapChain.Reset();
	m_Width = 0;
	m_Height = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 백버퍼를 화면에 표시 (더블 버퍼링의 "교체" 단계)
void SwapChain::Present(bool _bVsync)
{
	if (m_pSwapChain)
	{
		// 첫 인자(SyncInterval): 1이면 수직동기화 대기, 0이면 즉시 표시
		m_pSwapChain->Present(_bVsync ? 1 : 0, 0);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 백버퍼 전체를 그리기 영역으로 설정
void SwapChain::ApplyFullViewport(ID3D11DeviceContext* _pContext) const
{
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<_f32>(m_Width);
	viewport.Height = static_cast<_f32>(m_Height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	_pContext->RSSetViewports(1, &viewport);
}

NS_SGF_END
