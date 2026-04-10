# jcr (JCore Research)

## 개요
Windows 힙 메모리 관리 및 메모리 할당 전략을 **실험/연구**하는 프로젝트.  
jc 라이브러리를 기반으로 저수준 메모리 동작과 x64 호출 규약을 분석.

## 소스 구조

```
Sources/jcr/
├── Core.h/cpp      - jc 라이브러리 통합 include
├── main.cpp        - 연구 메인 로직
└── Test.asm        - x64 어셈블리 테스트 코드
```

## 현재 연구 내용

### 1. Windows 힙 메모리 분석
- `GetProcessHeaps()` / `HeapWalk()` 로 힙 블록 순회
- 결과 파일: `process_heap.txt`, `heap_lfh_test.txt`

### 2. 힙 할당 전략 비교
| 전략 | 설명 |
|------|------|
| HEAP_STANDARD | 표준 힙 |
| HEAP_LAL | Low-fragmentation Allocation |
| HEAP_LFH | Low-Fragmentation Heap (HeapSetInformation 활성화) |

### 3. x64 어셈블리 연구 (Test.asm)
- `get_rbp()` - RBP 레지스터 값 반환
- `guard_page_check(rcx)` - 스택 Guard Page 메커니즘 테스트
- `params_3/4/5()` - x64 calling convention 파라미터 전달 검증

## 의존성
- `jc` (Static Library) - jc.lib 링크

## 빌드 특이사항
- ASLR 비활성화 (`RandomizedBaseAddress=false`) - 주소 분석 연구용
- 스택 크기: Debug=1,400,000 / Release=140,000 바이트
- Language Standard: C++20
- 산출물: 실행파일 (EXE)
