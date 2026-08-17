# Done: HashTable Pop 기능 추가

**날짜:** 2026-04-25  
**결과:** 완료 (빌드 성공, 경고 0, 오류 0)

---

## 작업 내용

`HashMap`, `HashSet`, `HashTable`, `HashTable_Bucket`에 `Pop` 함수를 추가했다.

### Pop 동작
- 키에 대응하는 값을 찾아 `::operator new + placement new(Move)`로 힙에 복사
- 버킷에서 해당 항목 제거 및 버킷이 비어지면 연결 해제
- 반환된 포인터는 호출자가 `delete`로 해제 책임
- 개체 미존재 시 `nullptr` 반환

---

## 수정 파일

### `HashTable_Bucket.h`
- `Bucket<TKey, TAllocator>::Pop<Ky>(const Ky& _key) -> TKey*` 추가 (Set용)
- `Bucket<TKey, TValue, TAllocator>::Pop<Ky>(const Ky& _key) -> TValue*` 추가 (Map용)

### `HashTable.h`
- `HashTable<TKey, TAllocator>::Pop<Ky>(const Ky& _key) -> TKey*` 추가
- `HashTable<TKey, TValue, TAllocator>::Pop<Ky>(const Ky& _key) -> TValue*` 추가

### `HashSet.h`
- `HashSet::Pop(const TKey& _key) -> TKey*` 추가
- `HashSet::Pop(const char* _key) -> TKey*` 추가 (IsStringType_v TKey 전용)

### `HashMap.h`
- `HashMap::Pop(const TKey& _key) -> TValue*` 추가
- `HashMap::Pop(const char* _pKey) -> TValue*` 추가 (IsStringType_v TKey 전용)

---

## 사용 예시

```cpp
// HashMap
HashMap<String, int*> map;
map.Insert("key", new int(42));
int* pVal = map.Pop("key");  // 42 포인터 반환, map에서 제거
// pVal 사용 후:
delete pVal;

// HashSet
HashSet<String> set;
set.Insert("hello");
String* pKey = set.Pop("hello");  // "hello" 반환, set에서 제거
delete pKey;
```
