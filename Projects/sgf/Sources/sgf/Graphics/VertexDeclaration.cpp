/*
 * 작성자: 윤정도
 * 생성일: 8/23/2026
 * =====================
 * 정점 선언 구현부
 */

#include "Core.h"
#include "sgf/Graphics/VertexDeclaration.h"
#include "jc/Hasher.h"

#include <cstring>

NS_SGF_BEGIN

using namespace jc;

////////////////////////////////////////////////////////////////////////////////////////
const char* ToSemanticName(VertexSemantic _semantic)
{
	static constexpr const char* NAMES[static_cast<_s32>(VertexSemantic::Max)] =
	{
		"POSITION", "NORMAL", "TANGENT", "COLOR", "TEXCOORD", "BLENDINDICES", "BLENDWEIGHT",
	};
	return NAMES[static_cast<_s32>(_semantic)];
}

////////////////////////////////////////////////////////////////////////////////////////
_u32 VertexElementSize(VertexElementFormat _format)
{
	switch (_format)
	{
	case VertexElementFormat::vefFloat1:	return 4;
	case VertexElementFormat::vefFloat2:	return 8;
	case VertexElementFormat::vefFloat3:	return 12;
	case VertexElementFormat::vefFloat4:	return 16;
	case VertexElementFormat::vefColor32:	return 4;
	case VertexElementFormat::vefUByte4:	return 4;
	default: jc_assert_msg(false, "알 수 없는 VertexElementFormat"); return 0;
	}
}

namespace
{
	////////////////////////////////////////////////////////////////////////////////////
	DXGI_FORMAT ToDxgi(VertexElementFormat _format)
	{
		switch (_format)
		{
		case VertexElementFormat::vefFloat1:	return DXGI_FORMAT_R32_FLOAT;
		case VertexElementFormat::vefFloat2:	return DXGI_FORMAT_R32G32_FLOAT;
		case VertexElementFormat::vefFloat3:	return DXGI_FORMAT_R32G32B32_FLOAT;
		case VertexElementFormat::vefFloat4:	return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case VertexElementFormat::vefColor32:	return DXGI_FORMAT_R8G8B8A8_UNORM;
		case VertexElementFormat::vefUByte4:	return DXGI_FORMAT_R8G8B8A8_UINT;
		default: return DXGI_FORMAT_UNKNOWN;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// 인터닝 — 같은 내용이면 같은 인스턴스. 저장소 수명 = 프로그램 수명.
// [주의] 초기화 경로 단일 스레드 가정. 멀티스레드 로딩 도입 시 락 추가할 것.
const VertexDeclaration* VertexDeclaration::GetOrCreate(const VertexElement* _pElements, _s32 _count)
{
	jc_assert_msg(_pElements != nullptr && _count > 0 && _count <= MAX_VERTEX_ELEMENTS,
		"VertexDeclaration 요소 개수가 잘못되었습니다.");

	// 1. 오프셋 해석(자동 누적) + 슬롯별 stride + 해시 계산 (HashBuilder<Fnv1a64>로 패딩 배제 누적)
	_u16 resolvedOffsets[MAX_VERTEX_ELEMENTS] = {};
	_u32 strides[MAX_VERTEX_STREAMS] = {};
	jc::HashBuilder<jc::HashAlgorithm::Fnv1a64> builder;
	for (_s32 i = 0; i < _count; ++i)
	{
		const VertexElement& element = _pElements[i];
		jc_assert_msg(element.slot_ < MAX_VERTEX_STREAMS, "슬롯 번호가 범위를 벗어났습니다.");

		const _u16 offset = (element.offset_ == VERTEX_OFFSET_APPEND)
			? static_cast<_u16>(strides[element.slot_])
			: element.offset_;
		resolvedOffsets[i] = offset;

		const _u32 end = offset + VertexElementSize(element.format_);
		if (end > strides[element.slot_]) { strides[element.slot_] = end; }

		builder.AppendIntegral(static_cast<_u64>(element.semantic_));
		builder.AppendIntegral(element.semanticIndex_);
		builder.AppendIntegral(static_cast<_u64>(element.format_));
		builder.AppendIntegral(element.slot_);
		builder.AppendIntegral(offset);
	}
	const _u64 hash = builder.Digest();

	// 2. 기존 항목 검색 — 해시 선비교 후 내용 확인 (해시 충돌 방어)
	struct Registry
	{
		jc::Vector<VertexDeclaration*> decls_;
		~Registry()
		{
			for (_s32 i = 0; i < decls_.Size(); ++i) { delete decls_[i]; }
			decls_.Clear();
		}
	};
	static Registry s_registry;

	for (_s32 i = 0; i < s_registry.decls_.Size(); ++i)
	{
		VertexDeclaration* pExisting = s_registry.decls_[i];
		if (pExisting->hash_ != hash || pExisting->elements_.Size() != _count) { continue; }

		bool same = true;
		for (_s32 e = 0; e < _count; ++e)
		{
			const VertexElement& lhs = pExisting->elements_[e];
			const VertexElement& rhs = _pElements[e];
			if (lhs.semantic_ != rhs.semantic_ || lhs.semanticIndex_ != rhs.semanticIndex_ ||
				lhs.format_ != rhs.format_ || lhs.slot_ != rhs.slot_ ||
				pExisting->resolvedOffsets_[e] != resolvedOffsets[e])
			{
				same = false;
				break;
			}
		}
		if (same) { return pExisting; }
	}

	// 3. 신규 등록
	VertexDeclaration* pDecl = dbg_new VertexDeclaration();
	pDecl->elements_.Reserve(_count);
	for (_s32 i = 0; i < _count; ++i)
	{
		pDecl->elements_.PushBack(_pElements[i]);
		pDecl->resolvedOffsets_[i] = resolvedOffsets[i];
	}
	for (_s32 s = 0; s < MAX_VERTEX_STREAMS; ++s) { pDecl->strides_[s] = strides[s]; }
	pDecl->hash_ = hash;
	s_registry.decls_.PushBack(pDecl);
	return pDecl;
}

////////////////////////////////////////////////////////////////////////////////////////
bool VertexDeclaration::Contains(const char* _szSemanticName, _u32 _semanticIndex) const
{
	for (_s32 i = 0; i < elements_.Size(); ++i)
	{
		const VertexElement& element = elements_[i];
		if (element.semanticIndex_ == _semanticIndex &&
			_stricmp(ToSemanticName(element.semantic_), _szSemanticName) == 0)
		{
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////
_s32 BuildD3DElementDescs(const VertexDeclaration& _decl, D3D11_INPUT_ELEMENT_DESC* _pOutDescs, _s32 _capacity)
{
	const _s32 count = _decl.ElementCount();
	if (_pOutDescs == nullptr || _capacity < count) { return 0; }

	for (_s32 i = 0; i < count; ++i)
	{
		const VertexElement& element = _decl.Element(i);
		D3D11_INPUT_ELEMENT_DESC& desc = _pOutDescs[i];
		desc.SemanticName = ToSemanticName(element.semantic_);
		desc.SemanticIndex = element.semanticIndex_;
		desc.Format = ToDxgi(element.format_);
		desc.InputSlot = element.slot_;
		desc.AlignedByteOffset = _decl.ResolvedOffset(i);
		desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = 0;
	}
	return count;
}

NS_SGF_END
