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
 *
 * [D3D11 매핑 주석 규칙]
 * 각 멤버 우측 주석에 대응하는 D3D11/DXGI 상수를 명시한다.
 * 엔진 전용(StaticLevel, ShaderStage 등)은 "D3D 직접 매핑 없음"으로 표기한다.
 *
 * [★ 표시]
 * 3D 게임에서 자주 사용되는 옵션에 ★를 붙였다. Wireframe처럼 디버깅에서
 * 빈번히 쓰이는 옵션도 ★로 표시하고, 왜 자주 쓰이는지 이유를 함께 적었다.
 */

#pragma once

#include "sgf/Namespace.h"

NS_SGF_BEGIN

// 리소스 사용 방식: GPU 리소스를 CPU/GPU가 어떻게 접근하는지 (D3D11_USAGE 래핑)
// - DEFAULT/DYNAMIC 구분을 bool이 아닌 enum으로 명시하여 IMMUTABLE/STAGING까지 표현한다.
// - Buffers(정점/인덱스), Texture, RenderTarget/DepthStencil 등 모든 GPU 리소스 생성에 공통 사용.
enum class ResourceUsage
{
	ruDefault,		// ★ D3D11_USAGE_DEFAULT (0) - GPU 전용, CPU 접근 불가. 3D 메시/렌더타깃/깊이버퍼의 90%가 이 모드. 가장 빠르고 기본값이라 자주 사용
	ruImmutable,	// D3D11_USAGE_IMMUTABLE (1) - 생성 시 초기 데이터 필수, 이후 변경 불가. 가장 빠름. Texture::CreateFromMemory 등 불변 리소스용
	ruDynamic,		// ★ D3D11_USAGE_DYNAMIC (2) - CPU Map(WRITE_DISCARD)으로 매 프레임 갱신 가능. 3D 상수버퍼(b0/b1)/동적 정점버퍼에 필수라 자주 사용
	ruStaging,		// D3D11_USAGE_STAGING (3) - CPU 읽기/쓰기 가능, GPU는 복사 전용. Readback/Capture 등 CPU Readback용 (현재 엔진 미사용, 예약)
	Max,
};

// 블렌드 모드: 픽셀 셰이더 결과와 이미 그려진 색을 섞는 공식
// D3D11 매핑: FillBlendDesc() / RenderStates::GetBlendState() 내부 D3D11_BLEND_DESC 생성
enum class BlendMode
{
	bmNone,		// ★ D3D11_BLEND_DESC BlendEnable=FALSE, Src=ONE/Dest=ZERO - 3D 불투명 오브젝트 기본. 가장 많이 그리는 모드라 자주 사용
	bmAlpha,	// ★ D3D11_BLEND_SRC_ALPHA / INV_SRC_ALPHA, BlendOp=ADD - 3D 반투명(유리/연기/UI)에 필수. 투명 물체 대부분이 이 모드라 자주 사용
	bmAdd,		// ★ D3D11_BLEND_SRC_ALPHA / ONE, BlendOp=ADD - 3D 가산 합성. 겹칠수록 밝아져 파티클/불/빛 이펙트에 필수라 자주 사용
	bmMultiply,	// D3D11_BLEND_DEST_COLOR / ZERO, BlendOp=ADD - 3D 곱셈 합성. 겹칠수록 어두워져 그림자/오염 표현에 사용 (Add/Alpha보다 빈도 낮음)
	Max,
};

// 컬링 모드: 어느 방향을 보는 삼각형을 버릴지
// D3D11 매핑: D3D11_RASTERIZER_DESC::CullMode → ToD3D11()
enum class CullMode
{
	cmNone,			// ★ D3D11_CULL_NONE - 양면 모두 그림. 3D 나뭇잎/천/양면 평면에 필요하고 Wireframe 디버깅 시에도 자주 사용
	cmFront,		// D3D11_CULL_FRONT - 앞면 버림 (그림자 품질 개선 등 특수 기법용, 일반 3D에서는 드묾)
	cmBack,			// ★ D3D11_CULL_BACK - 뒷면 버림. 3D 닫힌 물체는 뒷면이 안 보이므로 절반을 버려 성능 2배. 3D 기본값이라 가장 자주 사용
	Max,
};

