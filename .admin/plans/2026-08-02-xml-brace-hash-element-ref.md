# 2026-08-02 XML 중괄호 파싱 규칙 변경 (# 접두사 + 중괄호 내부 표현식)

## 목표
1. **`#` 접두사 요소 참조**: `{#abcd.Width}` — `#`을 붙여야 name 기반 요소 조회가 된다.
2. **중괄호 내부 = 전체 표현식**: `{#abcd.Width + 30}` — 중괄호 밖 연산(`{abcd.Width} + 30`)은 더 이상 수식으로 해석하지 않는다.
3. **중첩 중괄호 유지**: `{@Items[{@Index}]}` — 기존 동작 그대로.

## 현재 상태 (검토 완료)
- `{abcd.Width + 30}` 중괄호 내부 전체 표현식은 **이미 동작** (UIResolve.cpp:496-510 — `{` 토큰 → FindMatchingBrace → 내부 전체 `ResolveExpression` 재귀)
- 중첩 중괄호도 **이미 동작** (depth 카운팅)
- `{#abcd.Width}`는 **미지원** (ParsePathBase에 `#` 분기 없음 → 실패 → 보간 폴백 → 빈 값)
- bare-ident 요소 조회(`{abcd.Width}`)는 현재 ParsePathBase:643에서 동작 중
- `ResolveAttribute`(UIResolve.cpp:1151)는 "전체 문자열을 표현식 시도 → 실패 시 보간 폴백" 2단계 — 이것이 외부 수식(`{a} + 30`)을 허용하는 원인

## 확정 사항 (사용자 답변)
1. **bare-ident 요소 조회 제거** — `#` 없이 이름만 쓰는 `{abcd.Width}`는 요소 조회 실패. `{#abcd.Width}`만 유효.
2. **외부 수식 제거** — `{abcd.Width} + 30` 형태는 수식 평가하지 않음. 중괄호 내부에서만 표현식.
3. 체인 내 자식 이름 조회(`{#a.b.Width}`의 `.b`)는 기존 bare 유지 (베이스만 `#` 필수)
4. **dev layout XML 5종 전부 새 문법 전환** — 기존 정적 속성 문법 보존 불필요. DataList + `{#...}` + 중괄호 표현식 + 보간 적용
5. **표현식 테스트 XML 작업 + 전반 체크** — 각종 표현식(`@`데이터/`#`요소/`$`상대/중첩/비교/논리/max/min/백틱)이 문제없이 동작하는지 검증

## 변경 설계

### 1. UIResolve.cpp — ParsePathBase에 `#` 케이스 추가 + bare-ident 조회 제거
- `if (c == '#')`: `++pos_` → `ReadIdent()` → `ctx_->pRoot->FindElementByName()` → 실패 시 Fail
- bare 식별자: 키워드(`true`/`false`/`max(`/`min(`)만 유효, 그 외 Fail (요소 조회 제거)

### 2. UIResolve.cpp — ResolveAttribute 외부 수식 제거
- 전체가 단일 `{...}`로 감싸진 경우(첫 `{`의 닫힘이 문자열 끝)만 내부 `ResolveExpression` 평가
- 실패 시(또는 부분 중괄호) → 기존 `ResolveInterpolate` 보간
- `Width="120"` 정적 문자열, `Text="HP : {@Player.HP}"` 보간은 기존 동작 유지

### 3. UIResolve.h 주석 갱신
- 중괄호 토큰 표기: `{#Root.Width}`, `{@Player.Name}`, `{$parent.Width}` 등

## 작업 절차
1. [x] plan 파일 작성 (본 문서)
2. [x] UIResolve.cpp: ParsePathBase `#` 케이스 + bare-ident 요소 조회 제거
3. [x] UIResolve.cpp: ResolveAttribute — 전체 감쌈 판정 + 내부 표현식 + 보간 폴백 (외부 수식 제거)
4. [x] UIResolve.h 주석 갱신
5. [x] 빌드 (Build.ps1 sgcl Debug x64) 0 에러 + 런타임 정상 확인
6. [x] dev XML 5종 새 문법 전환 (ui_generic/ui_login/ui_channel/ui_inventory/ui_test)
   - DataList로 공통 값 정의, 정적 속성 → `{@key}`/`{#요소.프로퍼티}`/중괄호 표현식/보간 전환
   - Name/구조 유지 (코드 FindElementByName 연동 보존)
7. [x] ui_test.xml 표현식 테스트 확장 + UI_Test.cpp OnInit(XML) 검증 로그 추가
8. [x] 빌드 (sgcl Debug x64) 0 에러
9. [x] 런타임 검증 (바인딩 결과 로그 확인, 크래시 없음)
10. [x] plan/progress 업데이트 + done 문서 작성

## 검증 기준
- [x] `{#...}` 요소 참조 파싱 (코드/임시 XML로 확인)
- [x] `{#abcd.Width + 30}` 중괄호 내부 표현식 평가
- [x] `{abcd.Width}`(bare)는 실패 → 보간 빈 값
- [x] `{abcd.Width} + 30`(외부 수식)은 수식 미평가 (보간 결과)
- [x] `{@Items[{@Index}]}` 중첩 중괄호 동작 유지
- [x] dev XML 5종 새 문법 로드 + 코드 연동 정상 (OnInit 로그)
- [x] 표현식 테스트: 산술/데이터(Array·Map)/요소 참조/$상대/비교/논리/max·min/백틱/중첩 인덱스 동작 확인
- [x] 빌드 0 에러 / 런타임 크래시 없음

## 진행 로그
- 2026-08-02: plan 생성. 검토 완료 — 중괄호 내부 표현식/중첩은 이미 동작, `#` 추가 + bare 제거 + 외부 수식 제거 필요
- 2026-08-02: ParsePathBase `#` 케이스 + bare 요소 조회 제거, ResolveAttribute 외부 수식 제거 완료, 빌드 0 에러
- 2026-08-02: dev XML 5종 새 문법 전환 완료, ui_test 표현식 테스트 + 검증 로그 추가 완료
- 2026-08-02: **런타임 검증 완료** — ui_test 24개 표현식 전부 기대값 일치 (arith=70, paren=90, elem=160, elemExpr=200, chain=220, root=640, interp=HP: 87 (99Lv), empty=빈값, logic=true, cmp=false, max=100, min=87, nestedIdx=Potion 등) + pbar_percent=30 / chk_bind=1 / chk_bool=1 / groupO2=290x140. ui_generic 크기 체인 바인딩 정확 (btn_small 60x45 → btn_medium 120x60=60*2 → btn_large 200x75=120*2-40). ui_channel/ui_login/ui_inventory OnInit(XML) 정상. ui_inventory `equip` 로그 nullptr은 XML 분기에서 pEquipSlotPanel_ 멤버 미할당인 기존 코드 특성(사용처 없음) — 기능 영향 없음.
