/*
 * 작성자: 윤정도
 * 생성일: 2026-08-23 (C안)
 * =====================
 * 리소스 레지스트리 인터페이스 — GraphicDevice가 ResourceMgr를 알지 못하게 하는 추상화
 */

#pragma once

#include "jc/Type.h"

NS_SGF_BEGIN

class IResource;

struct IResourceRegistry
{
	virtual ~IResourceRegistry() = default;

	// 소유권 이전 + 키 발급 (실패 시 INVALID_RESOURCE_KEY)
	virtual _u64 Register(IResource* _pResource) = 0;

	// 키로 조회 (세대 불일치/타입 불일치/미존재 시 nullptr)
	virtual IResource* Resolve(_u64 _key) = 0;

	// 키로 제거 (디폴트/세대불일치 시 false)
	virtual bool Unregister(_u64 _key) = 0;
};

NS_SGF_END