// 채우기 모드: 삼각형 내부를 채울지 선만 그릴지
// D3D11 매핑: D3D11_RASTERIZER_DESC::FillMode → ToD3D11()
enum class FillMode
{
	fmSolid,		// ★ D3D11_FILL_SOLID - 면 채우기. 3D 최종 렌더링 기본. 99%가 이 모드라 자주 사용
	fmWireframe,	// ★ D3D11_FILL_WIREFRAME - 선만 그리기. 3D 메쉬 구조/토폴로지/깊이 디버깅에 필수라 자주 사용
	Max,
};

// 앞면 판정 기준: 정점이 어느 방향으로 감길 때 앞면으로 볼지
// D3D11 매핑: D3D11_RASTERIZER_DESC::FrontCounterClockwise (BOOL)
enum class FrontFace
{
	ffClockwise,			// ★ D3D11 FrontCounterClockwise=FALSE - 시계 방향 = 앞면. D3D 기본값. 3D 에셋 대부분이 이 규칙이라 자주 사용
	ffCounterClockwise,		// D3D11 FrontCounterClockwise=TRUE - 반시계 방향 = 앞면 (OpenGL 스타일, ToD3D11 → TRUE, 일반 3D에서는 드묾)
	Max,
};

// 깊이 모드: 깊이 테스트/쓰기 조합
// D3D11 매핑: D3D11_DEPTH_STENCIL_DESC::{DepthEnable, DepthWriteMask, DepthFunc=LESS_EQUAL} (FillDepthStencilDesc)
enum class DepthMode
{
	dmDisabled,		// D3D11 DepthEnable=FALSE, DepthWriteMask=ZERO - 3D에서 스카이박스/2D UI처럼 깊이 무시하고 순서대로 그릴 때 사용
	dmReadWrite,	// ★ D3D11 DepthEnable=TRUE, DepthWriteMask=ALL, DepthFunc=LESS_EQUAL - 3D 불투명 기본. 테스트+기록 모두 켜야 올바른 occlusion이 되므로 가장 자주 사용
	dmReadOnly,		// ★ D3D11 DepthEnable=TRUE, DepthWriteMask=ZERO - 3D 반투명(유리/파티클). 테스트는 하되 기록은 안 해야 뒤 물체가 가려지지 않으므로 자주 사용
	Max,
};

// 샘플러 필터: 텍스처 확대/축소 시 픽셀 사이 색을 정하는 방법
// D3D11 매핑: D3D11_SAMPLER_DESC::Filter → ToD3D11()
enum class FilterMode
{
	fmPoint,		// D3D11_FILTER_MIN_MAG_MIP_POINT - 가장 가까운 픽셀 그대로 (도트게임 느낌, 3D 픽셀아트 외에는 드묾)
	fmLinear,		// ★ D3D11_FILTER_MIN_MAG_MIP_LINEAR - 주변 4픽셀을 섞어 부드럽게. 3D 일반 텍스처 기본. 90%가 이 모드라 자주 사용
	fmAnisotropic,	// ★ D3D11_FILTER_ANISOTROPIC, MaxAnisotropy=16 - 3D 비스듬한 바닥/벽에서 멀리까지 선명도 유지. 3D 품질 옵션으로 자주 사용
	Max,
};

// 샘플러 주소 모드: UV가 0~1 범위를 벗어났을 때 처리 방법
// D3D11 매핑: D3D11_SAMPLER_DESC::{AddressU/V/W} → ToD3D11()
enum class AddressMode
{
	amWrap,			// ★ D3D11_TEXTURE_ADDRESS_WRAP - 반복 타일링. 3D 바닥/벽 타일에 필수라 자주 사용
	amMirror,		// D3D11_TEXTURE_ADDRESS_MIRROR - 거울처럼 뒤집으며 반복 (특수 타일링, 빈도 낮음)
	amClamp,		// ★ D3D11_TEXTURE_ADDRESS_CLAMP - 가장자리 색 고정. 3D 일반 오브젝트/스카이박스 기본. 가장 많이 쓰이므로 자주 사용
	amBorder,		// ★ D3D11_TEXTURE_ADDRESS_BORDER - 테두리 색(BorderColor). 3D 그림자 맵/프로젝션에서 범위 밖을 검정으로 처리할 때 필수라 자주 사용
	Max,
};

