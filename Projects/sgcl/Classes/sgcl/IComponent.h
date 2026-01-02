/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 3:13:57 AM
 * =====================
 *
 */


#pragma once

#include <jc/Declspec.h>

#include <sgcl/Struct.h>

#define SG_COMPONENT_TYPE_GETTER(component_type)				\
public:															\
	Type GetType() const override { return component_type; }	\
	static Type Type() { return component_type; }

class JC_NOVTABLE IComponent
{
public:
	enum Type
	{
		eMove,
		eAI,
		ePhysics,
		eAuthentication
	};

	virtual ~IComponent() = default;
	virtual void Initialize() = 0;
	virtual Type GetType() const = 0;
};
