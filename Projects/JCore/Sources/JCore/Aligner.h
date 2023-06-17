/*
 * 작성자: 윤정도
 */

#pragma once

#include <JCore/Namespace.h>

NS_JC_BEGIN

// operator 오버로딩을 사용하지 않는 경우 쓰는게 좋을듯..
template <typename T>
struct alignas(64) Aligner : T {
	using Type = T;
};

NS_JC_END

/*
 *	struct A
 *	{
 *		alignas(0x1000) struct alignas(64) {
 *			int data;
 *		} arrays[100];
 *	
 *		// => ANONYMOUS_ALIGNED_VAR(0x1000, 64, int data) arrays[100];
 *	};
 */
#define ANONYMOUS_ALIGNED_VAR(variable_align, element_align, element_variable)	\
	alignas(variable_align) struct alignas(element_align) {						\
		element_variable														\
	}

#define ALIGNED_VAR(variable_align, element_align, element_variable, struct_name) \
	alignas(variable_align) struct alignas(element_align) struct_name {			 \
		element_variable														 \
	}

#define ANONYMOUS_CACHE_ALIGNED_VAR(element_variable)	\
	alignas(64) struct alignas(64) {					\
		element_variable								\
	}