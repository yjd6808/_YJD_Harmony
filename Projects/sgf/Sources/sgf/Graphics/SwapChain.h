/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 1:00:00 AM
 * =====================
 * 스왑체인 래퍼 (v2: GraphicDevice에서 분리)
 *
 * [스왑체인이란?]
 *  화면에 보이는 버퍼(Front)와 그리는 중인 버퍼(Back)를 번갈아 교체(Present)하는
 *  장치다. 덕분에 그리는 중간 과정이 사용자에게 노출되지 않는다. (더블 버퍼링)
 *
 * [렌더타겟뷰(RTV)란?]
 *  "이 텍스처에 그려도 된다"라고 GPU에게 알려주는 해석서. 백버퍼도 텍스처이므로
 *  그리기 대상으로 쓰려면 RTV가 필요하다.
 */

#pragma once

#include "jc/Type.h"
#include "sgf/Namespace.h"

NS_SGF_BEGIN

class SwapChain
{
public:
	SwapChain();

	// 이미 생성된 스왑체인의 소유권을 넘겨받고, 백버퍼의 RTV를 만든다.
	// (디바이스와 스왑체인은 D3D11CreateDeviceAndSwapChain으로 함께 만들어져서
	//  생성 자체는 GraphicDevice가 담당하고, 이후 관리를 이 클래스가 맡는다)
	// @param _pSwapChain : 소유권을 넘겨받을 스왑체인 (참조카운트 그대로 받아 보관)
	bool Initialize(IDXGISwapChain* _pSwapChain, ID3D11Device* _pDevice, _s32 _width, _s32 _height);

	void Finalize();

	// 백버퍼를 화면에 표시한다.
	// @param _bVsync : true면 모니터 주사율에 맞춰 대기 (화면 윋어짐 방지)
	void Present(bool _bVsync);

	// 백버퍼 전체 크기의 뷰포트(그리기 영역)를 설정한다.
	void ApplyFullViewport(ID3D11DeviceContext* _pContext) const;

	ID3D11RenderTargetView* RTV() const { return pRenderTargetView_.Get(); }
	_s32 Width() const { return width_; }
	_s32 Height() const { return height_; }

private:
	SgfComPtr<IDXGISwapChain> pSwapChain_;					// 버퍼 교체 담당
	SgfComPtr<ID3D11RenderTargetView> pRenderTargetView_;	// 백버퍼를 그리기 대상으로 보는 뷰
	_s32 width_;		// 백버퍼 가로
	_s32 height_;		// 백버퍼 세로
};

NS_SGF_END
