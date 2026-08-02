# 2026-08-02 XML 중괄호 파싱 규칙 변경 (# 접두사 + 중괄호 내부 표현식) — 완료

## 개요
XML 중괄호 파싱 규칙을 변경: `{#abcd.Width}` 형태의 `#` 접두사 요소 참조 도입, 중괄호 내부를 전체 표현식으로 평가, 중첩 중괄호 유지. dev layout XML 5종을 전부 새 문법으로 전환하고 표현식 테스트로 검증 완료.

## 변경 사항

### 코드 (Projects/sgcl)
- `Classes/sgcl/Game/UI/Xml/UIResolve.cpp`
  - `ParsePathBase`에 `#` 케이스 추가: `#이름` → `pRoot->FindElementByName`, 실패 시 Fail
  - bare-ident 요소 조회 제거: 키워드(`true`/`false`/`max`/`min`) 외 bare 식별자는 Fail (보간 폴백 → 빈 값)
  - `ResolveAttribute` 재작성: 백틱/중괄호 없으면 문자열 그대로, 단일 `` ` `` 감쌈은 리터럴, 단일 `{...}`(첫 `{`의 닫힘이 문자열 끝)이면 내부 전체 `ResolveExpression` 평가, 실패 시 `ResolveInterpolate` 보간 → **외부 수식(`{a} + 30`) 제거**
- `Classes/sgcl/Game/UI/Xml/UIResolve.h` — 문법 주석 갱신 (`{#Root.Width}`, 체인 `{#a.b.Width}`, 컬렉션 `{#List[0].Width}`, 중괄호/백틱 밖 연산 토큰 미지원 명시)
- `Classes/sgcl/Game/UI_Implementation/UI_Test.cpp` — OnInit(XML)에 표현식 검증 로그 추가 (`logExprText` 람다, 24개 expr + pbar/체크박스/그룹 크기)
- `Classes/sgcl/Game/UI_Implementation/UI_Generic.cpp` — OnInit(XML)에 btn_small/medium/large, chk_a, pbar_1, title 텍스트 크기/값 검증 로그 추가
- `Classes/sgcl/Scene/Scene_Login.cpp` — 임시 검증용 `Show("ui_test")`/`Show("ui_generic")`/`Show("ui_login")`/`Show("ui_inventory")` 추가 후 **원복 완료** (최종 상태는 기존과 동일: `Show("ui_channel")`만)

### XML (UltimateHarmony_Dev/assets/res_data/layout, 5종 전부 새 문법 전환)
- `ui_test.xml` — DataList(진법/타입/Map/중첩Map/Array/중첩인덱스) + 24개 표현식 TextBlock + ProgressBar/ToggleButton/CheckBox 바인딩
- `ui_generic.xml` — `{#btn_medium.Width * 2}`, `{#chk_a.Width + 15}`, `{#pbar_1.Height + 5}` 등 요소 체인 바인딩 + `{!{@chkDefault}}` 논리 + 보간(노트 텍스트)
- `ui_login.xml` — 창 크기/간격/입력 폭/배경색/문자열(`리소스 출처: {@sourceName}`) DataList 바인딩
- `ui_channel.xml` — 창/마진/간격/페이지 표시(`{@pageCur} / {@pageMax}`) DataList 바인딩, Grid 정의/구조/Name 유지
- `ui_inventory.xml` — 색/패딩/간격/슬롯 크기 DataList, `ScrollBar Height="{#inven_slots.Height}"` 요소 참조 연동

## 검증 결과 (런타임 로그)
- **ui_test 표현식 24개 전부 기대값 일치**: arith=70, paren=90, mod=2, data=120, hex=255, bin=13, float=1.23, double=1.2342342, map=John, nested=10, array=Shield, nestedIdx=Potion(중첩 인덱스), str=`Hello Hello`(백틱+문자열), cmp=false, logic=true, max=100, min=87, elem=160(`#`), chain=220(프로퍼티 체인), elemExpr=200(중괄호 내부 수식), parent=0(StackPanel Auto), root=640, interp=`HP: 87 (99Lv)`, empty=빈값(bare 제거)
- **부가 바인딩**: pbar_percent=30.0(배열 인덱스), chk_bind=1(`Level > 50` → bool 캐스팅), chk_bool=1, groupO2=290x140(`#groupO1.Width*2-30`, `#groupO1.Height+20`)
- **ui_generic 크기 체인**: btn_small 60x45 → btn_medium 120x60(=60*2) → btn_large 200x75(=120*2-40), chk_a 60x45, pbar_1 120x9, title=`Theme - Size / Gauge Demo`
- **ui_channel/ui_login/ui_inventory**: OnInit(XML) 정상, 모든 FindElementByName 포인터 non-null (ui_inventory `equip` 포인터만 nullptr — XML 분기에서 `pEquipSlotPanel_` 멤버를 미할당하는 **기존 코드 특성**, 이후 사용처 없음 → 기능 영향 없음)
- 빌드 0 에러/0 경고, 런타임 크래시 없음 (여러 회 실행)

## 알려진 사실
- `Scripts\BuildProject\sgcl.bat`은 cmd에서 `%~dp0`이 `..\..\Scripts\Build.ps1`로 잘못 확장되어 실패 → **`Scripts\Build.ps1` 직접 호출이 우회책**
  ```
  powershell -ExecutionPolicy Bypass -File Scripts\Build.ps1 -ProjectName sgcl -Configuration Debug -Platform x64 -FileLog -NoConsoleLogging
  ```
- 검증 실행: `Scripts\start_up_debugging.ps1 -ProjectName sgcl` (로그: `UltimateHarmony_Dev\release\sgcl\logs\default\`)
