/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 10:47:22 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Define.h>

#include <TF/Common/Namespace.h>

NS_JC_BEGIN


	class DefaultAllocator;

	JCORE_FORWARD_TEMPLATE(1) class SharedPtr;
	JCORE_FORWARD_TEMPLATE(2) class ArrayQueue;
NS_JC_END

NS_JNET_BEGIN
	struct ICommand;
NS_JNET_END


using CommandQueue = JCore::ArrayQueue<JNetwork::ICommand*, JCore::DefaultAllocator>;