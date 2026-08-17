/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:10:00 PM
 * =====================
 * 그래픽 열거형 모음 (접두어 표기 통일)
 *
 * [표기 규칙]
 * 각 열거형 멤버는 열거형 이름의 머리글자 소문자 접두어를 붙인다.
 * BlendMode -> bm, CullMode -> cm, FillMode/FilterMode -> fm, ...
 * (ResourceType::rtTexture와 같은 표기를 전체 열거형에 통일 적용. D-16b)
 * 종단에는 항상 Max를 둔다. (배열 크기/순회용. 컴벤션 5.6)
 *
 * [왜 enum class인가?]
 * 일반 enum과 달리 암시적 int 변환이 없고 이름 충돌이 없다.
 * D3D11 값으로의 변환은 아래 ToD3D11 인라인 함수로만 허용한다.
 */

#pragma once

#include "sgf/Namespace.h"

NS_SGF_BEGIN

// 블렌드 모드: 픽셀 셰이더 결과와 이미 그려진 색을 섞는 공식
enum class BlendMode
{
	bmNone,			// 섞지 않고 덮어쓰기 (불투명)
	bmAlpha,		// 새색*알파 + 기존색*(1-알파) <- 일반 반투명
	bmAdd,		// 새색*알파 + 기존색 <- 빛/이펙트 (밝아짐)
	bmMultiply,		// 새색 * 기존색 <- 그림자/어둠 (어두워짐)
	Max,
};

// 컬링 모드: 어느 방향을 보는 삼각형을 버릴지
enum class CullMode
{
	cmNone,			// 양면 모두 그림
	cmFront,		// 앞면 버림 (그림자 품질 개선 등 특수 기법용)
	cmBack,			// 뒷면 버림 (기본값)
	Max,
};

// 채우기 모드: 삼각형 내부를 채울지 선만 그릴지
enum class FillMode
{
	fmSolid,		// 면 채우기 (기본값)
	fmWireframe,	// 선만 그리기 (디버깅/토폴로지 확인용)
	Max,
};

// 앞면 판정 기준: 정점이 어느 방향으로 감길 때 앞면으로 볼지
enum class FrontFace
{
	ffClockwise,			// 시계 방향 = 앞면 (D3D 기본값)
	ffCounterClockwise,		// 반시계 방향 = 앞면 (OpenGL 스타일)
	Max,
};

// 깊이 모드: 깊이 테스트/쓰기 조합
enum class DepthMode
{
	dmDisabled,		// 깊이 테스트 OFF (2D 적층 그리기)
	dmReadWrite,	// 테스트 ON + 기록 ON (일반 3D. 기본값)
	dmReadOnly,		// 테스트 ON + 기록 OFF (반투명 3D 물체)
	Max,
};

// 샘플러 필터: 텍스처 확대/축소 시 픽셀 사이 색을 정하는 방법
enum class FilterMode
{
	fmPoint,		// 가장 가까운 픽셀 그대로 (도트게임 느낌)
	fmLinear,		// 주변 픽셀을 섞어 부드럽게 (기본값)
	fmAnisotropic,	// 비등방성 필터 (비스듬한 바닥 품질 향상)
	Max,
};

// 샘플러 주소 모드: UV가 0~1 범위를 벗어났을 때 처리 방법
enum class AddressMode
{
	amWrap,			// 반복 (타일링)
	amMirror,		// 거울처럼 뒤집으며 반복
	amClamp,		// 가장자리 색 고정 (기본값)
	amBorder,		// 지정한 테두리 색 (그림자 맵 범위 밖 처리 등)
	Max,
};

// 셰이더 스테이지: 리소스를 어느 셰이더 단계에 묶을지
enum class ShaderStage
{
	ssVertex,		// 버텍스 셰이더 (VS)
	ssPixel,		// 픽셀 셰이더 (PS)
	Max,
};

// 프리미티브 토폴로지: 정점들을 어떤 도형으로 이을지
enum class PrimitiveTopology
{
	ptPointList,		// 점 목록
	ptLineList,			// 독립된 선분 목록 (정점 2개당 선 1개)
	ptLineStrip,		// 연결된 선 (앞 선의 끝 = 다음 선의 시작)
	ptTriangleList,		// 독립된 삼각형 목록 (기본값)
	ptTriangleStrip,	// 연결된 삼각형 띄
	Max,
};

// 픽셀 포맷: 텍스처/백버퍼/깊이버퍼의 픽셀 메모리 형식
enum class PixelFormat
{
	pfRgba8,			// 8비트 x 4채널 (일반 텍스처/백버퍼 기본값)
	pfBgra8,			// 8비트 x 4채널 BGRA 순서 (WIC 디코딩 결과 등)
	pfRgba32Float,		// 32비트 float x 4채널 (HDR/중간 계산용)
	pfR32Float,			// 32비트 float x 1채널 (그림자 맵 등)
	pfDepth24Stencil8,	// 깊이 24비트 + 스텐실 8비트 (깊이버퍼 기본값)
	pfDepth32,			// 깊이 32비트 float
	Max,
};

// ============================================================================
// D3D11 값 변환 헬퍼 (PCH가 d3d11.h를 포함하므로 여기서 바로 사용 가능)
// 매핑은 이 파일 한 곳에만 둔다. 사용처에서 D3D11 상수를 직접 쓰지 말 것.
// ============================================================================

inline D3D11_CULL_MODE ToD3D11(CullMode _mode)
{
	switch (_mode)
	{
	case CullMode::cmNone:	return D3D11_CULL_NONE;
	case CullMode::cmFront:	return D3D11_CULL_FRONT;
	default:				return D3D11_CULL_BACK;
	}
}

inline D3D11_FILL_MODE ToD3D11(FillMode _mode)
{
	return _mode == FillMode::fmWireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
}

inline D3D11_TEXTURE_ADDRESS_MODE ToD3D11(AddressMode _mode)
{
	switch (_mode)
	{
	case AddressMode::amWrap:	return D3D11_TEXTURE_ADDRESS_WRAP;
	case AddressMode::amMirror:	return D3D11_TEXTURE_ADDRESS_MIRROR;
	case AddressMode::amBorder:	return D3D11_TEXTURE_ADDRESS_BORDER;
	default:					return D3D11_TEXTURE_ADDRESS_CLAMP;
	}
}

inline D3D11_FILTER ToD3D11(FilterMode _mode)
{
	switch (_mode)
	{
	case FilterMode::fmPoint:		return D3D11_FILTER_MIN_MAG_MIP_POINT;
	case FilterMode::fmAnisotropic:	return D3D11_FILTER_ANISOTROPIC;
	default:						return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	}
}

inline D3D11_PRIMITIVE_TOPOLOGY ToD3D11(PrimitiveTopology _topology)
{
	switch (_topology)
	{
	case PrimitiveTopology::ptPointList:		return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	case PrimitiveTopology::ptLineList:			return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	case PrimitiveTopology::ptLineStrip:		return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case PrimitiveTopology::ptTriangleStrip:	return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	default:									return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
}

inline DXGI_FORMAT ToD3D11(PixelFormat _format)
{
	switch (_format)
	{
	case PixelFormat::pfBgra8:				return DXGI_FORMAT_B8G8R8A8_UNORM;
	case PixelFormat::pfRgba32Float:		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case PixelFormat::pfR32Float:			return DXGI_FORMAT_R32_FLOAT;
	case PixelFormat::pfDepth24Stencil8:	return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case PixelFormat::pfDepth32:			return DXGI_FORMAT_D32_FLOAT;
	default:								return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
}

NS_SGF_END
