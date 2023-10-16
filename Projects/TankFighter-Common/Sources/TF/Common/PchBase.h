/*
 * 작성자: 윤정도
 * 생성일: 10/9/2023 7:10:55 PM
 * =====================
 *
 */


#pragma once


#include <JCore/Assert.h>
#include <JCore/TypeTraits.h>
#include <JCore/Define.h>
#include <JCore/Functional.h>
#include <JCore/Math.h>
#include <JCore/Time.h>
#include <JCore/Stream.h>
#include <JCore/Env.h>
#include <JCore/Random.h>
#include <JCore/Bit.h>

#include <JCore/Logger/LoggerDefine.h>

#include <JCore/Pool/BinarySearchMemoryPool.h>
#include <JCore/Pool/IndexedMemoryPool.h>
#include <JCore/Pool/ObjectPool.h>

#include <JCore/Pattern/Singleton.h>

#include <JCore/Primitives/StaticString.h>
#include <JCore/Primitives/StringUtil.h>
#include <JCore/Primitives/String.h>
#include <JCore/Primitives/BitFlag.h>

#include <JCore/Utils/Concatenation.h>
#include <JCore/Utils/Console.h>

#include <JCore/Threading/Thread.h>
#include <JCore/Threading/Pulser.h>

#include <JCore/FileSystem/File.h>
#include <JCore/FileSystem/Path.h>
#include <JCore/FileSystem/FileStream.h>
#include <JCore/FileSystem/Directory.h>

#include <JCore/Container/Vector.h>
#include <JCore/Container/ArrayStack.h>
#include <JCore/Container/ArrayQueue.h>
#include <JCore/Container/Arrays.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Container/HashSet.h>
#include <JCore/Container/TreeMap.h>
#include <JCore/Container/TreeSet.h>
#include <JCore/Container/LinkedList.h>

#include <JNetwork/Network.h>
#include <JNetwork/NetMaster.h>
#include <JNetwork/IPEndPoint.h>
#include <JNetwork/Socket.h>
#include <JNetwork/Winsock.h>
#include <JNetwork/NetLoggerDefine.h>

#include <JNetwork/Host/TcpServer.h>
#include <JNetwork/Host/TcpClient.h>
#include <JNetwork/Host/UdpClient.h>

#include <JNetwork/IOCPOverlapped/IOCPOverlappedRecv.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedRecvFrom.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSend.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSendTo.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedTask.h>

#include <JNetwork/IOCP/IOCPTask.h>
#include <JNetwork/Packet/CommandNameDictionary.h>
#include <JNetwork/Packet/CommandParser.h>

#include <TF/Common/Base/ComponentCollection.h>
#include <TF/Common/Base/SessionComponent.h>


namespace Core
{
	inline JNetwork::CommandNameDictionary CommandNameMap;
}

void InitializeCommon();
void FinalizeCommon();