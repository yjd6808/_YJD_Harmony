/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:50:00 PM
 * =====================
 * 머티리얼 (셰이더 + 파이프라인 상태 + 텍스처 조합)
 *
 * [머티리얼이란?]
 * "어떻게 그릴지"의 묶음. 메시(무엇을)와 분리되어 있어서
 * 같은 큐브를 벽돌 재질로도, 금속 재질로도 그릴 수 있다.
 *
 * [참조 방식] (D-13)
 * 셰이더/텍스처는 포인터가 아닌 ResourceMgr 키(_u64)로 보관한다.
 * 리소스가 제거되면 Bind 시점에 안전하게 감지된다. (댐링 포인터 방지)
 *
 * [상수버퍼 규약] b2 = 머티리얼 상수 (baseColor)
 */

#pragma once

#include "sgf/Graphics/IResource.h"
#include "sgf/Graphics/GraphicsEnums.h"
#include "sgf/Graphics/Buffers.h"
#include "sgf/Graphics/GraphicContext.h"

NS_SGF_BEGIN

using namespace jc;

class GraphicDevice;

// b2 슬롯에 올라가는 머티리얼 상수 (16바이트 배수)
struct MaterialConstants
{
	_f32 baseColor_[4] = { 1.0f, 1.0f, 1.0f, 1.0f };	// 기본색/틴트
};

struct MaterialDesc
{
	_u64 vertexShaderKey_ = INVALID_RESOURCE_KEY;
	_u64 pixelShaderKey_ = INVALID_RESOURCE_KEY;
	_u64 textureKeys_[GraphicContext::MAX_TEXTURE_SLOTS] = {};

	color baseColor_ = color::WHITE;

	BlendMode blendMode_ = BlendMode::bmNone;
	DepthMode depthMode_ = DepthMode::dmReadWrite;
	CullMode cullMode_ = CullMode::cmBack;
	FillMode fillMode_ = FillMode::fmSolid;
	FrontFace frontFace_ = FrontFace::ffClockwise;
	FilterMode filter_ = FilterMode::fmLinear;
	AddressMode addrU_ = AddressMode::amClamp;
	AddressMode addrV_ = AddressMode::amClamp;

	static MaterialDesc Default2D();
	static MaterialDesc Default3D();

	_u64 Hash() const;
	bool operator==(const MaterialDesc& _other) const;
	bool operator!=(const MaterialDesc& _other) const { return !(*this == _other); }
};

class Material : public ResourceBase
{
	SGF_RESOURCE_TYPE(rtMaterial)

	friend class ResourceMgr;

public:
	~Material();

	const MaterialDesc& GetDesc() const { return desc_; }

	////////////////////////////////////////////////////////////////////////////////////////
	// 셰이더/텍스처 (ResourceMgr 키)
	_u64 GetVertexShaderKey() const { return vertexShaderKey_; }
	_u64 GetPixelShaderKey() const { return pixelShaderKey_; }

	// @param _slot: 셰이더의 register(t0)~t7 번호
	_u64 GetTextureKey(_u32 _slot) const;
	color GetBaseColor() const { return color::FromFloat(constants_.baseColor_[0], constants_.baseColor_[1], constants_.baseColor_[2], constants_.baseColor_[3]); }

	// 이 머티리얼의 모든 상태를 파이프라인에 장착한다.
	// (셰이더/텍스처 키는 ResourceMgr에서 그때그때 해석된다)
	// @return 셰이더 해서 실패 등으로 그릴 수 없으면 false
	bool Bind(GraphicContext& _context);

private:
	Material();
	bool Initialize(GraphicDevice& _device, const MaterialDesc& _desc);
	void Finalize();
	void ApplyDesc(const MaterialDesc& _desc);

	GraphicDevice* pDevice_;			// Bind 시 RenderStates 풀/상수버퍼 조회용 (소유하지 않음)
	MaterialDesc desc_;

	_u64 vertexShaderKey_;									// VS 리소스 키
	_u64 pixelShaderKey_;									// PS 리소스 키
	_u64 textureKeys_[GraphicContext::MAX_TEXTURE_SLOTS];	// 슬롯별 텍스처 키

	// 파이프라인 상태 설정 키 (D3D 상태 객체는 RenderStates 풀이 소유)
	BlendMode blendMode_ = BlendMode::bmNone;			// 블렌드 공식
	DepthMode depthMode_ = DepthMode::dmReadWrite;		// 깊이 테스트/기록
	CullMode cullMode_ = CullMode::cmBack;				// 컬링
	FillMode fillMode_ = FillMode::fmSolid;				// 채우기
	FrontFace frontFace_ = FrontFace::ffClockwise;		// 앞면 판정
	FilterMode filter_ = FilterMode::fmLinear;			// 샘플러 필터
	AddressMode addrU_ = AddressMode::amClamp;			// 샘플러 주소 U
	AddressMode addrV_ = AddressMode::amClamp;			// 샘플러 주소 V

	MaterialConstants constants_;						// b2 상수 내용
	ConstantBuffer<MaterialConstants> constantBuffer_;	// b2 상수버퍼
	bool constantsDirty_;								// 상수 갱신 필요 여부
};

NS_SGF_END
