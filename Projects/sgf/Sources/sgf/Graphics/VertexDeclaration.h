/*
 * 작성자: 윤정도
 * 생성일: 8/23/2026
 * =====================
 * 정점 선언 (VertexDeclaration) — 정점 포맷의 단일 진실
 *
 * [왜 필요한가?]
 * 기존에는 정점 포맷이 3곳에 중복 정의되어 있었다: Vertex.h의 D3D11_INPUT_ELEMENT_DESC 배열,
 * VertexBuffer의 stride, GraphicDevice의 캐시 슬롯. 이제 이 모든 정보는 VertexDeclaration 하나에 모인다.
 * COLOR가 4바이트 R8G8B8A8_UNORM인 정보는 셰이더 리플렉션으로 알 수 없으므로 선언이 포맷의 진실이어야 한다.
 *
 * [인터닝]
 * 같은 내용의 선언은 프로그램 전체에서 같은 인스턴스를 공유한다 (GetOrCreate).
 * 덕분에 포인터 비교만으로 같은 포맷인지 판단할 수 있고, 지역 배열로 선언을 만들어도 안전하다.
 *
 * [시맨틱]
 * HLSL의 시맨틱 이름(POSITION, COLOR, TEXCOORD 등)과 1:1 대응한다.
 * VS 입력 시그니처 검증(Contains)에 사용된다.
 */

#pragma once

#include "jc/Type.h"
#include "jc/Container/Vector.h"
#include "sgf/Namespace.h"
#include <d3d11.h>

NS_SGF_BEGIN

using namespace jc;

// 정점 시맨틱 — HLSL 시맨틱 이름과 1:1 대응
enum class VertexSemantic : _u8
{
	vsPosition = 0,		// "POSITION"
	vsNormal,			// "NORMAL"
	vsTangent,			// "TANGENT"
	vsColor,			// "COLOR"
	vsTexCoord,			// "TEXCOORD"
	vsBlendIndices,		// "BLENDINDICES"
	vsBlendWeight,		// "BLENDWEIGHT"
	Max,
};

// 정점 요소 형식 — DXGI 포맷과 1:1 대응 (Mesh.h의 VertexFormat은 2D/3D 판별용이므로 이름 분리)
enum class VertexElementFormat : _u8
{
	vefFloat1 = 0,		// DXGI_FORMAT_R32_FLOAT          (4B)
	vefFloat2,			// DXGI_FORMAT_R32G32_FLOAT       (8B)
	vefFloat3,			// DXGI_FORMAT_R32G32B32_FLOAT    (12B)
	vefFloat4,			// DXGI_FORMAT_R32G32B32A32_FLOAT (16B)
	vefColor32,			// DXGI_FORMAT_R8G8B8A8_UNORM     (4B) — jc::color
	vefUByte4,			// DXGI_FORMAT_R8G8B8A8_UINT      (4B)
	Max,
};

// 오프셋 자동 누적 표시 — 이 값을 쓰면 이전 요소 끝부터 자동 배치된다
constexpr _u16 VERTEX_OFFSET_APPEND = 0xFFFF;
// 선언당 최대 요소 수 (D3D11 한계 32, 여유 있게 16)
constexpr _s32 MAX_VERTEX_ELEMENTS = 16;
// 입력 슬롯 수 (향후 스트림 분리용)
constexpr _s32 MAX_VERTEX_STREAMS = 4;

// 정점 요소 하나 — { 시맨틱, 시맨틱번호, 형식, 슬롯, 오프셋 }
struct VertexElement
{
	VertexSemantic semantic_ = VertexSemantic::vsPosition;
	_u8 semanticIndex_ = 0;							// TEXCOORD0/1... 번호
	VertexElementFormat format_ = VertexElementFormat::vefFloat3;
	_u8 slot_ = 0;									// 지금은 0 고정 (확장 예약)
	_u16 offset_ = VERTEX_OFFSET_APPEND;			// 기본: 자동 누적
};

// 정점 선언 — 정점 구조체 하나의 메모리 배치를 기술한다
class VertexDeclaration
{
public:
	// 동일 내용이면 항상 같은 인스턴스 반환 (인터닝). 반환 포인터는 프로그램 종료까지 유효.
	static const VertexDeclaration* GetOrCreate(const VertexElement* _pElements, _s32 _count);

	template <_s32 N>
	static const VertexDeclaration* GetOrCreate(const VertexElement (&_elements)[N])
	{
		return GetOrCreate(_elements, N);
	}

	_u64 Hash() const { return hash_; }
	_s32 ElementCount() const { return elements_.Size(); }
	const VertexElement& Element(_s32 _index) const { return elements_[_index]; }
	_u16 ResolvedOffset(_s32 _index) const { return resolvedOffsets_[_index]; }
	_u32 Stride(_u8 _slot = 0) const { return strides_[_slot]; }

	// VS 입력 시그니처 사전 검증용 — 해당 시맨틱이 이 선언에 있는가?
	bool Contains(const char* _szSemanticName, _u32 _semanticIndex) const;

private:
	VertexDeclaration() = default;

private:
	jc::Vector<VertexElement> elements_;
	_u16 resolvedOffsets_[MAX_VERTEX_ELEMENTS] = {};
	_u32 strides_[MAX_VERTEX_STREAMS] = {};
	_u64 hash_ = 0;
};

const char* ToSemanticName(VertexSemantic _semantic);
_u32 VertexElementSize(VertexElementFormat _format);

// 선언 → D3D11_INPUT_ELEMENT_DESC 배열 채우기. 채운 개수 반환. (GraphicDevice 전용)
_s32 BuildD3DElementDescs(const VertexDeclaration& _decl, D3D11_INPUT_ELEMENT_DESC* _pOutDescs, _s32 _capacity);

NS_SGF_END
