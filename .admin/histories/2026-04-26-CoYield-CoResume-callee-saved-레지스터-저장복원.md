# CoYield / CoResume - callee-saved 레지스터 저장/복원

- 날짜: 2026-04-26
- 작업자: AI (Claude)

---

## 작업 목적

Windows x64 ABI에서 코루틴 컨텍스트 스위칭 시 callee-saved 레지스터를 올바르게 저장/복원하지 않으면, 코루틴이 resume될 때 스케줄러의 레지스터 값이 오염될 수 있다. 이 문제를 해결하기 위해 `CoRegs` 구조체에 callee-saved 레지스터 필드를 추가하고, `CoYield` / `CoResume` / `CoRun` 어셈블리 코드에서 해당 레지스터를 저장/교환/복원하도록 구현하였다.

---

## 변경 파일

### 1. `Projects/jcr/Sources/jcr/06_CoroutineImpl1.h`

- `CoRegs` 구조체에 callee-saved 정수 레지스터 필드 추가
  - `rsi_`, `rdi_`, `r12_`, `r13_`, `r14_`, `r15_` (각 8바이트)
- `CoRegs` 구조체에 callee-saved XMM 레지스터 필드 추가
  - `xmm6_` ~ `xmm15_` (각 16바이트, `_u8[16]`)
- 각 필드에 오프셋 주석 추가 (offset 단위)
- `sizeof(CoRegs) = 248`
- 기존 필드(`gs16_`) 주석 오타 수정: `0x8` → `0x10`

### 2. `Projects/jcr/Sources/jcr/06_CoroutineImpl2.asm`

- `CoRegs` 구조체(MASM) 동기화: 정수/XMM 레지스터 필드 추가
- `OFFSET_COREGS_*` 상수 추가 (RSI, RDI, R12~R15, XMM6~XMM15)

#### `CoRun` 변경
- 코루틴 첫 실행 시 스케줄러의 callee-saved 정수 레지스터(`rsi`, `rdi`, `r12`~`r15`)를 `regs_`에 저장
- 스케줄러의 callee-saved XMM 레지스터(`xmm6`~`xmm15`)를 `regs_`에 저장 (`movdqu` 사용)

#### `CoYield` 변경
- xchg 패턴으로 callee-saved 정수 레지스터 교환
  - `r10`을 임시 레지스터로 사용
  - 패턴: `r10 = regs_[reg]` → `regs_[reg] = reg` → `reg = r10`
- xchg 패턴으로 callee-saved XMM 레지스터 교환
  - `xmm0`(volatile)을 임시 레지스터로 사용
  - 패턴: `xmm0 = regs_[xmmN]` → `regs_[xmmN] = xmmN` (movdqu) → `xmmN = xmm0` (movaps reg←reg)

#### `CoResume` 변경
- `CoYield`와 동일한 xchg 패턴으로 callee-saved 정수/XMM 레지스터 교환

---

## 핵심 설계 결정

| 항목 | 결정 | 이유 |
|------|------|------|
| XMM 저장 명령 | `movdqu` | CoRegs의 xmm 필드가 16바이트 정렬 보장 안됨 (8바이트 기준 구조체) |
| XMM 복원 명령 | `movaps reg←reg` | 레지스터→레지스터는 정렬 무관, movdqu보다 빠름 |
| 정수 레지스터 임시 | `r10` | CoYield/CoResume 진입 시점에 r10은 이미 다른 용도로 활용 완료 |
| XMM 임시 레지스터 | `xmm0` | Windows x64 ABI volatile 레지스터, 보존 불필요 |
| 저장 방식 | xchg (swap) | 별도 저장/복원을 한 번의 교환으로 처리하여 코드 단순화 |

---

## 참고

- Windows x64 ABI callee-saved 정수 레지스터: `rbx`, `rbp`, `rdi`, `rsi`, `r12`~`r15`
  - `rbx`, `rbp`는 기존에 이미 처리됨
- Windows x64 ABI callee-saved XMM 레지스터: `xmm6`~`xmm15`
