/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:50:00 PM
 * =====================
 * 머티리얼 (v3: 셰이더 + 파이프라인 상태 + 텍스처 조합)
 *
 * [머티리얼이란?]
 *  "어떻게 그릴지"의 묶음. 메시(무엇을)와 분리되어 있어서
 *  같은 큐브를 벽돌 재질로도, 금속 재질로도 그릴 수 있다.
 *
 * [참조 방식] (v3 D-13)
 *  셰이더/텍스처는 포인터가 아닌 ResourceMgr 키(_u64)로 보관한다.
 *  리소스가 제거되면 Bind 시점에 안전하게 감지된다. (댐링 포인터 방지)
 *
 * [상수버퍼 규약] b2 = 머티리얼 상수 (baseColor)
 */

#pragma once

#include "sgf/Graphics/IResource.h"
#include "sgf/Graphics/GraphicsEnums.h"
#include "sgf/Graphics/PipelineState.h"
#include "sgf/Graphics/Buffers.h"
#include "sgf/Graphics/GraphicContext.h"

NS_SGF_BEGIN

using namespace jc;

class GraphicDevice;

// b2 슬롯에 올라가는 머티리얼 상수 (16바이트 배수)
struct MaterialConstants
{
	color baseColor_{ 1.0f, 1.0f, 1.0f, 1.0f };	// 기본색/틴트
};

class Material : public ResourceBase
{
	SGF_RESOURCE_TYPE(rtMaterial)

public:
	Material();
	~Material();

	// 디폴트 상태(솔리드+백컬링, 블렌드 없음, 깊이 읽기쓰기, Linear+Clamp)로 준비한다.
	// 셰이더 키는 별도로 지정해야 한다. (SetVertexShaderKey/SetPixelShaderKey)
	bool Initialize(GraphicDevice* _pDevice);

	void Finalize();

	// === 셰이더/텍스처 (ResourceMgr 키) ===
	void SetVertexShaderKey(_u64 _key) { vertexShaderKey_ = _key; }
	void SetPixelShaderKey(_u64 _key) { pixelShaderKey_ = _key; }
	_u64 GetVertexShaderKey() const { return vertexShaderKey_; }
	_u64 GetPixelShaderKey() const { return pixelShaderKey_; }

	// @param _slot : 셰이더의 register(t0)~t7 번호
	void SetTextureKey(_u32 _slot, _u64 _key);
	_u64 GetTextureKey(_u32 _slot) const;

	// === 파이프라인 상태 ===
	bool SetRasterizer(CullMode _cull, FillMode _fill = FillMode::fmSolid, FrontFace _frontFace = FrontFace::ffClockwise);
	bool SetBlend(BlendMode _mode);
	bool SetDepth(DepthMode _mode);
	bool SetSampler(FilterMode _filter, AddressMode _addressU = AddressMode::amClamp, AddressMode _addressV = AddressMode::amClamp);

	const RasterizerState& GetRasterizer() const { return rasterizer_; }
	const BlendState& GetBlend() const { return blend_; }
	const DepthStencilState& GetDepth() const { return depth_; }
	const SamplerState& GetSampler() const { return sampler_; }

	// === 머티리얼 상수 ===
	void SetBaseColor(const color& _color) { constants_.baseColor_ = _color; constantsDirty_ = true; }
	const color& GetBaseColor() const { return constants_.baseColor_; }

	// 이 머티리얼의 모든 상태를 파이프라인에 장착한다.
	// (셰이더/텍스처 키는 ResourceMgr에서 그때그때 해석된다)
	// @return 셰이더 해서 실패 등으로 그릴 수 없으면 false
	bool Bind(GraphicContext& _context);

private:
	GraphicDevice* pDevice_;			// 상태 재설정용 (소유하지 않음)

	_u64 vertexShaderKey_;				// VS 리소스 키
	_u64 pixelShaderKey_;				// PS 리소스 키
	_u64 textureKeys_[GraphicContext::MAX_TEXTURE_SLOTS];	// 슬롯별 텍스처 키

	RasterizerState rasterizer_;		// 래스터라이저 상태
	BlendState blend_;					// 블렌드 상태
	DepthStencilState depth_;			// 깊이 상태
	SamplerState sampler_;				// 샘플러 (슬롯 0)

	MaterialConstants constants_;		// b2 상수 내용
	ConstantBuffer<MaterialConstants> constantBuffer_;	// b2 상수버퍼
	bool constantsDirty_;				// 상수 갱신 필요 여부
};

NS_SGF_END
