/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 3:13:57 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Declspec.h>

#define SG_COMPONENT_TYPE_GETTER(component_type)				\
public:															\
	Type getType() const override { return component_type; }	\
	static Type type() { return component_type; }

class JCORE_NOVTABLE IComponent
{
public:
	enum Type
	{
		eMove,
		eAI,
		ePhysics
	};

	virtual ~IComponent() = default;
	virtual void initialize() = 0;
	virtual Type getType() const = 0;
};

