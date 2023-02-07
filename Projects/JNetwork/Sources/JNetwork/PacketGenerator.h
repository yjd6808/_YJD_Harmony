/*
 * 작성자: 윤정도
 * 생성일: 12/12/2022 8:49:05 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Packet.h>

NS_JNET_BEGIN

	template <typename TPool>
	class PacketGenerator
	{
	public:
		template <typename... CommandArgs, typename... Ints>
		static decltype(auto) CreateStatic(Ints... sizes){
			return new StaticPacket<CommandArgs...>(sizes...);
		}
	};

NS_JNET_END

