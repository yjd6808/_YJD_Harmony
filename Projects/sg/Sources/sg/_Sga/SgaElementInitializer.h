/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 4:45:00 PM
 * =====================
 *
 */


#pragma once

#include <sg/_Sga/SgaElement.h>

#include <jc/Primitives/ArraySegment.h>
#include <jc/Stream.h>


class SgaElementInitializer
{
public:
	virtual ~SgaElementInitializer() = default;

	virtual void Initialize(const SgaElementPtr& _pElement, jc::Stream& _stream, bool _indexOnly) = 0;

	static void Initialize();
	static void Finalize();
	static void InitializeElement(const SgaElementPtr& _pElement, jc::Stream& _stream, bool _indexOnly);
};


template <_s32 Version>
class SgaElementInitializerImpl : public SgaElementInitializer
{
public:
	~SgaElementInitializerImpl() override
	{
	}

	void Initialize(const SgaElementPtr& _pElement, jc::Stream& _stream, bool _indexOnly) override;
};
