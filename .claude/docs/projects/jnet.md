# jnet (JNetwork)

## 개요
**Windows IOCP 기반 고성능 TCP/UDP 네트워킹 라이브러리** (Static Library).  
비동기 이벤트 기반, 게임 서버 등 고성능 환경을 대상으로 설계.  
jc 라이브러리에 강하게 의존.

## 소스 구조

```
Sources/jnet/
├── IOCP/              - IOCP 엔진 (IOCP.h, IOCPWorker.h, IOCPPostOrder.h)
├── IOCPOverlapped/    - Overlapped I/O (Accept, Connect, Recv, Send, RecvFrom, SendTo, Disconnect)
├── Host/              - 네트워크 호스트 (TcpServer, TcpClient, TcpSession, UdpClient, SessionContainer)
├── Packet/            - 패킷/명령어 (Packet.h, Command.h, PacketHeader.h, PacketParser.h)
├── Buffer/            - 버퍼 관리 (StreamBuffer, PooledStreamBuffer, PacketBuffer)
├── EventListener/     - 이벤트 콜백 인터페이스
├── Socket.h           - Winsock2 래퍼
├── IPAddress.h        - IPv4/IPv6 주소
├── IPEndPoint.h       - 주소:포트 쌍
├── Protocol.h         - TCP/UDP, IPv4/IPv6 열거형
├── Config.h           - 전역 설정 (버퍼 크기, 논블로킹 모드)
├── NetGroup.h         - 다중 IOCP 그룹 관리
└── Winsock.h          - Winsock 초기화/종료
```

## 패킷 헤더 구조 (4바이트)
```
Magic(2) = 0xAFD3 | PayloadLen(2) | PacketType(1) | ElementCount(1)
```

## 패킷/명령어 타입

| 타입 | 설명 |
|------|------|
| `RawPacket` | 원본 바이트 데이터 |
| `StaticCmdPacket<Cmds...>` | 고정 크기 명령어 묶음 |
| `DynamicCmdPacket<Cmds...>` | 가변 크기 명령어 묶음 |
| `SingleCmdPacket<Cmd>` | 단일 명령어 |
| `StaticCommand` | 컴파일타임 크기 결정 |
| `DynamicCommand` | 런타임 크기 결정 (배열 필드) |

## 이벤트 리스너

```cpp
// 세션 이벤트
OnConnected(), OnDisconnected(), OnConnectFailed()
OnReceivedCommand(), OnReceivedPacket(), OnSent()

// 서버 이벤트
OnStarted(), OnStartFailed(), OnStopped()
```

## 핵심 클래스

| 클래스 | 파일 | 역할 |
|--------|------|------|
| `IOCP` | `IOCP/IOCP.h` | IOCP 매니저, 워커 스레드 풀 |
| `IOCPWorker` | `IOCP/IOCPWorker.h` | IOCP 워커 스레드 |
| `TcpServer` | `Host/TcpServer.h` | TCP 서버 |
| `TcpClient` | `Host/TcpClient.h` | TCP 클라이언트 |
| `TcpSession` | `Host/TcpSession.h` | TCP 인바운드 세션 |
| `UdpClient` | `Host/UdpClient.h` | UDP 클라이언트 |
| `PacketBuffer` | `Buffer/PacketBuffer.h` | 패킷 단위 버퍼 |
| `StreamBuffer` | `Buffer/StreamBuffer.h` | 스트림 버퍼 |

## 의존성
- `jc` (SharedPtr, HashMap, NormalLock, SpinLock, RefCountObject, MemoryPool 등)
- Winsock2 (Windows 네트워크 API)

## 빌드 산출물
- `jnet.lib` (Static Library)
