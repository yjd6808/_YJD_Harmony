/*
 * 작성자: 윤정도
 * 생성일: 1/9/2023 12:04:30 AM
 * =====================
 *
 */


#pragma once

#include "jc/Assert.h"
#include "jc/TypeTraits.h"
#include "jc/Define.h"
#include "jc/Functional.h"
#include "jc/Math.h"
#include "jc/Macro.h"
#include "jc/Time.h"
#include "jc/Stream.h"
#include "jc/Env.h"
#include "jc/Event.h"
#include "jc/Random.h"
#include "jc/Bit.h"
#include "jc/Functional.h"

#include "jc/Pool/BinarySearchMemoryPool.h"
#include "jc/Pool/IndexedMemoryPool.h"
#include "jc/Pool/ObjectPool.h"

#include "jc/Pattern/Singleton.h"
#include "jc/Pattern/NonCopyableh.h"

#include "jc/Primitives/StaticString.h"
#include "jc/Primitives/StringUtil.h"
#include "jc/Primitives/String.h"
#include "jc/Primitives/StringTokenizer.h"
#include "jc/Primitives/BitFlag.h"

#include "jc/Utils/Concatenation.h"
#include "jc/Utils/Console.h"

#include "jc/Threading/Thread.h"
#include "jc/Threading/Scheduler.h"
#include "jc/Threading/Pulser.h"

#include "jc/IO/File.h"
#include "jc/IO/Path.h"
#include "jc/IO/FileStream.h"
#include "jc/IO/Directory.h"

#include "jc/Container/Vector.h"
#include "jc/Container/ArrayStack.h"
#include "jc/Container/ArrayQueue.h"
#include "jc/Container/Arrays.h"
#include "jc/Container/HashMap.h"
#include "jc/Container/HashSet.h"
#include "jc/Container/TreeMap.h"
#include "jc/Container/TreeSet.h"
#include "jc/Container/LinkedList.h"
#include "jc/Container/Properties.h"

#include "jnet/PCH/Core.h"
#include "jnet/NetGroupMgr.h"
#include "jnet/IPEndPoint.h"
#include "jnet/Socket.h"
#include "jnet/Winsock.h"

#include "jnet/Host/TcpServer.h"
#include "jnet/Host/TcpClient.h"
#include "jnet/Host/UdpClient.h"

#include "jnet/IOCPOverlapped/IOCPOverlappedRecv.h"
#include "jnet/IOCPOverlapped/IOCPOverlappedRecvFrom.h"
#include "jnet/IOCPOverlapped/IOCPOverlappedSend.h"
#include "jnet/IOCPOverlapped/IOCPOverlappedSendTo.h"
#include "jnet/IOCPOverlapped/IOCPOverlappedTask.h"

#include "jnet/IOCP/IOCPTask.h"
#include "jnet/Packet/CommandParser.h"
#include "jnet/Packet/CommandNameDictionary.h"

#include <json.h>
#include <fstream>
#include <set>
#include <map>

#include "sgcc/PCH/Core.h"

#include "sg/Namespace.h"
#include "sg/Const/Const.h"
#include "sg/DLLExport.h"
#include "sg/Logo.h"
#include "sg/CommonCore.h"
#include "sg/Util/JsonUtil.h"

#include "sg/Core/VirtualMachine.h"

#define SG_CACHELINE_SIZE        64
#define SG_FLT_EPSILON           0.0001f
#define SG_PI                    3.141592f

// 로컬라이즈
#define SG_LOCAL(text)				(text)			// SG_LT로 변환됨
#define SG_LOCAL_RAW(text)			(text)			// SG_LT_RAW로 변환됨

#define SG_LT(textKey)				(g_cDescMgr.GetText(textKey))
#define SG_LT_RAW(textKey)			(g_cDescMgr.GetTextRaw(textKey))