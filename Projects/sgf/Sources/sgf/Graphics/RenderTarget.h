/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 1:30:00 PM
 * =====================
 * 렌더 타깃 - "화면 대신 텍스처에 그리기"
 *
 * [렌더 타깃이란?]
 *  지금까지는 항상 백버퍼(화면)에 그렸다. 렌더 타깃을 만들면 우리가 만든
 *  텍스처를 그리기 대상으로 쓸 수 있고, 다 그린 뒤에는 그 텍스처를
 *  일반 텍스처처럼 다시 샘플링할 수 있다.
 *  미니맵/거울(19번), 후처리(20번), 그림자 맵(21번)이 전부 이 기법 위에 있다.
 *
 * [핵심 아이디어: 같은 텍스처, 두 개의 뷰]
 *  - RTV(RenderTargetView)   : "여기에 그려라"   (출력용 해석)
 *  - SRV(ShaderResourceView) : "여기서 읽어라"   (입력용 해석)
 *  단, 동시에 둘 다는 불가능! 그리는 동안에는 읽을 수 없다.
 *  GraphicDevice::SetRenderTarget이 바인딩 해제를 자동으로 처리해준다.
 *
 * [깊이 전용 모드 (그림자 맵)]
 *  그림자 맵은 "빛의 시점에서 본 깊이"만 필요하므로 색 텍스처가 없다.
 *  깊이 텍스처를 TYPELESS 포맷으로 만들고
 *  DSV로는 D32_FLOAT(깊이 기록), SRV로는 R32_FLOAT(깊이 읽기)로 해석한다.
 */

#pragma once

#include "jc/Math.h"

NS_SGF_BEGIN

using namespace jc;

class GraphicDevice;

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	// 색 + 깊이를 모두 가진 일반 렌더 타깃을 만든다. (미니맵/후처리용)
	// @param _pDevice : 그래픽 디바이스
	// @param _width   : 텍스처 가로 크기 (백버퍼와 달라도 된다)
	// @param _height  : 텍스처 세로 크기
	// @return 성공 여부
	bool Create(GraphicDevice* _pDevice, int _width, int _height);

	// 깊이 전용 렌더 타깃을 만든다. (그림자 맵용 - 색 기록 없음)
	// 해상도가 클수록 그림자 경계가 깨끗해진다. (보통 1024/2048 정사각형)
	bool CreateDepthOnly(GraphicDevice* _pDevice, int _width, int _height);

	// 모든 리소스를 해제한다.
	void Destroy();

	// 타깃을 지운다. (색은 _clearColor로, 깊이는 1.0=가장 멀리로)
	// 이 타깃에 그리기 시작하기 전에 호출할 것.
	void Clear(GraphicDevice* _pDevice, const color& _clearColor);

	// 다 그린 결과를 일반 텍스처처럼 셰이더 입력으로 바인딩한다.
	// @param _slot : PS의 t레지스터 슬롯 번호
	void BindAsTexture(GraphicDevice* _pDevice, UINT _slot = 0);

	bool IsDepthOnly() const { return bDepthOnly_; }
	int Width() const { return width_; }
	int Height() const { return height_; }

	// GraphicDevice::SetRenderTarget에서 사용하는 내부 접근자
	ID3D11RenderTargetView* RTV() const { return pRTV_.Get(); }		// 깊이 전용이면 nullptr
	ID3D11DepthStencilView* DSV() const { return pDSV_.Get(); }
	ID3D11ShaderResourceView* SRV() const;								// 모드에 맞는 SRV 반환

private:
	SgfComPtr<ID3D11Texture2D> pColorTexture_;			// 색 기록용 텍스처
	SgfComPtr<ID3D11RenderTargetView> pRTV_;			// 색 텍스처를 "그리기 대상"으로 보는 뷰
	SgfComPtr<ID3D11ShaderResourceView> pColorSRV_;	// 색 텍스처를 "셰이더 입력"으로 보는 뷰

	SgfComPtr<ID3D11Texture2D> pDepthTexture_;			// 깊이 기록용 텍스처
	SgfComPtr<ID3D11DepthStencilView> pDSV_;			// 깊이 텍스처를 "깊이 버퍼"로 보는 뷰
	SgfComPtr<ID3D11ShaderResourceView> pDepthSRV_;	// 깊이 텍스처를 "셰이더 입력"으로 보는 뷰 (그림자 맵)

	bool bDepthOnly_;	// 깊이 전용 모드 여부
	int width_;		// 가로 크기
	int height_;		// 세로 크기
};

NS_SGF_END
