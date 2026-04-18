# sgs 프로젝트 폴더 구조 재정비 계획

## 작업일: 2026-04-16
## 참고: .admin/histories/2026-04-16-sg-folder-restructure.md (sg 프로젝트 동일 작업)

## 목표
sg와 동일하게 `_` 접두사 폴더 이름 제거 및 관련 경로 수정

## 수행할 작업

### 1. 물리 폴더 이름 변경
| 이전 | 이후 |
|------|------|
| Sources/sgs/_API/ | Sources/sgs/API/ |
| Sources/sgs/_Net/ | Sources/sgs/Net/ |

### 2. #include 경로 수정
- `sgs/_API/` → `sgs/API/`
- `sgs/_Net/` → `sgs/Net/`

### 3. 수정 대상 파일

**sgs 프로젝트 소스 (9개):**
- Sources/sgs/SessionComponent.h
- Sources/sgs/ServerCore.cpp
- Sources/sgs/_Net/NetGroup_Main.cpp
- Sources/sgs/_Net/NetCore.cpp
- Sources/sgs/_Net/NetGroup_InterServ.cpp
- Sources/sgs/R_SETUP_COMMON.cpp
- Sources/sgs/R_PING_COMMON.cpp
- Sources/sgs/InterServerSendHelper.cpp
- Sources/sgs/AuthenticationComponent.cpp

**sgs 프로젝트 파일 (2개):**
- Project/sgs.vcxproj
- Project/sgs.vcxproj.filters

**sgs_auth 프로젝트 (3개):**
- Sources/sgs_auth/R_AUTHENTICATION.cpp
- Sources/sgs_auth/main.cpp
- Sources/sgs_auth/AuthCore.cpp

**sgs_center 프로젝트 (2개):**
- Sources/sgs_center/CenterCore.cpp
- Sources/sgs_center/main.cpp

**sgs_lobby 프로젝트 (5개):**
- Sources/sgs_lobby/R_LOBBY.cpp
- Sources/sgs_lobby/UnauthenticatedSessionManager.cpp
- Sources/sgs_lobby/LobbyCore.cpp
- Sources/sgs_lobby/main.cpp
- Sources/sgs_lobby/R_AUTHENTICATION.cpp

**sgs_query 프로젝트 (1개):**
- Sources/sgs_query/main.cpp

### 4. vcxproj.filters 필터명 변경
| 이전 | 이후 |
|------|------|
| _API | API |
| _Net | Net |
| _Net\Recv | Net\Recv |
| _Net\Send | Net\Send |
| _Net\Cmd | Net\Cmd |
| _Net\Database | Net\Database |
| _Util | Util |
