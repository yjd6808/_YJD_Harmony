# sg 프로젝트 폴더 구조 재정비 완료

## 작업일: 2026-04-16
## 결과: 빌드 성공 (경고 1개, 오류 0개)

## 수행한 작업

### 1. 물리 폴더 이름 변경
| 이전 | 이후 |
|------|------|
| Sources/sg/_API/ | Sources/sg/API/ |
| Sources/sg/_Core/ | Sources/sg/Core/ |
| Sources/sg/_Net/ | Sources/sg/Net/ |
| Sources/sg/_Object/ | Sources/sg/Object/ |
| Sources/sg/_Object/_Character/ | Sources/sg/Object/Character/ |
| Sources/sg/_Sga/ | Sources/sg/Sga/ |
| Sources/sg/_Struct/ | Sources/sg/Struct/ |
| Sources/sg/_Util/ | Sources/sg/Util/ |
| Sources/sg/_Util/_DescMgr/ | Sources/sg/Util/DescMgr/ |

### 2. 신규 폴더 생성 및 파일 이동
- `Sources/sg/Const/` 생성 → Const.h, Const.cpp, Const_*.h (14개 파일) 이동
- `Sources/sg/Contents/` 생성 → BaseContents.h, BaseContents.cpp 이동

### 3. #include 경로 일괄 수정
- sg 프로젝트 소스 92개 파일 수정
- sgcl, sgs, sgs_center 프로젝트 76개 파일 수정
- 치환 패턴:
  - `sg/_API/` → `sg/API/`
  - `sg/_Core/` → `sg/Core/`
  - `sg/_Net/` → `sg/Net/`
  - `sg/_Sga/` → `sg/Sga/`
  - `sg/_Struct/` → `sg/Struct/`
  - `sg/_Util/_DescMgr/` → `sg/Util/DescMgr/`
  - `sg/_Util/` → `sg/Util/`
  - `sg/Const.h` → `sg/Const/Const.h`
  - `sg/Const_*` → `sg/Const/Const_*`
  - `sg/BaseContents.h` → `sg/Contents/BaseContents.h`

### 4. sg.vcxproj 파일 경로 수정
- 모든 `_API\`, `_Core\`, `_Net\`, `_Object\`, `_Object\_Character\`, `_Sga\`, `_Struct\`, `_Util\`, `_Util\_DescMgr\` 경로 수정
- Const, Contents 파일 이동에 따른 경로 업데이트

### 5. sg.vcxproj.filters 수정
- 필터명: `_API`→`API`, `_Core`→`Core`, `_Net`→`Net`, `_Net\Cmd`→`Net\Cmd`, `_Net\Listener`→`Net\Listener`, `_Sga`→`Sga`, `_Struct`→`Struct`, `_Util`→`Util`, `_Util\_DescMgr`→`Util\DescMgr`
- 파일 경로도 동일하게 업데이트

## 빌드 결과
- Debug x64: **성공** (경고 1개: DescMgr_MapPhysics.h BOM 없음 - 기존 파일 문제)
