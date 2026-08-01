# jc_gbench 개발 TODO

## 코루틴 벤치마크
- [ ] 코루틴 컨텍스트 스위칭 오버헤드 측정
- [ ] 코루틴 풀 재사용 성능 vs 새 할당 비교
- [ ] 코루틴 기반 게임 루프 처리량(throughput) 측정

## 기존 모듈 벤치마크
- [ ] `ObjectPool` vs `new/delete` 비교 벤치마크
- [ ] `HashMap` vs `std::unordered_map` 성능 비교
- [ ] `SpinLock` vs `NormalLock` 경합 상황 비교
- [ ] `SPSC Queue` 처리량 측정
