/*
 * 작성자: 윤정도
 * 생성일: 12/12/2022 2:43:41 PM
 * =====================
 * 내가 생각대로 정의해본 메모리풀 메모리 할당 전략
 * 메모리풀에 대해서 제대로 학습한게 아닌 순수한 상태에서 내 생각을 표현한것이기 때문에 헛소리일 수도 있다.
 * 나중에 실제 전문가들이 구현한 메모리풀들을 확인해보는 과정을 거쳐서 내가 이런 생각을 했던게 의미가 있는건지
 * 확인해보는 과정이 필요하다.
 * 성능이 안좋으면 이렇게하는것도 다 의미가 없어진다.
 *
 * 일단 적긴적었는데 구현을 우째해야할지 막막하네
 *
 * ===========================================================================================
 * 여기서 락을 둔다는 의미는 락프리로 구현하든지, 실제 뮤텍스를 쓰든지 원자적 수행을 보장해준다는 의미로 적음
 *
 * eSingle: 싱글 쓰레드기반, 멀티 쓰레드 기반 상관없이 전역적으로 공유하는데 특화된 메모리 풀
 *          싱글 쓰레드기반시 락을 사용하지 않도록하고, 멀티 쓰레드 기반시 락을 사용토록 설정할 수 있다.
 *			이 설정은 어플리케이션 개발자가 컴파일타임에 수행토록 해줄 것
 *          50개의 쓰레드가 있으면 50개 모두가 하나의 풀을 사용하는 방식
 *
 * eMultiple: 메모리풀 활용도가 어느정도 높아서 Contention이 고려될 때
 *			  이를 어느정도 완화시키기위함
 *			  풀 갯수를 지정할 수 있다.
 *			  예를들어 풀 갯수를 3개를 지정하면
 *			  50개의 쓰레드가 있다고 했을 때 3개로 나눠서 접근하여 할당받음.
 * eTls: 쓰레드별로 각각 메모리풀을 둔다. (무야!)
 */


#pragma once

NS_JC_BEGIN
	

enum MemoryPoolStrategy
{
	eSingle = 0x1,
	eMultiple = 0x2,
	eTls = 0x3,
	eMemoryPoolStrategyMax = eTls,
	eMemoryPoolStrategyMask = 0xf
};

NS_DETAIL_BEGIN
template <MemoryPoolStrategy Strategy>
inline constexpr bool IsValidMemoryPoolStrategy = Strategy >= eSingle && Strategy <= eMemoryPoolStrategyMax;
NS_DETAIL_END

NS_JC_END 

