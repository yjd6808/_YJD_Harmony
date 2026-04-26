# Plan: HashTable Pop 기능 추가

**날짜:** 2026-04-25  
**작업자:** AI

---

## 목표

`HashMap`, `HashSet`, `HashTable`, `HashTable_Bucket`에 `Pop` 함수를 추가한다.  
- Pop 반환값: 포인터 형태 (`TKey*` 또는 `TValue*`)
- 개체가 존재하지 않으면 `nullptr` 반환
- 반환된 포인터는 호출자가 `delete`로 해제 책임

---

## 분석

### 클래스 계층
```
HashSet / HashMap
    └── HashTable<TKey> / HashTable<TKey, TValue>
            └── Bucket<TKey> / Bucket<TKey, TValue>
                    └── BucketNode<T>
```

### Pop 반환 타입
- `HashSet::Pop(const TKey& _key)` → `TKey*`
- `HashMap::Pop(const TKey& _key)` → `TValue*`
- `HashTable<TKey>::Pop(const Ky& _key)` → `TKey*`
- `HashTable<TKey, TValue>::Pop(const Ky& _key)` → `TValue*`
- `Bucket<TKey>::Pop(const Ky& _key)` → `TKey*`
- `Bucket<TKey, TValue>::Pop(const Ky& _key)` → `TValue*`

### 구현 방식
1. `Bucket::Pop`: 찾은 인덱스의 데이터를 `::operator new + placement new(Move)`로 힙 복사 후 버킷에서 제거
2. `HashTable::Pop`: `Bucket::Pop` 호출 → 비어진 버킷 DisjointBucket → `--size_` → 포인터 반환
3. `HashSet::Pop`, `HashMap::Pop`: `HashTable::Pop` 호출하여 위임

---

## 수정 파일 목록

1. `Projects/jc/Sources/jc/Container/HashTable_Bucket.h`
   - `Bucket<TKey, TAllocator>::Pop` 추가 (Set용)
   - `Bucket<TKey, TValue, TAllocator>::Pop` 추가 (Map용)

2. `Projects/jc/Sources/jc/Container/HashTable.h`
   - `HashTable<TKey, TAllocator>::Pop` 추가 (Set용)
   - `HashTable<TKey, TValue, TAllocator>::Pop` 추가 (Map용)

3. `Projects/jc/Sources/jc/Container/HashSet.h`
   - `HashSet::Pop` 추가

4. `Projects/jc/Sources/jc/Container/HashMap.h`
   - `HashMap::Pop` 추가

---

## 작업 순서

1. HashTable_Bucket.h - Bucket Pop 추가
2. HashTable.h - HashTable Pop 추가
3. HashSet.h - HashSet Pop 추가
4. HashMap.h - HashMap Pop 추가
5. 빌드 확인 (jc)