// 셰이더 스테이지: 리소스를 어느 셰이더 단계에 묶을지
// D3D 매핑: D3D 직접 enum 없음 — GraphicContext::SetConstantBuffer/SetTexture/SetSampler에서 VSSet*/PSSet* 분기용
enum class ShaderStage
{
	ssVertex,		// ★ D3D 직접 매핑 없음 - VSSetConstantBuffers/Resources/Samplers. 3D 월드/뷰/투영 행렬(b0) 전달에 필수라 항상 사용
	ssPixel,		// ★ D3D 직접 매핑 없음 - PSSetConstantBuffers/Resources/Samplers. 3D 텍스처/샘플러/조명 상수 전달에 필수라 항상 사용
	Max,
};

// 프리미티브 토폴로지: 정점들을 어떤 도형으로 이을지
// D3D11 매핑: IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_*) → ToD3D11()
enum class PrimitiveTopology
{
	ptPointList,		// D3D11_PRIMITIVE_TOPOLOGY_POINTLIST - 점 목록 (파티클 점, 디버깅용, 일반 3D 메쉬에서는 드묾)
	ptLineList,			// D3D11_PRIMITIVE_TOPOLOGY_LINELIST - 독립된 선분 목록 (3D 와이어/디버그 라인에 사용)
	ptLineStrip,		// D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP - 연결된 선 (3D 궤적/경로 디버깅에 사용)
	ptTriangleList,		// ★ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST - 독립된 삼각형 목록. 3D 메쉬의 99%가 이 방식이라 가장 자주 사용
	ptTriangleStrip,	// D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP - 연결된 삼각형 띠 (지형/리본 최적화에 가끔 사용)
	Max,
};

// 메시 렌더 모드 — B(다이나믹) 기본, A(스태틱)는 GameObject::SetMeshStaticLevel(slStatic) 명시 호출로 전환
// D3D 매핑: D3D 직접 매핑 없음 — 엔진 전용. slStatic = 현재 월드 행렬 고정 (GPU 변환이므로 정점은 무변형 — 검사는 GetMeshStaticLevel() 하나로)
enum class StaticLevel
{
	slDynamic,	// ★ D3D 직접 매핑 없음 - B, 매 프레임 현재 월드 행렬 사용. 3D 움직이는 오브젝트 기본이라 가장 자주 사용
	slStatic,	// ★ D3D 직접 매핑 없음 - A, 고정된 월드 행렬 사용. 3D 정적 배경/건물 배칭에 필수라 자주 사용 (위치/회전/스케일 고정)
	Max,
};

// 픽셀 포맷: 텍스처/백버퍼/깊이버퍼의 픽셀 메모리 형식
// D3D 매핑: DXGI_FORMAT_* → ToD3D11()
enum class PixelFormat
{
	pfRgba8,			// ★ DXGI_FORMAT_R8G8B8A8_UNORM - 8비트 x 4채널. 3D 일반 텍스처/백버퍼 기본. 90%가 이 포맷이라 자주 사용
	pfBgra8,			// DXGI_FORMAT_B8G8R8A8_UNORM - 8비트 x 4채널 BGRA 순서 (WIC 디코딩 결과 등, 내부 변환용)
	pfRgba32Float,		// DXGI_FORMAT_R32G32B32A32_FLOAT - 32비트 float x 4채널 (HDR/중간 계산용, 특수 효과에만 사용)
	pfR32Float,			// DXGI_FORMAT_R32_FLOAT - 32비트 float x 1채널 (그림자 맵 단일 채널 등에 사용)
	pfDepth24Stencil8,	// ★ DXGI_FORMAT_D24_UNORM_S8_UINT - 깊이 24비트 + 스텐실 8비트. 3D 깊이버퍼 기본. 3D라면 항상 쓰므로 자주 사용
	pfDepth32,			// DXGI_FORMAT_D32_FLOAT - 깊이 32비트 float (고정밀 깊이 필요 시, 일반 3D보다 빈도 낮음)
	Max,
};

////////////////////////////////////////////////////////////////////////////////////////////
// D3D11 값 변환 헬퍼 (PCH가 d3d11.h를 포함하므로 여기서 바로 사용 가능)
// 매핑은 이 파일 한 곳에만 둔다. 사용처에서 D3D11 상수를 직접 쓰지 말 것.
inline D3D11_USAGE ToD3D11(ResourceUsage _usage)
{
	switch (_usage)
	{
	case ResourceUsage::ruImmutable:	return D3D11_USAGE_IMMUTABLE;
	case ResourceUsage::ruDynamic:		return D3D11_USAGE_DYNAMIC;
	case ResourceUsage::ruStaging:		return D3D11_USAGE_STAGING;
	default:							return D3D11_USAGE_DEFAULT;
	}
}

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
