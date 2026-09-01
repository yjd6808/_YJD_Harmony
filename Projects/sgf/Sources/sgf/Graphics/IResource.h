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
enum class ResourceType : _u8
{
	rtUnknown = 0,
	rtTexture = 1,
	rtMaterial = 2,
	rtMesh = 3,
	rtVertexShader = 4,
	rtPixelShader = 5,
	rtRenderTarget = 6,
	rtSpriteAnimationClip = 7,
	rtVertexBuffer = 8,
	rtIndexBuffer = 9,
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
	"RenderTarget",
	"SpriteAnimationClip",
	"VertexBuffer",
	"IndexBuffer",
};

// 유효하지 않은 리소스 키 (jc::IIdProvider의 INVALID_KEY와 같은 값)
constexpr _u64 INVALID_RESOURCE_KEY = 0;

// ── C안 키 레이아웃: [type 8][gen 24][index 32] ──────────────────────────
// 63..56  type (1~255, 0=무효)
// 55..32  gen  (1~0xFFFFFF, 0=무효 — 슬롯 재사용 시 +1, 랩 시 1로 순환)
// 31..0   index (0~0xFFFFFFFF, 타입별 슬롯 배열 인덱스)
constexpr _u64 RESOURCE_KEY_TYPE_SHIFT = 56;
constexpr _u64 RESOURCE_KEY_GEN_SHIFT  = 32;
constexpr _u64 RESOURCE_KEY_TYPE_MASK  = 0xFFULL << RESOURCE_KEY_TYPE_SHIFT;
constexpr _u64 RESOURCE_KEY_GEN_MASK   = 0xFFFFFFULL << RESOURCE_KEY_GEN_SHIFT;
constexpr _u64 RESOURCE_KEY_INDEX_MASK = 0xFFFFFFFFULL;
constexpr _u32 RESOURCE_KEY_GEN_MAX    = 0xFFFFFF; // 24비트 최대
constexpr _u32 RESOURCE_KEY_INDEX_MAX  = 0xFFFFFFFF;

inline constexpr _u64 MakeResourceKey(ResourceType _type, _u32 _gen, _u32 _index)
{
	return ( (static_cast<_u64>(_type) << RESOURCE_KEY_TYPE_SHIFT) |
			 (static_cast<_u64>(_gen & RESOURCE_KEY_GEN_MAX) << RESOURCE_KEY_GEN_SHIFT) |
			 (static_cast<_u64>(_index)) );
}
inline constexpr ResourceType GetResourceTypeFromKey(_u64 _key) { return static_cast<ResourceType>((_key >> RESOURCE_KEY_TYPE_SHIFT) & 0xFFULL); }
inline constexpr _u32 GetResourceGenFromKey(_u64 _key)   { return static_cast<_u32>((_key >> RESOURCE_KEY_GEN_SHIFT) & 0xFFFFFFULL); }
inline constexpr _u32 GetResourceIndexFromKey(_u64 _key) { return static_cast<_u32>(_key & RESOURCE_KEY_INDEX_MASK); }
inline constexpr bool IsValidResourceKey(_u64 _key) { return _key != INVALID_RESOURCE_KEY && GetResourceTypeFromKey(_key) != ResourceType::rtUnknown && GetResourceGenFromKey(_key) != 0; }

// 파생 클래스에서 리소스 종류를 선언하는 도우미 매크로
// class Texture: public ResourceBase { SGF_RESOURCE_TYPE(rtTexture) ... };
#define SGF_RESOURCE_TYPE(typeName) \
public: \
	static constexpr ::sgf::ResourceType RESOURCE_TYPE = ::sgf::ResourceType::typeName; \
	virtual ::sgf::ResourceType GetResourceType() const override { return ::sgf::ResourceType::typeName; }

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

	virtual _u64 GetKey() const override { return key_; }
	virtual const char* GetDebugName() const override { return debugName_.SafeSource(); }

	// ResourceMgr만 호출한다. (Add/Remove 시 키 부여/회수)
	void SetKey(_u64 _key) { key_ = _key; }

	// 디버그 이름을 붙여둔다. (PrintStatus/로그에서 식별용)
	void SetDebugName(StringView _name)
	{
		debugName_ = _name;
	}

	// 경로로 등록된 리소스의 로드 경로. 경로 없이 Add된 리소스는 빈 문자열. (B-7)
	const char* GetPath() const { return path_.SafeSource(); }

	// ResourceMgr만 호출한다. (경로와 함께 Add할 때 보관 — 역방향 인덱스 불필요)
	void SetPath(StringView _path) { path_ = _path; }

protected:
	_u64 key_;			// ResourceMgr 발급 키
	String debugName_;	// 디버그 이름
	String path_;		// 등록 경로 (ResourceMgr::Add(resource, path)로 세팅)
};

NS_SGF_END
