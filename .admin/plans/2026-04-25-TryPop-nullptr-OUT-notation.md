# Plan: TryPop nullptr 처리 및 OUT 노테이션 추가

## 날짜
2026-04-25

## 작업 개요
`HashTable`, `HashTable_Bucket`, `TreeTable`의 `TryPop` 함수에 대해:
1. `_pOut` 매개변수가 `nullptr`인 경우 `false`를 반환하도록 처리 추가
2. `TValue* _pOut` / `TKey* _pOut` 매개변수에 `OUT` 노테이션 추가

## 수정 대상 파일 (3개)

### 1. `Projects/jc/Sources/jc/Container/HashTable_Bucket.h`
- **Line 286**: `TryPop(const Ky& _key, TKey* _pOut)` (Set용)
  - `OUT` 노테이션 추가
  - `_pOut == nullptr` 시 `false` 반환 추가
- **Line 505**: `TryPop(const Ky& _key, TValue* _pOut)` (Map용)
  - `OUT` 노테이션 추가
  - `_pOut == nullptr` 시 `false` 반환 추가

### 2. `Projects/jc/Sources/jc/Container/HashTable.h`
- **Line 197**: `TryPop(const Ky& _key, TKey* _pOut)` (Set용)
  - `OUT` 노테이션 추가
  - `_pOut == nullptr` 시 `false` 반환 추가
- **Line 832**: `TryPop(const Ky& _key, TValue* _pOut)` (Map용)
  - `OUT` 노테이션 추가
  - `_pOut == nullptr` 시 `false` 반환 추가

### 3. `Projects/jc/Sources/jc/Container/TreeTable.h`
- **Line 244**: `TryPop(const TKey& _key, TKey* _pOut)` (Set용)
  - `OUT` 노테이션 추가
  - 기존 `nullptr` 허용 로직을 `false` 반환으로 변경
- **Line 1282**: `TryPop(const TKey& key, TValue* _pOut)` (Map용)
  - `OUT` 노테이션 추가
  - 기존 `nullptr` 허용 로직을 `false` 반환으로 변경

## 변경 패턴

### Before (HashTable_Bucket, HashTable)
```cpp
bool TryPop(const Ky& _key, TValue* _pOut)
{
    // nullptr 체크 없음
    ...
    new (_pOut) TValue(...);  // nullptr이면 크래시
```

### After (HashTable_Bucket, HashTable)
```cpp
bool TryPop(const Ky& _key, OUT TValue* _pOut)
{
    if (_pOut == nullptr)
    {
        return false;
    }
    ...
```

### Before (TreeTable)
```cpp
bool TryPop(const TKey& _key, TKey* _pOut)
{
    ...
    if (_pOut != nullptr)
    {
        *_pOut = pNode->data_;
    }
    RemoveByNode(pNode);
    return true;
}
```

### After (TreeTable)
```cpp
bool TryPop(const TKey& _key, OUT TKey* _pOut)
{
    if (_pOut == nullptr)
    {
        return false;
    }
    ...
    *_pOut = pNode->data_;
    RemoveByNode(pNode);
    return true;
}
```
