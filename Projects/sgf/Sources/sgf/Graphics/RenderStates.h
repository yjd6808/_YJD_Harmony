/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 1:00:00 AM
 * =====================
 * 렌더 상태 저장소 (v2: GraphicDevice에서 분리)
 *
 * [왜 분리했나? - D3D 개체 뭉침 해소]
 *  기존 GraphicDevice는 디바이스/스왑체인/깊이버퍼/상태객체 전부를 한 클래스에
 *  담고 있었다. 역할별로 쪼개면 각각이 작고 이해하기 쉽다:
 *   - RenderStates        : 블렌드/깊이/래스터라이저/샘플러 상태 객체 보관함 (이 파일)
 *   - SwapChain           : 백버퍼 교체(Present) 담당
 *   - DepthStencilSurface : 깊이/스텐실 버퍼 담당
 *   - GraphicDevice       : 위 세을 조립해 간편 API를 제공하는 파사드(facade)
 *
 * [상태 객체(State Object)란?]
 *  DX11은 "반투명 설정", "깊이 테스트 설정" 같은 설정값 묶음을 미리 만들어두고
 *  그리기 전에 꺼내 끼우는 방식이다. 매번 만들면 느리므로 여기서 한 번만 만들고
 *  캐싱해서 재사용한다. (자주 쓰는 것은 미리, 나머지는 처음 요청될 때 지연 생성)
 */

#pragma once

#include "sgf/Namespace.h"

NS_SGF_BEGIN

using namespace jc;

// 블렌드 모드: 픽셀 셰이더 결과와 이미 그려진 색을 섞는 공식 선택
enum class BlendMode
{
	Opaque,		// 섞지 않고 덮어쓰기
	Alpha,		// 최종색 = 새색*알파 + 기존색*(1-알파)  <- 일반 반투명
	Additive,	// 최종색 = 새색*알파 + 기존색            <- 빛/이펙트 (밝아짐)
	Multiply,	// 최종색 = 새색 * 기존색                 <- 그림자/어둠 (어두워짐)
	Max,
};

// 샘플러 필터: 텍스처 확대/축소 시 픽셀 사이 색을 정하는 방법
enum class SamplerFilter
{
	Point,		// 가장 가까운 픽셀 그대로 (계단현상, 도트게임 느낌)
	Linear,		// 주변 픽셀을 섞어 부드럽게 (기본값)
	Max,
};

// 샘플러 주소 모드: UV가 0~1 범위를 벗어났을 때 처리 방법
enum class SamplerAddress
{
	Wrap,		// 반복 (타일링)
	Clamp,		// 가장자리 색 고정 (기본값)
	Mirror,		// 거울처럼 뒤집으며 반복
	Max,
};

// 컸링 모드: 어느 방향을 보는 삼각형을 버릴지
enum class CullMode
{
	None,		// 양면 모두 그림
	Back,		// 뒷면 버림 (기본값)
	Front,		// 앞면 버림 (그림자 품질 개선 등 특수 기법용)
	Max,
};

class RenderStates
{
public:
	RenderStates();

	// 자주 쓰는 기본 상태들(Opaque/Alpha 블렌드, 깊이 ON/OFF, 솔리드+백컸링,
	// Linear+Clamp 샘플러)을 미리 만들어둔다.
	// @param _pDevice : 상태 객체 생성에 쓸 디바이스 (소유하지 않음)
	bool Initialize(ID3D11Device* _pDevice);

	// 보관 중인 모든 상태 객체를 해제한다.
	void Finalize();

	// 요청한 상태 객체를 반환한다. 없으면 그 자리에서 만들어 캐싱한다. (지연 생성)
	ID3D11BlendState* GetBlendState(BlendMode _mode);
	ID3D11DepthStencilState* GetDepthState(bool _bEnable);
	ID3D11RasterizerState* GetRasterizerState(bool _bWireframe, CullMode _cull);
	ID3D11SamplerState* GetSamplerState(SamplerFilter _filter, SamplerAddress _address);

private:
	ID3D11Device* pDevice_;		// 생성용 디바이스 (소유하지 않음)

	// 캐시 저장소들. 인덱스 = enum 값.
	SgfComPtr<ID3D11BlendState> pBlendStates_[static_cast<int>(BlendMode::Max)];
	SgfComPtr<ID3D11DepthStencilState> pDepthStates_[2];				// [0]=OFF, [1]=ON
	SgfComPtr<ID3D11RasterizerState> pRasterizerStates_[2][static_cast<int>(CullMode::Max)];	// [솔리드/와이어][컸링]
	SgfComPtr<ID3D11SamplerState> pSamplerStates_[static_cast<int>(SamplerFilter::Max)][static_cast<int>(SamplerAddress::Max)];
};

NS_SGF_END
