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
bool Material::Initialize(GraphicDevice* _pDevice)
{
	pDevice_ = _pDevice;

	// 상태 객체는 생성하지 않는다 — 설정 키(디폴트)만 보관하고,
	// Bind 시점에 RenderStates 풀에서 조회해 공유한다. (B-3)
	if (!constantBuffer_.Create(_pDevice)) { return false; }
	constantsDirty_ = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Material::Finalize()
{
	pDevice_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Material::SetTextureKey(_u32 _slot, _u64 _key)
{
	jc_assert_msg(_slot < GraphicContext::MAX_TEXTURE_SLOTS, "텍스처 슬롯 범위를 벗어났습니다.");
	textureKeys_[_slot] = _key;
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 Material::GetTextureKey(_u32 _slot) const
{
	jc_assert_msg(_slot < GraphicContext::MAX_TEXTURE_SLOTS, "텍스처 슬롯 범위를 벗어났습니다.");
	return textureKeys_[_slot];
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Material::SetRasterizer(CullMode _cull, FillMode _fill, FrontFace _frontFace)
{
	jc_assert_msg(pDevice_ != nullptr, "Initialize 이후에만 상태를 바꿀 수 있습니다.");
	cullMode_ = _cull;
	fillMode_ = _fill;
	frontFace_ = _frontFace;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Material::SetBlend(BlendMode _mode)
{
	jc_assert_msg(pDevice_ != nullptr, "Initialize 이후에만 상태를 바꿀 수 있습니다.");
	blendMode_ = _mode;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Material::SetDepth(DepthMode _mode)
{
	jc_assert_msg(pDevice_ != nullptr, "Initialize 이후에만 상태를 바꿀 수 있습니다.");
	depthMode_ = _mode;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Material::SetSampler(FilterMode _filter, AddressMode _addressU, AddressMode _addressV)
{
	jc_assert_msg(pDevice_ != nullptr, "Initialize 이후에만 상태를 바꿀 수 있습니다.");
	filter_ = _filter;
	addrU_ = _addressU;
	addrV_ = _addressV;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Material::Bind(GraphicContext& _context)
{
	// 1. 셰이더 해서 (키 -> 포인터. 제거된 리소스면 nullptr)
	VertexShader* pVs = g_cResourceMgr.Find<VertexShader>(vertexShaderKey_);
	PixelShader* pPs = g_cResourceMgr.Find<PixelShader>(pixelShaderKey_);
	if (pVs == nullptr || pPs == nullptr)
	{
		jc_assert_msg(false, "머티리얼의 셰이더 키가 유효하지 않습니다.");
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
		constantBuffer_.Update(pDevice_, constants_);
		constantsDirty_ = false;
	}
	_context.SetConstantBuffer(ShaderStage::ssPixel, 2, constantBuffer_.Raw());
	return true;
}

NS_SGF_END
