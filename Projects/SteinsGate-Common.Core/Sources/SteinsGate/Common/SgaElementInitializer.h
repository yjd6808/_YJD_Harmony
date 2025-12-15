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

	virtual void Initialize(const SgaElementPtr& _pElement, JCore::Stream& _stream, bool _indexOnly) = 0;

	static void Initialize();
	static void Finalize();
	static void InitializeElement(const SgaElementPtr& _pElement, JCore::Stream& _stream, bool _indexOnly);
};


template <Int32 Version>
class SgaElementInitializerImpl : public SgaElementInitializer
{
public:
	~SgaElementInitializerImpl() override
	{
	}

	void Initialize(const SgaElementPtr& _pElement, JCore::Stream& _stream, bool _indexOnly) override;
};
