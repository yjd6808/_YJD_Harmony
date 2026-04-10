
레이어드 아키텍처 구조로, 아래로 갈수록 의존성 없는 기반 계층이다.

```
Layer 4: Game Applications
  sgcl — SteinsGate 게임 클라이언트

Layer 4: Game Server Services
  sgs - SteinsGate 서버 공통, 
  sgs_auth - 인증 서버, 
  sgs_center - 중앙 서버(서버간 통신)
  sgs_game - 게임 서버
  sgs_lobby - 로비 서버
  sgs_query - 쿼리 서버(jdb 라이브러리 사용)

Layer 3: Game Common
  sg (서버/클라 공통)

Layer 2: JNetwork (IOCP 기반 TCP/UDP 소켓 라이브러리)
  jnet — 코어 / jnet_gtest, jnet_gbench — 테스트/벤치마크

Layer 1: JCore (모든 프로젝트의 기반 C++ 유틸리티 라이브러리)
  jc — 코어 / jc_gtest, jc_gbench — 테스트/벤치마크

Database: jdb (SQLite 추상화)
Research: jcr, jdbr, jnetr_* (실험/연구용 프로젝트)

Tool: sgcc (공용 수학/타입), sgt (도구 공통), sgtc (도구 클라), sgts (도구 서버)
```
각 프로젝트 디렉토리 구성: `Sources/` (소스코드), `Project/` (vcxproj), `Intermediate/`, `Output/`

## 프로젝트별 상세 문서

### Core Libraries
- [jc](projects/jc.md) — JCore, 모든 프로젝트의 기반 C++ 유틸리티 라이브러리
- [jcr](projects/jcr.md) — JCore Research, 메모리/힙 연구 프로젝트
- [jnet](projects/jnet.md) — JNetwork, IOCP 기반 TCP/UDP 소켓 라이브러리
- [jdb](projects/jdb.md) — JDatabase, MySQL/SQL Server 추상화 라이브러리
- [jdbr](projects/jdbr.md) — JDatabase Research, jdb 테스트/데모

### Game Common
- [sg](projects/sg.md) — SteinsGate 서버/클라 공통 라이브러리
- [sgs](projects/sgs.md) — SteinsGate 서버 공통 (컴포넌트 아키텍처, 서버간 통신)

### Game Servers
- [sgs_center](projects/sgs_center.md) — 중앙 서버 (서버간 라우팅 허브)
- [sgs_auth](projects/sgs_auth.md) — 인증 서버 (로그인/토큰 발급)
- [sgs_lobby](projects/sgs_lobby.md) — 로비 서버 (캐릭터 관리, 입장)
- [sgs_game](projects/sgs_game.md) — 게임 서버 (미구현)
- [sgs_query](projects/sgs_query.md) — 쿼리 서버 (상태 조회)

### Game Client
- [sgcl](projects/sgcl.md) — SteinsGate 게임 클라이언트 (Cocos2d-x)

### Tool Projects
- [sgcc](projects/sgcc.md) — cocos2d-x의 기본타입을 sg 프로젝트에 연동하기 위한 브릿지 프로젝트
- [sge](projects/sge.md) — (미사용)
- [sgt](projects/sgt.md) — 도구 공통 라이브러리
- [sgtc](projects/sgtc.md) — 도구 클라이언트 (Cocos2d-x)
- [sgts](projects/sgts.md) — 도구 서버 (미구현)
