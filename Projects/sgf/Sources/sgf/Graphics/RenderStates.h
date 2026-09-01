/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 1:00:00 AM
 * 수정일: 8/9/2026 5:20:00 PM (열거형을 GraphicsEnums.h로 통합, 접두어 표기)
 * =====================
 * 렌더 상태 저장소 (플라이웨이트 캐시)
 *
 * [상태 객체(State Object)란?]
 * DX11은 "반투명 설정", "깊이 테스트 설정" 같은 설정값 묶음을 미리 만들어두고
 * 그리기 전에 꺼내 끼우는 방식이다. 매번 만들면 느리므로 여기서 한 번만 만들고
 * 캐싱해서 재사용한다. (자주 쓰는 것은 미리, 나머지는 처음 요청될 때 지연 생성)
 *
 * [변경]
 * - 열거형 정의는 GraphicsEnums.h로 이동했다. (접두어 표기: bmAlpha, cmBack, ...)
 * - 상태 핸들(PipelineState.h)이 이 캐시를 공유한다.
 * - FrontFace/DepthMode(ReadOnly)/Anisotropic/Border 조합이 추가되었다.
 */

#pragma once

#include "sgf/Graphics/GraphicsEnums.h"

NS_SGF_BEGIN

using namespace jc;

// [하위 호환] 이름 별칭. 새 코드는 FilterMode/AddressMode를 쓰자.
using SamplerFilter = FilterMode;
using SamplerAddress = AddressMode;

class RenderStates
{
public:
	RenderStates();

	// 자주 쓰는 기본 상태들(bmNone/bmAlpha 블렌드, 깊이 ON/OFF, 솔리드+백컬링,
	// Linear+Clamp 샘플러)을 미리 만들어둔다.
	// @param _pDevice: 상태 객체 생성에 쓸 디바이스 (소유하지 않음)
	bool Initialize(ID3D11Device* _pDevice);

	// 보관 중인 모든 상태 객체를 해제한다.
	void Finalize();

	// 요청한 상태 객체를 반환한다. 없으면 그 자리에서 만들어 캐싱한다. (지연 생성)
	ID3D11BlendState* GetBlendState(BlendMode _mode);

	// 깊이 상태 (dmDisabled/dmReadWrite/dmReadOnly)
	ID3D11DepthStencilState* GetDepthState(DepthMode _mode);

	// 래스터라이저 상태 (컬링 + 채우기 + 앞면 판정 조합)
	ID3D11RasterizerState* GetRasterizerState(
		CullMode _cull,
		FillMode _fill = FillMode::fmSolid,
		FrontFace _frontFace = FrontFace::ffClockwise);

	// 샘플러 상태 (U/V 주소 모드 분리 + Anisotropic/Border 지원)
	ID3D11SamplerState* GetSamplerState(FilterMode _filter, AddressMode _addressU, AddressMode _addressV);

private:
	ID3D11Device* pDevice_;		// 생성용 디바이스 (소유하지 않음)

	// 캐시 저장소들. 인덱스 = enum 값.
	SgfComPtr<ID3D11BlendState> pBlendStates_[static_cast<_s32>(BlendMode::Max)];
	SgfComPtr<ID3D11DepthStencilState> pDepthStates_[static_cast<_s32>(DepthMode::Max)];
	SgfComPtr<ID3D11RasterizerState> pRasterizerStates_[static_cast<_s32>(FillMode::Max)][static_cast<_s32>(CullMode::Max)][static_cast<_s32>(FrontFace::Max)];
	SgfComPtr<ID3D11SamplerState> pSamplerStates_[static_cast<_s32>(FilterMode::Max)][static_cast<_s32>(AddressMode::Max)][static_cast<_s32>(AddressMode::Max)];
};

NS_SGF_END
