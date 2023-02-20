/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 4:45:00 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/SgaElement.h>

#include <JCore/Primitives/ArraySegment.h>
#include <JCore/Stream.h>


class SgaElementInitializer
{
public:
	virtual ~SgaElementInitializer() = default;

	virtual void Initialize(const SgaElementPtr& element, JCore::Stream& strean, bool indexOnly) = 0;


	static void Initialize();
	static void Finalize();
	static void InitializeElement(const SgaElementPtr& element, JCore::Stream& strean, bool indexOnly);
};



template <Int32 Version>
class SgaElementInitializerImpl : public SgaElementInitializer
{
public:
	~SgaElementInitializerImpl() override {}

	void Initialize(const SgaElementPtr& element, JCore::Stream& strean, bool indexOnly) override;
};


