/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 5:50:00 PM
 * =====================
 * 리소스 공통 인터페이스 (IResource) + 공통 구현 (ResourceBase)
 *
 * [리소스란?]
 * 텍스처/머티리얼/메시/셰이더처럼 "만드는 비용이 비싸고, 여러 곳에서
 * 공유해 쓰는" GPU 관련 객체. ResourceMgr에 등록해 키(_u64)로 찾아 쓴다.
 *
 * [설계 원칙: 소유자 = 수명 결정자]
 * ResourceMgr에 Add된 리소스는 ResourceMgr가 소유하고 Remove/Finalize에서
 * delete한다. Add하지 않은 리소스는 만든 쪽이 직접 책임진다.
 */

#pragma once

#include "jc/Type.h"
#include "jc/Primitives/String.h"
#include "jc/Primitives/StringView.h"
#include "sgf/Namespace.h"

NS_SGF_BEGIN

using namespace jc;

// 리소스 종류. 값 이름은 rt(ResourceType) 접두어 규칙을 따른다.
enum class ResourceType
{
	rtUnknown,
	rtTexture,				// Texture
	rtMaterial,				// Material
	rtMesh,					// Mesh
	rtVertexShader,			// VertexShader
	rtPixelShader,			// PixelShader
	rtInputLayout,			// InputLayout
	rtRenderTarget,			// RenderTarget
	rtSpriteAnimationClip,	// 스프라이트 애니메이션 클립 (30번 튜토리얼)
	Max,
};

// 리소스 종류 이름 테이블 (디버깅/로그용. 인덱스 = enum 값)
constexpr const char* RESOURCE_TYPE_NAMES[static_cast<_s32>(ResourceType::Max)] =
{
	"Unknown",
	"Texture",
	"Material",
	"Mesh",
	"VertexShader",
	"PixelShader",
	"InputLayout",
	"RenderTarget",
	"SpriteAnimationClip",
};

// 유효하지 않은 리소스 키 (jc::IIdProvider의 INVALID_KEY와 같은 값)
constexpr _u64 INVALID_RESOURCE_KEY = 0;

// 파생 클래스에서 리소스 종류를 선언하는 도우미 매크로
// class Texture: public ResourceBase { SGF_RESOURCE_TYPE(rtTexture) ... };
#define SGF_RESOURCE_TYPE(typeName) \
public: \
	static constexpr ::sgf::ResourceType RESOURCE_TYPE = ::sgf::ResourceType::typeName; \
	::sgf::ResourceType GetResourceType() const override { return ::sgf::ResourceType::typeName; }

// 모든 리소스의 공통 인터페이스
class IResource
{
public:
	virtual ~IResource() {}

	// 리소스 종류 (SGF_RESOURCE_TYPE 매크로가 구현해준다)
	virtual ResourceType GetResourceType() const = 0;

	// ResourceMgr가 발급한 키 (등록 전이면 INVALID_RESOURCE_KEY)
	virtual _u64 GetKey() const = 0;

	// 디버그 이름 (없으면 빈 문자열)
	virtual const char* GetDebugName() const = 0;
};

// 키/디버그 이름 보관을 구현해둔 공통 베이스.
// 새 리소스는 보통 이 클래스를 상속받으면 된다.
class ResourceBase : public IResource
{
public:
	ResourceBase()
		: key_(INVALID_RESOURCE_KEY)
	{
	}

	_u64 GetKey() const override { return key_; }
	const char* GetDebugName() const override { return debugName_.SafeSource(); }

	// ResourceMgr만 호출한다. (Add/Remove 시 키 부여/회수)
	void SetKey(_u64 _key) { key_ = _key; }

	// 디버그 이름을 붙여둔다. (PrintStatus/로그에서 식별용)
	void SetDebugName(StringView _name)
	{
		debugName_ = _name;
	}

protected:
	_u64 key_;			// ResourceMgr 발급 키
	String debugName_;	// 디버그 이름
};

NS_SGF_END
