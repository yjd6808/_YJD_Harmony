# sgs 프로젝트 폴더 구조 재정비 완료

## 작업일: 2026-04-16
## 결과: 빌드 성공 (경고 0개, 오류 0개)

## 수행한 작업

### 1. 물리 폴더 이름 변경
| 이전 | 이후 |
|------|------|
| Sources/sgs/_API/ | Sources/sgs/API/ |
| Sources/sgs/_Net/ | Sources/sgs/Net/ |

### 2. #include 경로 일괄 수정
- sgs 프로젝트 소스 9개 파일 수정
- sgs_auth, sgs_center, sgs_lobby, sgs_query 프로젝트 11개 파일 수정
- 치환 패턴:
  - `sgs/_API/` → `sgs/API/`
  - `sgs/_Net/` → `sgs/Net/`

### 3. 수정된 파일 목록

**sgs 내부 (9개):**
- Sources/sgs/SessionComponent.h
- Sources/sgs/ServerCore.cpp
- Sources/sgs/Net/NetGroup_Main.cpp
- Sources/sgs/Net/NetCore.cpp
- Sources/sgs/Net/NetGroup_InterServ.cpp
- Sources/sgs/R_SETUP_COMMON.cpp
- Sources/sgs/R_PING_COMMON.cpp
- Sources/sgs/InterServerSendHelper.cpp
- Sources/sgs/AuthenticationComponent.cpp

**sgs_auth (3개):**
- R_AUTHENTICATION.cpp
- main.cpp
- AuthCore.cpp

**sgs_center (2개):**
- CenterCore.cpp
- main.cpp

**sgs_lobby (5개):**
- R_LOBBY.cpp
- UnauthenticatedSessionManager.cpp
- LobbyCore.cpp
- main.cpp
- R_AUTHENTICATION.cpp

**sgs_query (1개):**
- main.cpp

### 4. sgs.vcxproj 파일 경로 수정
- `_API\` → `API\`
- `_Net\` → `Net\`

### 5. sgs.vcxproj.filters 수정
- 필터명: `_API`→`API`, `_Net`→`Net`, `_Net\Recv`→`Net\Recv`, `_Net\Send`→`Net\Send`, `_Net\Cmd`→`Net\Cmd`, `_Net\Database`→`Net\Database`, `_Util`→`Util`
- 파일 경로도 동일하게 업데이트

## 빌드 결과
- Debug x64: **성공** (경고 0개, 오류 0개)
