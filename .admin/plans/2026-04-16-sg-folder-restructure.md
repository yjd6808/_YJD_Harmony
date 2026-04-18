# sg 프로젝트 폴더 구조 재정비 계획

## 작성일: 2026-04-16

## 목적
sg 프로젝트의 실제 폴더 구조를 Visual Studio 프로젝트 필터와 일치시킨다.

## 변경 대상

### 1. 폴더 이름 변경 (언더바 제거)
| 현재 폴더 | 변경 후 폴더 |
|-----------|-------------|
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
| 파일 (현재 위치) | 이동 위치 |
|-----------------|----------|
| Sources/sg/Const.h, Const.cpp, Const_*.h | Sources/sg/Const/ |
| Sources/sg/BaseContents.h, BaseContents.cpp | Sources/sg/Contents/ |

### 3. PCH 폴더
- Sources/sg/PCH/ 폴더는 이미 존재함 → 변경 없음

## 영향 범위

### include 경로 변경이 필요한 프로젝트
- `sg` - 내부 소스 전체
- `sgcl` - Classes/sgcl/ 하위 소스 전체
- `sgs` - Sources/sgs/ 하위 소스 전체
- `sgs_center` - CenterCore.cpp

### #include 패턴 치환 목록
| 치환 전 | 치환 후 |
|--------|--------|
| `sg/_API/` | `sg/API/` |
| `sg/_Core/` | `sg/Core/` |
| `sg/_Net/` | `sg/Net/` |
| `sg/_Sga/` | `sg/Sga/` |
| `sg/_Struct/` | `sg/Struct/` |
| `sg/_Util/_DescMgr/` | `sg/Util/DescMgr/` |
| `sg/_Util/` | `sg/Util/` |
| `sg/Const.h` | `sg/Const/Const.h` |
| `sg/Const_*.h` | `sg/Const/Const_*.h` |
| `sg/BaseContents.h` | `sg/Contents/BaseContents.h` |

### vcxproj 변경
- 모든 `_API\`, `_Core\`, `_Net\`, `_Object\`, `_Object\_Character\`, `_Sga\`, `_Struct\`, `_Util\`, `_Util\_DescMgr\` → 언더바 제거
- Const, Contents 파일들 경로 업데이트

### vcxproj.filters 변경
- 필터명: `_API`→`API`, `_Core`→`Core`, `_Net`→`Net`, `_Net\Cmd`→`Net\Cmd`, `_Net\Listener`→`Net\Listener`, `_Sga`→`Sga`, `_Struct`→`Struct`, `_Util`→`Util`, `_Util\_DescMgr`→`Util\DescMgr`
- 파일 경로도 동일하게 업데이트

## 실행 순서
1. 플랜 문서 작성 ✓
2. 물리 폴더 이름 변경 (Rename-Item)
3. 신규 폴더 생성 및 파일 이동 (Move-Item)
4. sg 프로젝트 소스 내 include 일괄 수정 (PowerShell replace)
5. sgcl, sgs, sgs_* 프로젝트 include 일괄 수정
6. sg.vcxproj 경로 수정
7. sg.vcxproj.filters 경로 및 필터명 수정
8. 빌드 수행
9. 히스토리 문서 작성
