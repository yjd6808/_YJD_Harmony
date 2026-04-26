# Plan: TreeTable TryPop 구현

**날짜**: 2026-04-25  
**작업**: TreeTable(TreeSet/TreeMap)에 TryPop 기능 추가

---

## 배경 및 목적

HashTable/HashSet/HashMap에는 `TryPop` 메서드가 있어서  
키에 해당하는 값을 꺼내면서(복사/이동) 동시에 트리에서 제거하는 기능이 있다.  
TreeTable(TreeSet/TreeMap)에는 아직 해당 기능이 없으므로 추가한다.

---

## 구현 범위

### 1. `TreeTable<TKey>` (TreeSet용) — `TreeTable.h` 약 Line 244 근처

```cpp
bool TryPop(const TKey& _key, TKey* _pOut)
{
    TTreeNode* pNode = FindNode(_key);
    if (pNode == nullptr)
        return false;
    new (_pOut) TKey(Move(pNode->data_));
    RemoveByNode(pNode);
    return true;
}
```

### 2. `TreeTable<TKey, TValue>` (TreeMap용) — `TreeTable.h` 약 Line 1165 근처

```cpp
// 키로 값을 꺼내면서 삭제 (Value만 반환)
bool TryPop(const TKey& _key, TValue* _pOut)
{
    TTreeNode* pNode = FindNode(_key);
    if (pNode == nullptr)
        return false;
    new (_pOut) TValue(Move(pNode->data_.value_));
    RemoveByNode(pNode);
    return true;
}
```

### 3. `TreeSet.h` — Remove 아래쪽에 추가

```cpp
bool TryPop(const TKey& _key, TKey* _pOut)
{
    return table_.TryPop(_key, _pOut);
}
```

### 4. `TreeMap.h` — Remove 아래쪽에 추가

```cpp
bool TryPop(const TKey& _key, TValue* _pOut)
{
    return treeTable_.TryPop(_key, _pOut);
}
```

---

## 수정 파일 목록

| 파일 | 변경 내용 |
|------|----------|
| `Projects/jc/Sources/jc/Container/TreeTable.h` | TryPop 추가 (TKey 버전, TKey+TValue 버전) |
| `Projects/jc/Sources/jc/Container/TreeSet.h` | TryPop 래퍼 추가 |
| `Projects/jc/Sources/jc/Container/TreeMap.h` | TryPop 래퍼 추가 |

총 3개 파일 수정

---

## 작업 순서

1. TreeTable.h — TKey 버전 TryPop 추가 (TryGetFirst 바로 위)
2. TreeTable.h — TKey+TValue 버전 TryPop 추가 (TryGetFirst 바로 위)
3. TreeSet.h — TryPop 래퍼 추가 (Remove 아래)
4. TreeMap.h — TryPop 래퍼 추가 (Remove 아래)
5. 빌드 수행 (jc 프로젝트)
6. /done 수행

---

## 참고

- HashTable의 TryPop: `placement new`로 _pOut에 Move 복사 후 노드 삭제
- TreeTable Remove는 `RemoveByNode(pNode)` 사용
- placement new 패턴: `new (_pOut) TType(Move(data_))`
