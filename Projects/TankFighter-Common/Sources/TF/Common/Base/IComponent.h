/*
 * 작성자: 윤정도
 * 생성일: 8/21/2023 9:21:46 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Declspec.h>
#include <JCore/Primitives/RefCountObject.h>

#define TF_COMPONENT_TYPE_GETTER(component_type)				\
public:															\
	int GetType() const override { return component_type; }		\
	static int Type() { return component_type; }

struct JCORE_NOVTABLE IComponent : JCore::RefCountObject
{
	~IComponent() override = default;

	virtual void Initialize() = 0;
	virtual int GetType() const = 0;
	void ReleaseAction() override { delete this; }
};


