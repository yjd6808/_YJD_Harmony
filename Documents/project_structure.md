### 프로젝트  종류

![솔루션 구성](Images/프로젝트%20모습.png)

※ ThirdParty의 프로젝트들은 외부 라이브러리 프로젝트들을 포함하고 있습니다.
<br>

1. <b>JCore</b> : 모든 프로젝트의 가장 기본이되는 개인용 C++ 라이브러리입니다.
2. <b>JCore-Benchmark</b> : JCore 라이브러리와 표준 C++ 라이브러리와 성능 비교를 위해 만든 벤치마크 프로젝트입니다.
3. <b>JCore-Test</b> : JCore 라이브러리를 테스트하기 위한 구글 테스트 프로젝트입니다.
4. <b>JNetwork</b> : JCore 라이브러리를 바탕으로 제작해본 개인용 네트워크 라이브러리입니다.
5. <b>JNetwork-Resarch.X</b> : JNetwork 라이브러리를 활용하여 테스트 네트워크 통신을 하기 위해 만든 프로젝트입니다.
6. <b>JNetwork-Test</b> : JNetwork 라이브러리를 테스트하기 위한 구글 테스트 프로젝트입니다.
7. <b>TankFighter-Client</b> : JNetwork와 JCore, Cocos2d-x 게임엔진을 기반으로 개발한 게임 클라이언트입니다.
8. <b>TankFighter-Common</b> : TankFighter-Client, Server의 공통 구조체, enum등을 정의한 라이브러리입니다.
9. <b>TankFighter-Server</b> : JNetwork와 JCore를 활용하여 개발한 서버 프로젝트입니다.

<br>

### 프로젝트 디렉토리 구성

1. Intermediate : 중간 파일들 출력되는 폴더
2. Output : 결과 파일 출력되는 폴더
3. Sources : 소스 파일들이 저장되어있는 폴더
4. Scripts : 프로젝트 관련 스크립트
6. Project : 프로젝트 파일들