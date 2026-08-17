/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (게임 오브젝트 시스템)
 * =====================
 * 컴포넌트 베이스
 *
 * [컴포넌트란?]
 * GameObject에 붙는 "부품". Transform(위치)과 Material(그림 방식)은
 * GameObject가 기본 멤버로 직접 소유하고(O(1) 접근), 그 외의 확장 부품은
 * 이 베이스를 상속받아 components_ 리스트에 넣는다.
 *
 * [타입 대역]
 * ctTransform=0 / ctMaterial=1 은 GameObject 기본 멤버로 예약.
 * 2~99: 엔진 예약 (신규 내장 컴포넌트용)
 * 100+: 커스텀 (사용자 정의 컴포넌트)
 * (ctCamera2D/ctCamera3D 제거 — 카메라는 컴포넌트가 아니라 GameObject 파생)
 */

#pragma once

#include "jc/Type.h"

NS_SGF_BEGIN

enum class ComponentType : _u16
{
	ctTransform = 0,	// GameObject 기본 멤버 (O(1) 접근)
	ctMaterial = 1,		// GameObject 기본 멤버 (O(1) 접근)
	ctCustom = 100,		// 커스텀 시작 지점 (100+)
};

// 파생 클래스에서 컴포넌트 타입을 선언하는 도우미 매크로
// class MyComponent: public sgf::Component { SGF_COMPONENT_TYPE(ctCustom) ... };
#define SGF_COMPONENT_TYPE(_Type) \
public: \
	static constexpr sgf::ComponentType COMPONENT_TYPE = sgf::ComponentType::_Type; \
	virtual sgf::ComponentType GetComponentType() const override { return COMPONENT_TYPE; }

class Component
{
public:
	Component() = default;
	virtual ~Component() = default;

	// 이 컴포넌트의 종류. (GetComponent<T>()의 매칭에 사용)
	virtual ComponentType GetComponentType() const = 0;
};

NS_SGF_END