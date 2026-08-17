/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 1:00:00 AM
 * =====================
 * 깊이/스텐실 버퍼 (GraphicDevice에서 분리)
 *
 * [깊이 버퍼(Depth Buffer)란?]
 * 픽셀마다 "카메라로부터의 거리"를 기록하는 별도 텍스처.
 * 새 픽셀이 기존보다 멀면 버리고, 가까우면 덮어쓴다.
 * 덕분에 그리는 순서와 상관없이 가까운 물체가 먼 물체를 가린다. (3D 필수)
 *
 * [스텐실(Stencil)이란?]
 * 픽셀마다 8비트 "도장 값"을 기록해 마스킹(특정 영역만 그리기/제외하기)에
 * 쓰는 기능. D24_UNORM_S8_UINT 포맷 = 깊이 24비트 + 스텐실 8비트.
 */

#pragma once

#include "jc/Type.h"
#include "sgf/Namespace.h"

NS_SGF_BEGIN

class DepthStencilSurface
{
public:
	// 깊이/스텐실 텍스처와 DSV(깊이스텐실뷰)를 만든다.
	bool Initialize(ID3D11Device* _pDevice, _s32 _width, _s32 _height);

	void Finalize();

	// 깊이를 1.0(가장 멀리), 스텐실을 0으로 초기화한다. 매 프레임 시작 시 호출.
	void Clear(ID3D11DeviceContext* _pContext);

	ID3D11DepthStencilView* DSV() const { return pDepthStencilView_.Get(); }

private:
	SgfComPtr<ID3D11DepthStencilView> pDepthStencilView_;	// 깊이 버퍼를 그리기 대상으로 보는 뷰
};

NS_SGF_END
