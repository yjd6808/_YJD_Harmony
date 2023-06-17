/*
 * 작성자: 윤정도
 * 생성일: 6/15/2023 6:09:06 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Namespace.h>

#define SG_CACHELINE_SIZE		64
#define SG_FLT_EPSILON			0.0001f
#define SG_PI					3.141592f

 // ============================================================
 //     JCore
 // ============================================================

using SGString = JCore::String;

template <Int32U Size>
using SGStaticString = JCore::StaticString<Size>;
using SGPath = JCore::Path;
using SGFile = JCore::File;
using SGDirectory = JCore::Directory;
using SGStringUtil = JCore::StringUtil;

using SGMemory = JCore::Memory;
using SGArrays = JCore::Arrays;

template <typename TKey, typename TValue, typename TAllocator = JCore::DefaultAllocator>
using SGHashMap = JCore::HashMap<TKey, TValue, TAllocator>;

template <typename T, typename TAllocator = JCore::DefaultAllocator>
using SGVector = JCore::Vector<T, TAllocator>;

template <typename T, typename TAllocator = JCore::DefaultAllocator>
using SGLinkedList = JCore::LinkedList<T, TAllocator>;

template <typename T, typename TAllocator = JCore::DefaultAllocator>
using SGArrayQueue = JCore::ArrayQueue<T, TAllocator>;

using SGMath = JCore::Math;

template <typename... Args>
using SGActionFn = JCore::Action<Args...>;

template <typename R, typename... Args>
using SGFuncFn = JCore::Func<R, Args...>;

template <typename... Args>
using SGPredicateFn = JCore::Predicate<Args...>;

using SGThread = JCore::Thread;

using SGStream = JCore::Stream;
using SGStreamPtr = JCore::StreamPtr;
using SGFileStream = JCore::FileStream;
using SGFileStreamPtr = JCore::FileStreamPtr;

using SGConsoleColor = JCore::ConsoleColor;
using SGDateTime = JCore::DateTime;
using SGTimeSpan = JCore::TimeSpan;

using SGNormalLock = JCore::NormalLock;
using SGRecursiveLock = JCore::RecursiveLock;

template <typename... Args>
using SGEventList = JCore::Event<Args...>;

template <typename... TArgs>
using SGTuple = JCore::Tuple<TArgs...>;

using SGIndexMemroyPool = JCore::IndexedMemoryPool;
using SGBinarySearchMemroyPool = JCore::BinarySearchMemoryPool;

template <typename T>
using SGObjectPool = JCore::ObjectPool<T>;

template <typename T>
using SGSingletonStatic = JCore::SingletonStatic<T>;
template <typename T>
using SGSingletonPointer = JCore::SingletonPointer<T>;


// ============================================================
 //     JNework
 // ============================================================

using SGSocket = JNetwork::Socketv4;
using SGEndPoint = JNetwork::IPv4EndPoint;
using SGAddress = JNetwork::IPv4Address;
using SGSession = JNetwork::Session;
using SGTcpClient = JNetwork::TcpClient;
using SGUdpClient = JNetwork::UdpClient;
using SGTcpSession = JNetwork::TcpSession;
using SGTcpServer = JNetwork::TcpServer;

using SGTcpClientPtr = JNetwork::TcpClientPtr;
using SGUdpClientPtr = JNetwork::UdpClientPtr;
using SGTcpSessionPtr = JNetwork::TcpSessionPtr;
using SGTcpServerPtr = JNetwork::TcpServerPtr;

using SGTcpClientWPtr = JNetwork::TcpClientPtr;
using SGUdpClientWPtr = JNetwork::UdpClientPtr;
using SGTcpSessionWPtr = JNetwork::TcpSessionPtr;
using SGTcpServerWPtr = JNetwork::TcpServerPtr;

using SGNetGroup = JNetwork::NetGroup;
using SGNetMaster = JNetwork::NetMaster;
using SGIOCP = JNetwork::IOCP;

using SGOverlapped = JNetwork::IOCPOverlapped;
using SGISendPacket = JNetwork::ISendPacket;
using SGCommandBuffer = JNetwork::CommandBuffer;
using SGCommandBufferPacket = JNetwork::CommandBufferPacket;

using SGOverlappedPtr = JNetwork::IOCPOverlappedPtr;
using SGISendPacketPtr = JNetwork::ISendPacketPtr;
using SGCommandBufferPtr = JNetwork::CommandBufferPtr;
using SGCommandParser = JNetwork::CommandParser;


// ============================================================
//       컨테이너
// ============================================================

template <typename T, typename TAllocator = JCore::DefaultAllocator>
class alignas(SG_CACHELINE_SIZE) SGAlignedVector : public SGVector<T, TAllocator> {};

template <typename T, typename TAllocator = JCore::DefaultAllocator>
class alignas(SG_CACHELINE_SIZE) SGAlignedLinkedList : public SGLinkedList<T, TAllocator> {};

template <typename TKey, typename TValue, typename TAllocator = JCore::DefaultAllocator>
class alignas(SG_CACHELINE_SIZE) SGAlignedHashMap : public SGHashMap<TKey, TValue, TAllocator> {};



