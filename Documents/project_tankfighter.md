### TankFighter 프로젝트
제가 개발한 JCore 라이브러리와 JNetwork 라이브러리를 활용하여
만든 간단한 예제 게임입니다.

<br>

### TankFighter 프로젝트별 세부 구조

#### <서버 구조>
![서버 구조](Images/TankFighter/ServerStructure.png)

<br>

#### <데이터베이스 구조>
![데이터베이스 구조](Images/TankFighter/DatabaseStructure.png)

<br>

#### <클라이언트 구조>
![클라이언트 구조](Images/TankFighter/ClientStructure.png)

<br>

### 주요 구현 내역
 - 스왑 큐를 활용한 클라이언트 패킷 동기화 [설명 보기](./project_tankfighter-explain-swap-queue.md)
 - 지속적인 확인으로 메모리 누수가 발생하지 않는 게임 개발 [설명 보기]()
 - 쿼리 빌더 구현 [(코드 보기)](../Projects/TankFighter-Server/Sources/TF/Database/MysqlStatementBuilder.h)
 - 비동기 쿼리 처리 구현 [(코드 보기)](../Projects/TankFighter-Server/Sources/TF/Database/MysqlQueryFuture.h)
 - 배틀 중 난입 기능 구현 [(코드 보기)](../Projects/TankFighter-Server/Sources/TF/Worker/BattleFieldWorker.cpp)  
 - 클라이언트 쓰레드 동기화 [(코드 보기)](../Projects/TankFighter-Client/Classes/TF/Network/GameClientEventListener.cpp)

<br>

### [TankFighter 시연 영상 보기](https://youtu.be/2aqGhMcjXNk)
