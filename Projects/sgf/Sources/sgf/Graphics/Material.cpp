/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:50:00 PM
 * =====================
 * 머티리얼 구현부
 */

#include "Core.h"
#include "sgf/Graphics/Material.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "sgf/Graphics/ShaderProgram.h"
#include "sgf/Graphics/Texture.h"
#include "sgf/Graphics/ResourceMgr.h"
#include "jc/Hasher.h"

#include <cstring>

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
Material::Material()
	: pDevice_(nullptr)
	, vertexShaderKey_(INVALID_RESOURCE_KEY)
	, pixelShaderKey_(INVALID_RESOURCE_KEY)
	, constantsDirty_(true)
{
	for (_u32 i = 0; i < GraphicContext::MAX_TEXTURE_SLOTS; ++i)
	{
		textureKeys_[i] = INVALID_RESOURCE_KEY;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
Material::~Material()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Material::Initialize(GraphicDevice& _device, const MaterialDesc& _desc)
{
	pDevice_ = &_device;
	ApplyDesc(_desc);

	// 상태 객체는 생성하지 않는다 — 설정 키(디폴트)만 보관하고,
	// Bind 시점에 RenderStates 풀에서 조회해 공유한다. (B-3)
	if (!constantBuffer_.Create(_device))
	{
		pDevice_ = nullptr;
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////
void Material::ApplyDesc(const MaterialDesc& _desc)
{
	desc_ = _desc;
	vertexShaderKey_ = _desc.vertexShaderKey_;
	pixelShaderKey_ = _desc.pixelShaderKey_;
	for (_u32 i = 0; i < GraphicContext::MAX_TEXTURE_SLOTS; ++i)
	{
		textureKeys_[i] = _desc.textureKeys_[i];
	}
	blendMode_ = _desc.blendMode_;
	depthMode_ = _desc.depthMode_;
	cullMode_ = _desc.cullMode_;
	fillMode_ = _desc.fillMode_;
	frontFace_ = _desc.frontFace_;
	filter_ = _desc.filter_;
	addrU_ = _desc.addrU_;
	addrV_ = _desc.addrV_;
	_desc.baseColor_.ToFloat4(constants_.baseColor_);
	constantsDirty_ = true;
}

////////////////////////////////////////////////////////////////////////////////////////
MaterialDesc MaterialDesc::Default2D()
{
	MaterialDesc desc;
	desc.vertexShaderKey_ = g_cResourceMgr.GetDefaultVertexShader2DKey();
	desc.pixelShaderKey_ = g_cResourceMgr.GetDefaultPixelShader2DKey();
	desc.blendMode_ = BlendMode::bmAlpha;
	desc.depthMode_ = DepthMode::dmDisabled;
	desc.cullMode_ = CullMode::cmNone;
	return desc;
}

////////////////////////////////////////////////////////////////////////////////////////
MaterialDesc MaterialDesc::Default3D()
{
	MaterialDesc desc;
	desc.vertexShaderKey_ = g_cResourceMgr.GetDefaultVertexShader3DKey();
	desc.pixelShaderKey_ = g_cResourceMgr.GetDefaultPixelShader3DKey();
	desc.blendMode_ = BlendMode::bmNone;
	desc.depthMode_ = DepthMode::dmReadWrite;
	desc.cullMode_ = CullMode::cmBack;
	return desc;
}

////////////////////////////////////////////////////////////////////////////////////////
_u64 MaterialDesc::Hash() const
{
	jc::HashBuilder<jc::HashAlgorithm::Fnv1a64> builder;
	builder.AppendIntegral(vertexShaderKey_);
	builder.AppendIntegral(pixelShaderKey_);
	for (_u32 i = 0; i < GraphicContext::MAX_TEXTURE_SLOTS; ++i)
	{
		builder.AppendIntegral(textureKeys_[i]);
	}
	_f32 rgba[4] = {};
	baseColor_.ToFloat4(rgba);
	for (_s32 i = 0; i < 4; ++i)
	{
		_u32 bits = 0;
		memcpy(&bits, &rgba[i], sizeof(bits));
		builder.AppendIntegral(static_cast<_u64>(bits));
	}
	builder.AppendIntegral(static_cast<_u64>(blendMode_));
	builder.AppendIntegral(static_cast<_u64>(depthMode_));
	builder.AppendIntegral(static_cast<_u64>(cullMode_));
	builder.AppendIntegral(static_cast<_u64>(fillMode_));
	builder.AppendIntegral(static_cast<_u64>(frontFace_));
	builder.AppendIntegral(static_cast<_u64>(filter_));
	builder.AppendIntegral(static_cast<_u64>(addrU_));
	builder.AppendIntegral(static_cast<_u64>(addrV_));
	return builder.Digest();
}

////////////////////////////////////////////////////////////////////////////////////////
bool MaterialDesc::operator==(const MaterialDesc& _other) const
{
	if (vertexShaderKey_ != _other.vertexShaderKey_ ||
		pixelShaderKey_ != _other.pixelShaderKey_)
	{
		return false;
	}
	for (_u32 i = 0; i < GraphicContext::MAX_TEXTURE_SLOTS; ++i)
	{
		if (textureKeys_[i] != _other.textureKeys_[i])
		{
			return false;
		}
	}
	if (blendMode_ != _other.blendMode_ ||
		depthMode_ != _other.depthMode_ ||
		cullMode_ != _other.cullMode_ ||
		fillMode_ != _other.fillMode_ ||
		frontFace_ != _other.frontFace_ ||
		filter_ != _other.filter_ ||
		addrU_ != _other.addrU_ ||
		addrV_ != _other.addrV_)
	{
		return false;
	}
	_f32 left[4] = {};
	_f32 right[4] = {};
	baseColor_.ToFloat4(left);
	_other.baseColor_.ToFloat4(right);
	for (_s32 i = 0; i < 4; ++i)
	{
		if (left[i] != right[i])
		{
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Material::Finalize()
{
	pDevice_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 Material::GetTextureKey(_u32 _slot) const
{
	jc_assert_msg(_slot < GraphicContext::MAX_TEXTURE_SLOTS, _T("텍스처 슬롯 범위를 벗어났습니다."));
	return textureKeys_[_slot];
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Material::Bind(GraphicContext& _context)
{
	// 1. 셰이더 해서 (키 -> 포인터. 제거된 리소스면 nullptr)
	VertexShader* pVs = g_cResourceMgr.Find<VertexShader>(vertexShaderKey_);
	PixelShader* pPs = g_cResourceMgr.Find<PixelShader>(pixelShaderKey_);
	if (pVs == nullptr || pPs == nullptr)
	{
		jc_assert_msg(false, _T("머티리얼의 셰이더 키가 유효하지 않습니다."));
		return false;
	}

	_context.SetVertexShader(pVs);
	_context.SetPixelShader(pPs);

	// 2. 파이프라인 상태 — RenderStates 풀에서 공유 객체를 조회해 Raw 바인딩 (B-3)
	//    같은 설정의 재질 여러 개가 같은 D3D 포인터를 쓰므로 캐시 히트 → 재바인드 없음
	RenderStates& states = pDevice_->States();
	_context.SetBlendStateRaw(states.GetBlendState(blendMode_));
	_context.SetDepthStencilStateRaw(states.GetDepthState(depthMode_));
	_context.SetRasterizerStateRaw(states.GetRasterizerState(cullMode_, fillMode_, frontFace_));
	_context.SetSamplerRaw(ShaderStage::ssPixel, 0, states.GetSamplerState(filter_, addrU_, addrV_));

	// 3. 텍스처 (슬롯 0은 키가 없으면 디폴트 흰색 텍스처로 대체)
	for (_u32 slot = 0; slot < GraphicContext::MAX_TEXTURE_SLOTS; ++slot)
	{
		if (textureKeys_[slot] != INVALID_RESOURCE_KEY)
		{
			_context.SetTexture(ShaderStage::ssPixel, slot, g_cResourceMgr.Find<Texture>(textureKeys_[slot]));
		}
		else if (slot == 0)
		{
			_context.SetTexture(ShaderStage::ssPixel, 0, g_cResourceMgr.GetDefaultTexture());
		}
	}

	// 4. 머티리얼 상수 (변경되었을 때만 업로드)
	if (constantsDirty_)
	{
		constantBuffer_.Update(pDevice_->Context(), constants_);
		constantsDirty_ = false;
	}
	_context.SetConstantBuffer(ShaderStage::ssPixel, 2, constantBuffer_.Raw());
	return true;
}

NS_SGF_END
