# History: TryPop nullptr 처리 및 OUT 노테이션 추가

## 날짜
2026-04-25

## 작업 개요
`HashTable`, `HashTable_Bucket`, `TreeTable`의 `TryPop` 함수에 `_pOut == nullptr` 시 `false` 반환 처리 및 `OUT` 노테이션 추가

## 수정된 파일

### 1. `Projects/jc/Sources/jc/Container/HashTable_Bucket.h`
- **Line 286** `TryPop(const Ky& _key, TKey* _pOut)` (Set용)
  - `OUT` 노테이션 추가
  - `_pOut == nullptr` 시 `return false` 추가
- **Line 505** `TryPop(const Ky& _key, TValue* _pOut)` (Map용)
  - `OUT` 노테이션 추가
  - `_pOut == nullptr` 시 `return false` 추가

### 2. `Projects/jc/Sources/jc/Container/HashTable.h`
- **Line 197** `TryPop(const Ky& _key, TKey* _pOut)` (Set용)
  - `OUT` 노테이션 추가
  - `_pOut == nullptr` 시 `return false` 추가
- **Line 832** `TryPop(const Ky& _key, TValue* _pOut)` (Map용)
  - `OUT` 노테이션 추가
  - `_pOut == nullptr` 시 `return false` 추가

### 3. `Projects/jc/Sources/jc/Container/TreeTable.h`
- **Line 244** `TryPop(const TKey& _key, TKey* _pOut)` (Set용)
  - `OUT` 노테이션 추가
  - 기존 `if(_pOut != nullptr)` 허용 로직 → `_pOut == nullptr` 시 `return false`로 변경
  - 불필요한 nullptr 분기 제거
- **Line 1282** `TryPop(const TKey& key, TValue* _pOut)` (Map용)
  - `OUT` 노테이션 추가
  - 기존 `if(_pOut != nullptr)` 허용 로직 → `_pOut == nullptr` 시 `return false`로 변경
  - 불필요한 nullptr 분기 제거

## 빌드 결과
- **jc** Debug/x64: 경고 0개, 오류 0개 - 성공
