/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 6:10:00 PM
 * =====================
 * 리소스 매니저 (v3: 고유 리소스 통합 관리)
 *
 * [역할]
 *  - Texture/Material/Mesh/셰이더 등 IResource 구현체의 소유권을 넘겨받아 보관한다.
 *  - 키는 jc::Provider<AutoIncrementLinkedList<_u64>>가 발급한다. (반납 키 재사용. v3 D-12)
 *  - 경로로 로드한 리소스는 경로 인덱스로 중복 로드를 막는다.
 *  - 2D/3D 디폴트 셰이더/머티리얼/흰색 텍스처를 Initialize에서 만들고
 *    Finalize에서 소멸시킨다. 디폴트 키는 Remove로 지울 수 없다. (FR-30)
 *
 * [사용법]
 *  _u64 texKey = g_cResourceMgr.LoadTextureFromFile("Resources/hero.png");
 *  Texture* pTex = g_cResourceMgr.Find<Texture>(texKey);
 *  g_cResourceMgr.Remove(texKey);
 */

#pragma once

#include "sgf/Graphics/IResource.h"
#include "jc/Container/HashMap.h"
#include "jc/Container/HashSet.h"
#include "jc/Pool/Provider.h"
#include "jc/Primitives/String.h"
#include "jc/Primitives/StringView.h"

NS_SGF_BEGIN

using namespace jc;

class GraphicDevice;
class Texture;
class Material;
class VertexShader;
class PixelShader;

class ResourceMgr
{
public:
	static ResourceMgr& GetInstance();

	// 디폴트 리소스(셰이더/머티리얼/흰색 텍스처)까지 생성한다.
	bool Initialize(GraphicDevice* _pDevice);

	// 남은 모든 리소스(디폴트 포함)를 소멸시키고 키 발급기를 되돌린다.
	void Finalize();

	bool IsInitialized() const { return pDevice_ != nullptr; }

	// === 등록/제거 ===

	// 리소스 소유권을 넘겨받고 새 키를 발급한다. (이후 소멸은 ResourceMgr 담당)
	_u64 Add(IResource* _pResource);

	// 경로 인덱스와 함께 등록한다. 같은 경로가 이미 있으면 등록 실패(INVALID_RESOURCE_KEY).
	_u64 Add(IResource* _pResource, StringView _path);

	// 키의 리소스를 소멸시키고 키를 반납한다. 디폴트 키는 거부한다.
	bool Remove(_u64 _key);

	// 디폴트를 제외한 모든 리소스를 제거한다. (씬 전환 등)
	void RemoveAll();

	// === 검색 ===

	// 키로 찾는다. 없으면 nullptr.
	IResource* Find(_u64 _key);

	// 키로 찾고 타입까지 검증한다. 타입이 다르면 nullptr.
	template <typename T>
	T* Find(_u64 _key)
	{
		IResource* pResource = Find(_key);
		if (pResource == nullptr || pResource->GetResourceType() != T::RESOURCE_TYPE)
		{
			return nullptr;
		}
		return static_cast<T*>(pResource);
	}

	// 경로로 등록된 리소스의 키를 찾는다. 없으면 INVALID_RESOURCE_KEY.
	_u64 FindKeyByPath(StringView _path);

	// === 편의 로드 (경로 중복 로드 방지) ===

	// PNG 등 이미지 파일을 로드해 등록한다. 이미 로드된 경로면 기존 키를 반환.
	_u64 LoadTextureFromFile(const char* _szFilePath);

	// SVG 파일을 로드해 등록한다. 이미 로드된 경로면 기존 키를 반환.
	_u64 LoadTextureFromSvgFile(const char* _szFilePath, _f32 _scale = 1.0f);

	// === 디폴트 리소스 (D-17/D-18) ===

	Texture* GetDefaultTexture();				// 1x1 흰색 텍스처
	VertexShader* GetDefaultVertexShader2D();	// VertexPTC용
	PixelShader* GetDefaultPixelShader2D();
	VertexShader* GetDefaultVertexShader3D();	// VertexPNT용
	PixelShader* GetDefaultPixelShader3D();
	Material* GetDefaultMaterial2D();			// 알파 블렌드 + 깊이 꺼짐
	Material* GetDefaultMaterial3D();			// 불투명 + 깊이 읽기쓰기

	_u64 GetDefaultTextureKey() const { return defaultTextureKey_; }
	_u64 GetDefaultVertexShader2DKey() const { return defaultVs2DKey_; }
	_u64 GetDefaultPixelShader2DKey() const { return defaultPs2DKey_; }
	_u64 GetDefaultVertexShader3DKey() const { return defaultVs3DKey_; }
	_u64 GetDefaultPixelShader3DKey() const { return defaultPs3DKey_; }
	_u64 GetDefaultMaterial2DKey() const { return defaultMaterial2DKey_; }
	_u64 GetDefaultMaterial3DKey() const { return defaultMaterial3DKey_; }

	// === 진단 ===

	_s32 GetCount() const { return resources_.Size(); }

	// 보관 중인 리소스 목록을 디버그 출력으로 내보낸다. (누수 진단용)
	void PrintStatus();

private:
	ResourceMgr();
	~ResourceMgr();
	ResourceMgr(const ResourceMgr&) = delete;
	ResourceMgr& operator=(const ResourceMgr&) = delete;

	bool CreateDefaults();
	void RemovePathEntry(_u64 _key);

	GraphicDevice* pDevice_;							// 소유하지 않음
	HashMap<_u64, IResource*> resources_;				// 키 -> 리소스 (소유)
	HashMap<String, _u64> pathIndex_;					// 경로 -> 키 (중복 로드 방지)
	HashSet<_u64> defaultKeys_;							// 제거 금지 키 (FR-30)
	Provider<AutoIncrementLinkedList<_u64>> keyProvider_;	// 키 발급기 (D-12)

	_u64 defaultTextureKey_;
	_u64 defaultVs2DKey_;
	_u64 defaultPs2DKey_;
	_u64 defaultVs3DKey_;
	_u64 defaultPs3DKey_;
	_u64 defaultMaterial2DKey_;
	_u64 defaultMaterial3DKey_;
};

NS_SGF_END

// sgcl 스타일 전역 접근 매크로
#define g_cResourceMgr (sgf::ResourceMgr::GetInstance())
