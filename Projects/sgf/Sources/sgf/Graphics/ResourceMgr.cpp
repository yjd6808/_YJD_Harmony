/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 6:10:00 PM
 * =====================
 * 리소스 매니저 구현부 — [type 8][gen 24][index 32]
 */

#include "Core.h"
#include "sgf/Graphics/ResourceMgr.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "sgf/Graphics/Texture.h"
#include "sgf/Graphics/Material.h"
#include "sgf/Graphics/Mesh.h"
#include "sgf/Graphics/ShaderProgram.h"
#include "sgf/Graphics/DefaultShaders.h"
#include "jc/Container/Vector.h"

#include <cstdio>

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
namespace
{
	//////////////////////////////////////////////////////////////////////////////////////
	bool CreatePrimitiveMesh2D(GraphicDevice* _pDevice, ResourceMgr& _mgr,
		PrimitiveMesh2DType _type, _u64& _outKey)
	{
		Mesh* pMesh = dbg_new Mesh;
		bool ok = false;
		switch (_type)
		{
		case PrimitiveMesh2DType::Rect:		ok = pMesh->InitializeAsRect2D(_pDevice); break;
		case PrimitiveMesh2DType::Circle:	ok = pMesh->InitializeAsCircle2D(_pDevice); break;
		case PrimitiveMesh2DType::Triangle:	ok = pMesh->InitializeAsTriangle2D(_pDevice); break;
		case PrimitiveMesh2DType::Line:		ok = pMesh->InitializeAsLine2D(_pDevice); break;
		default: ok = false; break;
		}
		if (!ok) { delete pMesh; return false; }
		_outKey = _mgr.Add(pMesh);
		return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	bool CreatePrimitiveMesh3D(GraphicDevice* _pDevice, ResourceMgr& _mgr,
		PrimitiveMesh3DType _type, _u64& _outKey)
	{
		Mesh* pMesh = dbg_new Mesh;
		bool ok = false;
		switch (_type)
		{
		case PrimitiveMesh3DType::Cube:		ok = pMesh->InitializeAsCube(_pDevice); break;
		case PrimitiveMesh3DType::Sphere:	ok = pMesh->InitializeAsSphere(_pDevice); break;
		case PrimitiveMesh3DType::Capsule:	ok = pMesh->InitializeAsCapsule(_pDevice); break;
		case PrimitiveMesh3DType::Cylinder:	ok = pMesh->InitializeAsCylinder(_pDevice); break;
		case PrimitiveMesh3DType::Plane:	ok = pMesh->InitializeAsPlane(_pDevice); break;
		case PrimitiveMesh3DType::Quad:		ok = pMesh->InitializeAsQuad3D(_pDevice); break;
		default: ok = false; break;
		}
		if (!ok) { delete pMesh; return false; }
		_outKey = _mgr.Add(pMesh);
		return true;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
ResourceMgr& ResourceMgr::GetInstance()
{
	static ResourceMgr s_instance;
	return s_instance;
}

//////////////////////////////////////////////////////////////////////////////////////////
ResourceMgr::ResourceMgr()
	: pDevice_(nullptr)
	, defaultTextureKey_(INVALID_RESOURCE_KEY)
	, defaultVs2DKey_(INVALID_RESOURCE_KEY)
	, defaultPs2DKey_(INVALID_RESOURCE_KEY)
	, defaultVs3DKey_(INVALID_RESOURCE_KEY)
	, defaultPs3DKey_(INVALID_RESOURCE_KEY)
	, defaultMaterial2DKey_(INVALID_RESOURCE_KEY)
	, defaultMaterial3DKey_(INVALID_RESOURCE_KEY)
{
	for (_s32 i = 0; i < PRIMITIVE_MESH2D_COUNT; ++i)
	{
		primitiveMesh2DKeys_[i] = INVALID_RESOURCE_KEY;
	}
	for (_s32 i = 0; i < PRIMITIVE_MESH3D_COUNT; ++i)
	{
		primitiveMesh3DKeys_[i] = INVALID_RESOURCE_KEY;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
ResourceMgr::~ResourceMgr()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ResourceMgr::Initialize(GraphicDevice* _pDevice)
{
	jc_assert_msg(_pDevice != nullptr, "GraphicDevice가 필요합니다.");

	if (pDevice_ != nullptr)
	{
		if (pDevice_ == _pDevice)
		{
			return true;
		}
		Finalize();
	}

	pDevice_ = _pDevice;
	pDevice_->SetResourceRegistry(this);

	if (!CreateDefaults())
	{
		Finalize();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ResourceMgr::Finalize()
{
	if (pDevice_ == nullptr)
	{
		return;
	}

	// 디바이스에서 레지스트리 분리 (Device가 Mgr보다 오래 살 수 있음)
	pDevice_->SetResourceRegistry(nullptr);

	// 1. 사용자 리소스 제거 (디폴트 제외)
	RemoveAll();

	// 2. 디폴트 포함 잔여 전부 소멸 — 슬롯 순회
	for (_s32 t = 0; t < TYPE_COUNT; ++t)
	{
		for (_s32 i = 0; i < slots_[t].Size(); ++i)
		{
			Slot& slot = slots_[t][i];
			if (slot.pResource_ != nullptr)
			{
				delete slot.pResource_;
				slot.pResource_ = nullptr;
			}
		}
		slots_[t].Clear();
		indexProviders_[t].Reset();
	}

	pathIndex_.Clear();
	defaultKeys_.Clear();

	defaultTextureKey_ = INVALID_RESOURCE_KEY;
	defaultVs2DKey_ = INVALID_RESOURCE_KEY;
	defaultPs2DKey_ = INVALID_RESOURCE_KEY;
	defaultVs3DKey_ = INVALID_RESOURCE_KEY;
	defaultPs3DKey_ = INVALID_RESOURCE_KEY;
	defaultMaterial2DKey_ = INVALID_RESOURCE_KEY;
	defaultMaterial3DKey_ = INVALID_RESOURCE_KEY;
	for (_s32 i = 0; i < PRIMITIVE_MESH2D_COUNT; ++i)
	{
		primitiveMesh2DKeys_[i] = INVALID_RESOURCE_KEY;
	}
	for (_s32 i = 0; i < PRIMITIVE_MESH3D_COUNT; ++i)
	{
		primitiveMesh3DKeys_[i] = INVALID_RESOURCE_KEY;
	}
	pDevice_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// IResourceRegistry
//////////////////////////////////////////////////////////////////////////////////////////
_u64 ResourceMgr::Register(IResource* _pResource)
{
	jc_assert_msg(pDevice_ != nullptr, "Initialize 이후에만 등록할 수 있습니다.");
	jc_assert_msg(_pResource != nullptr, "등록할 리소스가 비어있습니다.");
	jc_assert_msg(_pResource->GetKey() == INVALID_RESOURCE_KEY, "이미 등록된 리소스입니다.");

	const ResourceType type = _pResource->GetResourceType();
	jc_assert_msg(type != ResourceType::rtUnknown, "알 수 없는 리소스 타입입니다.");
	const _s32 t = static_cast<_s32>(type);
	jc_assert_msg(t >= 0 && t < TYPE_COUNT, "타입 범위 초과");

	const _u32 raw = static_cast<_u32>(indexProviders_[t].Acquire());
	const _u32 index = raw - 1; // 1-base → 0-base

	// 슬롯 배열 확장
	if (index >= static_cast<_u32>(slots_[t].Size()))
	{
		// Acquire는 1씩 증가하므로 index == Size인 경우만 정상
		jc_assert_msg(index == static_cast<_u32>(slots_[t].Size()), "인덱스 연속성 위반");
		slots_[t].PushBack(Slot{});
		// 새로 추가된 슬롯의 gen은 기본 1 유지
	}

	Slot& slot = slots_[t][index];
	jc_assert_msg(slot.pResource_ == nullptr, "슬롯이 이미 점유됨 — 발급기 중복");

	slot.pResource_ = _pResource;
	const _u64 key = MakeResourceKey(type, slot.gen_, index);
	static_cast<ResourceBase*>(_pResource)->SetKey(key);
	return key;
}

//////////////////////////////////////////////////////////////////////////////////////////
IResource* ResourceMgr::Resolve(_u64 _key)
{
	return Find(_key);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ResourceMgr::Unregister(_u64 _key)
{
	return Remove(_key);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 ResourceMgr::Add(IResource* _pResource)
{
	return Register(_pResource);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 ResourceMgr::Add(IResource* _pResource, StringView _path)
{
	if (FindKeyByPath(_path) != INVALID_RESOURCE_KEY)
	{
		jc_assert_msg(false, "같은 경로의 리소스가 이미 등록되어 있습니다.");
		return INVALID_RESOURCE_KEY;
	}

	const _u64 key = Register(_pResource);
	static_cast<ResourceBase*>(_pResource)->SetPath(_path);
	pathIndex_.Insert(String(_path.SafeSource()), key);
	return key;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ResourceMgr::Remove(_u64 _key)
{
	if (_key == INVALID_RESOURCE_KEY) { return false; }

	if (defaultKeys_.Exist(_key))
	{
		jc_assert_msg(false, "디폴트 리소스는 제거할 수 없습니다.");
		return false;
	}

	const ResourceType type = GetResourceTypeFromKey(_key);
	const _s32 t = static_cast<_s32>(type);
	if (t <= 0 || t >= TYPE_COUNT) { return false; }

	const _u32 gen = GetResourceGenFromKey(_key);
	const _u32 index = GetResourceIndexFromKey(_key);
	if (gen == 0) { return false; }
	if (index >= static_cast<_u32>(slots_[t].Size())) { return false; }

	Slot& slot = slots_[t][index];
	if (slot.pResource_ == nullptr) { return false; }
	if (slot.gen_ != gen) { return false; } // 낡은 키
	if (slot.pResource_->GetResourceType() != type) { return false; }

	RemovePathEntry(_key);
	delete slot.pResource_;
	slot.pResource_ = nullptr;

	// 세대 증가 (0 건너뜀, 0xFFFFFF 이후 1로 순환)
	if (slot.gen_ >= RESOURCE_KEY_GEN_MAX)
	{
		slot.gen_ = 1;
	}
	else
	{
		slot.gen_ += 1;
		if (slot.gen_ == 0) { slot.gen_ = 1; }
	}

	indexProviders_[t].Release(index + 1);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ResourceMgr::RemoveAll()
{
	Vector<_u64> keys;
	keys.Reserve(GetCount());
	for (_s32 t = 0; t < TYPE_COUNT; ++t)
	{
		for (_s32 i = 0; i < slots_[t].Size(); ++i)
		{
			Slot& slot = slots_[t][i];
			if (slot.pResource_ != nullptr)
			{
				keys.PushBack(MakeResourceKey(static_cast<ResourceType>(t), slot.gen_, static_cast<_u32>(i)));
			}
		}
	}

	for (_s32 i = 0; i < keys.Size(); ++i)
	{
		const _u64 key = keys.At(i);
		if (defaultKeys_.Exist(key))
		{
			continue;
		}
		Remove(key);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
IResource* ResourceMgr::Find(_u64 _key)
{
	if (_key == INVALID_RESOURCE_KEY)
	{
		return nullptr;
	}

	const ResourceType type = GetResourceTypeFromKey(_key);
	const _s32 t = static_cast<_s32>(type);
	if (t <= 0 || t >= TYPE_COUNT) { return nullptr; }

	const _u32 gen = GetResourceGenFromKey(_key);
	const _u32 index = GetResourceIndexFromKey(_key);
	if (gen == 0) { return nullptr; }
	if (index >= static_cast<_u32>(slots_[t].Size())) { return nullptr; }

	Slot& slot = slots_[t][index];
	if (slot.pResource_ == nullptr) { return nullptr; }
	if (slot.gen_ != gen) { return nullptr; }
	return slot.pResource_;
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 ResourceMgr::FindKeyByPath(StringView _path)
{
	_u64* pKey = pathIndex_.Find(String(_path.SafeSource()));
	return (pKey != nullptr) ? *pKey : INVALID_RESOURCE_KEY;
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 ResourceMgr::LoadTextureFromFile(const jc::String& _szFilePath)
{
	jc_assert_msg(pDevice_ != nullptr, "Initialize 이후에만 로드할 수 있습니다.");

	const _u64 existingKey = FindKeyByPath(StringView{ _szFilePath });
	if (existingKey != INVALID_RESOURCE_KEY)
	{
		return existingKey;
	}

	Texture* pTexture = dbg_new Texture;
	if (!pTexture->LoadFromFile(pDevice_, _szFilePath))
	{
		delete pTexture;
		return INVALID_RESOURCE_KEY;
	}

	pTexture->SetDebugName(StringView{ _szFilePath });
	return Add(pTexture, StringView{ _szFilePath });
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 ResourceMgr::LoadTextureFromSvgFile(const jc::String& _szFilePath, _f32 _scale)
{
	jc_assert_msg(pDevice_ != nullptr, "Initialize 이후에만 로드할 수 있습니다.");

	const _u64 existingKey = FindKeyByPath(StringView{ _szFilePath });
	if (existingKey != INVALID_RESOURCE_KEY)
	{
		return existingKey;
	}

	Texture* pTexture = dbg_new Texture;
	if (!pTexture->LoadFromSvgFile(pDevice_, _szFilePath, _scale))
	{
		delete pTexture;
		return INVALID_RESOURCE_KEY;
	}

	pTexture->SetDebugName(StringView{ _szFilePath });
	return Add(pTexture, StringView{ _szFilePath });
}

//////////////////////////////////////////////////////////////////////////////////////////
Texture* ResourceMgr::GetDefaultTexture()
{
	return Find<Texture>(defaultTextureKey_);
}

//////////////////////////////////////////////////////////////////////////////////////////
VertexShader* ResourceMgr::GetDefaultVertexShader2D()
{
	return Find<VertexShader>(defaultVs2DKey_);
}

//////////////////////////////////////////////////////////////////////////////////////////
PixelShader* ResourceMgr::GetDefaultPixelShader2D()
{
	return Find<PixelShader>(defaultPs2DKey_);
}

//////////////////////////////////////////////////////////////////////////////////////////
VertexShader* ResourceMgr::GetDefaultVertexShader3D()
{
	return Find<VertexShader>(defaultVs3DKey_);
}

//////////////////////////////////////////////////////////////////////////////////////////
PixelShader* ResourceMgr::GetDefaultPixelShader3D()
{
	return Find<PixelShader>(defaultPs3DKey_);
}

//////////////////////////////////////////////////////////////////////////////////////////
Material* ResourceMgr::GetDefaultMaterial2D()
{
	return Find<Material>(defaultMaterial2DKey_);
}

//////////////////////////////////////////////////////////////////////////////////////////
Material* ResourceMgr::GetDefaultMaterial3D()
{
	return Find<Material>(defaultMaterial3DKey_);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 공통 헬퍼 — 프리미티브 인덱스 검증
inline bool IsValidPrimitiveIndex(_s32 _index, _s32 _count) { return _index >= 0 && _index < _count; }

Mesh* ResourceMgr::FindPrimitiveMesh2D(PrimitiveMesh2DType _type)
{
	const _s32 index = static_cast<_s32>(_type);
	if (!IsValidPrimitiveIndex(index, PRIMITIVE_MESH2D_COUNT)) { return nullptr; }
	return Find<Mesh>(primitiveMesh2DKeys_[index]);
}

//////////////////////////////////////////////////////////////////////////////////////////
Mesh* ResourceMgr::FindPrimitiveMesh3D(PrimitiveMesh3DType _type)
{
	const _s32 index = static_cast<_s32>(_type);
	if (!IsValidPrimitiveIndex(index, PRIMITIVE_MESH3D_COUNT)) { return nullptr; }
	return Find<Mesh>(primitiveMesh3DKeys_[index]);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 ResourceMgr::GetPrimitiveMesh2DKey(PrimitiveMesh2DType _type) const
{
	const _s32 index = static_cast<_s32>(_type);
	if (!IsValidPrimitiveIndex(index, PRIMITIVE_MESH2D_COUNT)) { return INVALID_RESOURCE_KEY; }
	return primitiveMesh2DKeys_[index];
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 ResourceMgr::GetPrimitiveMesh3DKey(PrimitiveMesh3DType _type) const
{
	const _s32 index = static_cast<_s32>(_type);
	if (!IsValidPrimitiveIndex(index, PRIMITIVE_MESH3D_COUNT)) { return INVALID_RESOURCE_KEY; }
	return primitiveMesh3DKeys_[index];
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 ResourceMgr::GetCount() const
{
	_s32 count = 0;
	for (_s32 t = 0; t < TYPE_COUNT; ++t)
	{
		for (_s32 i = 0; i < slots_[t].Size(); ++i)
		{
			if (slots_[t][i].pResource_ != nullptr) { ++count; }
		}
	}
	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ResourceMgr::FormatKey(_u64 _key, _s8* _outBuffer, _sz _bufferSize)
{
	if (_key == INVALID_RESOURCE_KEY)
	{
		sprintf_s(_outBuffer, _bufferSize, "INVALID");
		return;
	}
	const ResourceType type = GetResourceTypeFromKey(_key);
	const _u32 gen = GetResourceGenFromKey(_key);
	const _u32 index = GetResourceIndexFromKey(_key);
	const char* typeName = (static_cast<_s32>(type) >= 0 && static_cast<_s32>(type) < static_cast<_s32>(ResourceType::Max))
		? RESOURCE_TYPE_NAMES[static_cast<_s32>(type)] : "Unknown";
	sprintf_s(_outBuffer, _bufferSize, "%s(gen=%u, idx=%u) 0x%016llX", typeName, gen, index, _key);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ResourceMgr::PrintStatus()
{
	_s8 buffer[512];
	_s8 keyBuf[128];
	_s32 total = GetCount();
	_sz acquired = 0;
	for (_s32 t = 0; t < TYPE_COUNT; ++t) { acquired += indexProviders_[t].GetAcquiredCount(); }
	sprintf_s(buffer, "[ResourceMgr] 보관 중: %d개, 발급 중인 슬롯: %zu개\n", total, acquired);
	OutputDebugStringA(buffer);

	for (_s32 t = 0; t < TYPE_COUNT; ++t)
	{
		for (_s32 i = 0; i < slots_[t].Size(); ++i)
		{
			const Slot& slot = slots_[t][i];
			if (slot.pResource_ == nullptr) { continue; }
			const IResource* pResource = slot.pResource_;
			const _u64 key = MakeResourceKey(static_cast<ResourceType>(t), slot.gen_, static_cast<_u32>(i));
			FormatKey(key, keyBuf, sizeof(keyBuf));
			sprintf_s(buffer, "  key=%s type=%s name=%s\n",
				keyBuf,
				RESOURCE_TYPE_NAMES[static_cast<_s32>(pResource->GetResourceType())],
				pResource->GetDebugName());
			OutputDebugStringA(buffer);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ResourceMgr::CreateDefaults()
{
	// 1. 디폴트 셰이더 (소스 내장. D-17)
	VertexShader* pVs2D = dbg_new VertexShader;
	if (!pVs2D->InitializeFromSource(pDevice_, DEFAULT_SHADER_SOURCE_2D))
	{
		delete pVs2D;
		return false;
	}
	pVs2D->SetDebugName("DefaultVS2D");
	defaultVs2DKey_ = Add(pVs2D);

	PixelShader* pPs2D = dbg_new PixelShader;
	if (!pPs2D->InitializeFromSource(pDevice_, DEFAULT_SHADER_SOURCE_2D))
	{
		delete pPs2D;
		return false;
	}
	pPs2D->SetDebugName("DefaultPS2D");
	defaultPs2DKey_ = Add(pPs2D);

	VertexShader* pVs3D = dbg_new VertexShader;
	if (!pVs3D->InitializeFromSource(pDevice_, DEFAULT_SHADER_SOURCE_3D))
	{
		delete pVs3D;
		return false;
	}
	pVs3D->SetDebugName("DefaultVS3D");
	defaultVs3DKey_ = Add(pVs3D);

	PixelShader* pPs3D = dbg_new PixelShader;
	if (!pPs3D->InitializeFromSource(pDevice_, DEFAULT_SHADER_SOURCE_3D))
	{
		delete pPs3D;
		return false;
	}
	pPs3D->SetDebugName("DefaultPS3D");
	defaultPs3DKey_ = Add(pPs3D);

	// 2. 1x1 흰색 텍스처 (D-18. 텍스처 없는 머티리얼의 대체재)
	const _u8 whitePixel[4] = { 255, 255, 255, 255 };
	Texture* pWhite = dbg_new Texture;
	if (!pWhite->CreateFromMemory(pDevice_, whitePixel, 1, 1))
	{
		delete pWhite;
		return false;
	}
	pWhite->SetDebugName("DefaultWhiteTexture");
	defaultTextureKey_ = Add(pWhite);

	// 3. 디폴트 머티리얼 (2D: 알파 블렌드 + 깊이 끄기 / 3D: 불투명 + 깊이 읽기쓰기)
	Material* pMat2D = dbg_new Material;
	if (!pMat2D->Initialize(pDevice_)
		|| !pMat2D->SetBlend(BlendMode::bmAlpha)
		|| !pMat2D->SetDepth(DepthMode::dmDisabled))
	{
		delete pMat2D;
		return false;
	}
	pMat2D->SetVertexShaderKey(defaultVs2DKey_);
	pMat2D->SetPixelShaderKey(defaultPs2DKey_);
	pMat2D->SetDebugName("DefaultMaterial2D");
	defaultMaterial2DKey_ = Add(pMat2D);

	Material* pMat3D = dbg_new Material;
	if (!pMat3D->Initialize(pDevice_))
	{
		delete pMat3D;
		return false;
	}
	pMat3D->SetVertexShaderKey(defaultVs3DKey_);
	pMat3D->SetPixelShaderKey(defaultPs3DKey_);
	pMat3D->SetDebugName("DefaultMaterial3D");
	defaultMaterial3DKey_ = Add(pMat3D);

	// 4. 프리미티브 메시 (2D: 순수 2D 4종 — vfPTC2D 배칭용 / 3D 공용 6종 — vfPNT3D)
	// 2D/3D 별도 키 테이블 — 각 enum은 자기 영역만 담당한다.
	for (_s32 t = 0; t < PRIMITIVE_MESH2D_COUNT; ++t)
	{
		if (!CreatePrimitiveMesh2D(pDevice_, *this, static_cast<PrimitiveMesh2DType>(t), primitiveMesh2DKeys_[t]))
		{
			return false;
		}
	}
	for (_s32 t = 0; t < PRIMITIVE_MESH3D_COUNT; ++t)
	{
		if (!CreatePrimitiveMesh3D(pDevice_, *this, static_cast<PrimitiveMesh3DType>(t), primitiveMesh3DKeys_[t]))
		{
			return false;
		}
	}

	// 5. 디폴트 키 보호 등록 (FR-30)
	defaultKeys_.Insert(defaultVs2DKey_);
	defaultKeys_.Insert(defaultPs2DKey_);
	defaultKeys_.Insert(defaultVs3DKey_);
	defaultKeys_.Insert(defaultPs3DKey_);
	defaultKeys_.Insert(defaultTextureKey_);
	defaultKeys_.Insert(defaultMaterial2DKey_);
	defaultKeys_.Insert(defaultMaterial3DKey_);
	for (_s32 i = 0; i < PRIMITIVE_MESH2D_COUNT; ++i)
	{
		defaultKeys_.Insert(primitiveMesh2DKeys_[i]);
	}
	for (_s32 i = 0; i < PRIMITIVE_MESH3D_COUNT; ++i)
	{
		defaultKeys_.Insert(primitiveMesh3DKeys_[i]);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ResourceMgr::RemovePathEntry(_u64 _key)
{
	// 리소스가 자기 경로를 보관하므로, 역방향 인덱스(선형 탐색) 없이 바로 제거한다. (B-7)
	IResource* pResource = Find(_key);
	if (pResource == nullptr)
	{
		return;
	}

	const char* szPath = static_cast<ResourceBase*>(pResource)->GetPath();
	if (szPath[0] != '\0')
	{
		pathIndex_.Remove(String(szPath));
	}
}

NS_SGF_END
