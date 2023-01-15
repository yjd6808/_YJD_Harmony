/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 4:45:00 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Core/Npk/NpkElement.h>

#include <JCore/Primitives/ArraySegment.h>
#include <JCore/Stream.h>


class NpkElementInitializer
{
public:
	virtual ~NpkElementInitializer() = default;

	virtual void Initialize(const NpkElementPtr& element, JCore::Stream& strean, bool indexOnly) = 0;


	static void Initialize();
	static void Finalize();
	static void InitializeElement(const NpkElementPtr& element, JCore::Stream& strean, bool indexOnly);
};



template <Int32 Version>
class NpkElementInitializerImpl : public NpkElementInitializer
{
public:
	~NpkElementInitializerImpl() override {}

	void Initialize(const NpkElementPtr& element, JCore::Stream& strean, bool indexOnly) override;
};


