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
	, bConstantsDirty_(true)
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

	// 디폴트 상태 조합
	if (!rasterizer_.Initialize(_pDevice)) { return false; }
	if (!blend_.Initialize(_pDevice)) { return false; }
	if (!depth_.Initialize(_pDevice)) { return false; }
	if (!sampler_.Initialize(_pDevice)) { return false; }

	if (!constantBuffer_.Create(_pDevice)) { return false; }
	bConstantsDirty_ = true;
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
	return rasterizer_.Initialize(pDevice_, _cull, _fill, _frontFace);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Material::SetBlend(BlendMode _mode)
{
	jc_assert_msg(pDevice_ != nullptr, "Initialize 이후에만 상태를 바꿀 수 있습니다.");
	return blend_.Initialize(pDevice_, _mode);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Material::SetDepth(DepthMode _mode)
{
	jc_assert_msg(pDevice_ != nullptr, "Initialize 이후에만 상태를 바꿀 수 있습니다.");
	return depth_.Initialize(pDevice_, _mode);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Material::SetSampler(FilterMode _filter, AddressMode _addressU, AddressMode _addressV)
{
	jc_assert_msg(pDevice_ != nullptr, "Initialize 이후에만 상태를 바꿀 수 있습니다.");
	return sampler_.Initialize(pDevice_, _filter, _addressU, _addressV);
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

	// 2. 파이프라인 상태
	_context.SetRasterizerState(&rasterizer_);
	_context.SetBlendState(&blend_);
	_context.SetDepthStencilState(&depth_);
	_context.SetSampler(ShaderStage::ssPixel, 0, &sampler_);

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
	if (bConstantsDirty_)
	{
		constantBuffer_.Update(pDevice_, constants_);
		bConstantsDirty_ = false;
	}
	_context.SetConstantBuffer(ShaderStage::ssPixel, 2, constantBuffer_.Raw());
	return true;
}

NS_SGF_END
