// Networks

#include <JNetwork/IPEndPoint.h>
#include <JNetwork/Host/TcpClient.h>
#include <JNetwork/Packet.h>

#pragma warning (push, 0)
#include <cocos2d.h>
#pragma warning(pop)

#include <TF/Network/GameClient.h>
#include <TF/Network/CommandParser.h>
#include <TF/Network/CommandFn.h>
#include <TF/Network/GameClientEventListener.h>
#include <TF/Network/SendFn.h>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#pragma execution_character_set("utf-8")
#endif

#include <TF/Network/SendFn.cpp>
#include <TF/Network/GameClient.cpp>
#include <TF/Network/CommandParser.cpp>
#include <TF/Network/CommandFn.cpp>
#include <TF/Network/GameClientEventListener.cpp>
