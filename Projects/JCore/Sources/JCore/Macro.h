﻿/*
 * 작성자: 윤정도
 * 생성일: 5/20/2023 8:12:30 PM
 * =====================
 * 매크로 학습 후 나만의 매크로 기능들 구현
 * 학습 결과 정리: https://blog.naver.com/reversing_joa/223105229138
 */


#pragma once

// @참고: https://learn.microsoft.com/ko-kr/cpp/build/reference/zc-preprocessor?view=msvc-170
//    => GCC, Clang 전처리기와 동일하게 동작하도록 굳이 설정해줄 필요없다.
//       MSVC 전처리기의 동작방식에 익숙해지도록 해보자.
//
// #if _MSVC_TRADITIONAL
// #error C/C++ -> Preprocessor -> Use Standard Conforming Preprocessor -> Yes로 변경
// #endif

// 비주얼스튜디오 2019이상에서 사용가능한 /Zc:preprocessor 컴파일러 옵션을 켜지 않을 경우
// MSVC 전처리기의 경우 다른 GCC와 Clang 같은 컴파일러의 전처리기와 달리 __VA_ARGS__와 중첩 매크로구조에서의 ,(콤마)가
// 인자 구분자로서 동작하지 않기 때문에 Pre Scan을 강제로 발생시키기 위한 EXPAND 매크로를 정의하였다.
// (적기 귀찮으므로 5개까지만 적음, 추후 확장시 추가할 것)
#define JCORE_EXPAND_1(_1)								_1
#define JCORE_EXPAND_2(_1,_2)							_1,_2
#define JCORE_EXPAND_3(_1,_2,_3)						_1,_2,_3
#define JCORE_EXPAND_4(_1,_2,_3,_4)						_1,_2,_3,_4
#define JCORE_EXPAND_5(_1,_2,_3,_4,_5)					_1,_2,_3,_4,_5
#define JCORE_EXPAND_ARGS(...)							__VA_ARGS__


// 인자 갯수를 반환한다. (적기 귀찮으므로 10개까지만 적음, 추후 확장시 추가할 것
//   -> 확장사유: 2023/06/13 인자 40개로 확장함. SteinsGate/Common/QueryDefine.h의 SelectResult<TQry> 바인딩시 인자가 수십개 전달될 수도 있기 때문
//               매크로함수에 인자 수십개를 전달할 일이 있을까 했지만 있었다. ㄷㄷ
#define JCORE_ARGS_COUNT(...) JCORE_ARGS_COUNT_(__VA_ARGS__, JCORE_ARGS_COUNT_REST)
#define JCORE_ARGS_COUNT_(...) JCORE_EXPAND_1(JCORE_ARGS_COUNT__(__VA_ARGS__))			// JCORE_ARGS_COUNT_REST를 확장시켜주기 위한 중간 매크로
#define JCORE_ARGS_COUNT__(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_49,_40,N,...) N
#define JCORE_ARGS_COUNT_REST 40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0


// 전달받은 인자들을 토큰 결합시킨다. (적기 귀찮으므로 5개까지만 적음, 추후 확장시 추가할 것)
// 인자들중 홀수번째 인자들만 가져옴
#define JCORE_CONCAT_1(_1)								JCORE_CONCAT_1_(_1)
#define JCORE_CONCAT_1_(_1)								_1
#define JCORE_CONCAT_2(_1,_2)							JCORE_CONCAT_2_(_1,_2)
#define JCORE_CONCAT_2_(_1,_2)							_1##_2
#define JCORE_CONCAT_3(_1,_2,_3)						JCORE_CONCAT_3_(_1,_2,_3)
#define JCORE_CONCAT_3_(_1,_2,_3)						_1##_2##_3
#define JCORE_CONCAT_4(_1,_2,_3,_4)						JCORE_CONCAT_4_(_1,_2,_3,_4)
#define JCORE_CONCAT_4_(_1,_2,_3,_4)					_1##_2##_3##_4
#define JCORE_CONCAT_5(_1,_2,_3,_4,_5)					JCORE_CONCAT_5_(_1,_2,_3,_4,_5)
#define JCORE_CONCAT_5_(_1,_2,_3,_4,_5)					_1##_2##_3##_4##_5

#define JCORE_CONCAT_COUNTER(arg_name)					JCORE_CONCAT_2(arg_name, __COUNTER__)
#define JCORE_CONCAT_ARGS(...)																		\
	JCORE_EXPAND_1(																					\
		JCORE_CONCAT_3(																				\
			JCORE_CONCAT_, JCORE_ARGS_COUNT(__VA_ARGS__), (__VA_ARGS__)								\
		)																							\
	)


// 인자들중 짝수번째 인자들만 가져옴
// 반복작성이 귀찮아서 1~5번까지만 내가 적고 CHAT GPT한테 이어서 작성해달라캄
#define JCORE_EVEN_1(_1)
#define JCORE_EVEN_2(_1,_2)_2
#define JCORE_EVEN_3(_1,_2,_3)_2
#define JCORE_EVEN_4(_1,_2,_3,_4)_2,_4
#define JCORE_EVEN_5(_1,_2,_3,_4,_5)_2,_4
#define JCORE_EVEN_6(_1,_2,_3,_4,_5,_6)_2,_4,_6
#define JCORE_EVEN_7(_1,_2,_3,_4,_5,_6,_7)_2,_4,_6
#define JCORE_EVEN_8(_1,_2,_3,_4,_5,_6,_7,_8)_2,_4,_6,_8
#define JCORE_EVEN_9(_1,_2,_3,_4,_5,_6,_7,_8,_9)_2,_4,_6,_8
#define JCORE_EVEN_10(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10)_2,_4,_6,_8,_10
#define JCORE_EVEN_11(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11)_2,_4,_6,_8,_10
#define JCORE_EVEN_12(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12)_2,_4,_6,_8,_10,_12
#define JCORE_EVEN_13(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13)_2,_4,_6,_8,_10,_12
#define JCORE_EVEN_14(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14)_2,_4,_6,_8,_10,_12,_14
#define JCORE_EVEN_15(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15)_2,_4,_6,_8,_10,_12,_14
#define JCORE_EVEN_16(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16)_2,_4,_6,_8,_10,_12,_14,_16
#define JCORE_EVEN_17(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17)_2,_4,_6,_8,_10,_12,_14,_16
#define JCORE_EVEN_18(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18)_2,_4,_6,_8,_10,_12,_14,_16,_18
#define JCORE_EVEN_19(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19)_2,_4,_6,_8,_10,_12,_14,_16,_18
#define JCORE_EVEN_20(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20
#define JCORE_EVEN_21(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20
#define JCORE_EVEN_22(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22
#define JCORE_EVEN_23(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22
#define JCORE_EVEN_24(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22,_24
#define JCORE_EVEN_25(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22,_24
#define JCORE_EVEN_26(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22,_24,_26
#define JCORE_EVEN_27(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22,_24,_26
#define JCORE_EVEN_28(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22,_24,_26,_28
#define JCORE_EVEN_29(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22,_24,_26,_28
#define JCORE_EVEN_30(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22,_24,_26,_28,_30
#define JCORE_EVEN_31(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22,_24,_26,_28,_30
#define JCORE_EVEN_32(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22,_24,_26,_28,_30,_32
#define JCORE_EVEN_33(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22,_24,_26,_28,_30,_32
#define JCORE_EVEN_34(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22,_24,_26,_28,_30,_32,_34
#define JCORE_EVEN_35(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22,_24,_26,_28,_30,_32,_34
#define JCORE_EVEN_36(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22,_24,_26,_28,_30,_32,_34,_36
#define JCORE_EVEN_37(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37)_2,_4,_6,_8,_10,_12,_14,_16,_18,_20,_22,_24,_26,_28,_30,_32,_34,_36
#define JCORE_EVEN_ARGS(...)											\
	JCORE_EXPAND_1(														\
		JCORE_CONCAT_2(													\
			JCORE_EVEN_, JCORE_ARGS_COUNT(__VA_ARGS__), (__VA_ARGS__)	\
		)																\
	)



// 인자들중 홀수번째 인자들만 가져옴
// 반복작성이 귀찮아서 1~5번까지만 내가 적고 CHAT GPT한테 이어서 작성해달라캄
#define JCORE_ODD_1(_1)_1
#define JCORE_ODD_2(_1,_2)_1
#define JCORE_ODD_3(_1,_2,_3)_1,_3
#define JCORE_ODD_4(_1,_2,_3,_4)_1,_3
#define JCORE_ODD_5(_1,_2,_3,_4,_5)_1,_3,_5
#define JCORE_ODD_6(_1,_2,_3,_4,_5,_6)_1,_3,_5
#define JCORE_ODD_7(_1,_2,_3,_4,_5,_6,_7)_1,_3,_5,_7
#define JCORE_ODD_8(_1,_2,_3,_4,_5,_6,_7,_8)_1,_3,_5,_7
#define JCORE_ODD_9(_1,_2,_3,_4,_5,_6,_7,_8,_9)_1,_3,_5,_7,_9
#define JCORE_ODD_10(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10)_1,_3,_5,_7,_9
#define JCORE_ODD_11(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11)_1,_3,_5,_7,_9,_11
#define JCORE_ODD_12(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12)_1,_3,_5,_7,_9,_11
#define JCORE_ODD_13(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13)_1,_3,_5,_7,_9,_11,_13
#define JCORE_ODD_14(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14)_1,_3,_5,_7,_9,_11,_13
#define JCORE_ODD_15(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15)_1,_3,_5,_7,_9,_11,_13,_15
#define JCORE_ODD_16(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16)_1,_3,_5,_7,_9,_11,_13,_15
#define JCORE_ODD_17(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17)_1,_3,_5,_7,_9,_11,_13,_15,_17
#define JCORE_ODD_18(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18)_1,_3,_5,_7,_9,_11,_13,_15,_17
#define JCORE_ODD_19(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19
#define JCORE_ODD_20(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19
#define JCORE_ODD_21(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21
#define JCORE_ODD_22(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21
#define JCORE_ODD_23(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21,_23
#define JCORE_ODD_24(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21,_23
#define JCORE_ODD_25(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21,_23,_25
#define JCORE_ODD_26(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21,_23,_25
#define JCORE_ODD_27(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21,_23,_25,_27
#define JCORE_ODD_28(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21,_23,_25,_27
#define JCORE_ODD_29(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21,_23,_25,_27,_29
#define JCORE_ODD_30(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21,_23,_25,_27,_29
#define JCORE_ODD_31(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21,_23,_25,_27,_29,_31
#define JCORE_ODD_32(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21,_23,_25,_27,_29,_31
#define JCORE_ODD_33(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21,_23,_25,_27,_29,_31,_33
#define JCORE_ODD_34(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21,_23,_25,_27,_29,_31,_33
#define JCORE_ODD_35(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21,_23,_25,_27,_29,_31,_33,_35
#define JCORE_ODD_36(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21,_23,_25,_27,_29,_31,_33,_35
#define JCORE_ODD_37(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37)_1,_3,_5,_7,_9,_11,_13,_15,_17,_19,_21,_23,_25,_27,_29,_31,_33,_35,_37
#define JCORE_ODD_ARGS(...)												\
	JCORE_EXPAND_1(														\
		JCORE_CONCAT_3(													\
			JCORE_ODD_, JCORE_ARGS_COUNT(__VA_ARGS__), (__VA_ARGS__)	\
		)																\
	)


// 특정인덱스번째 인자를 가져오도록 한다.
#define JCORE_INDEX_OF_0(_0,...) _0
#define JCORE_INDEX_OF_1(_0,_1,...) _1
#define JCORE_INDEX_OF_2(_0,_1,_2,...) _2
#define JCORE_INDEX_OF_3(_0,_1,_2,_3,...) _3
#define JCORE_INDEX_OF_4(_0,_1,_2,_3,_4,...) _4
#define JCORE_INDEX_OF_5(_0,_1,_2,_3,_4,_5,...) _5
#define JCORE_INDEX_OF_6(_0,_1,_2,_3,_4,_5,_6,...) _6
#define JCORE_INDEX_OF_7(_0,_1,_2,_3,_4,_5,_6,_7,...) _7
#define JCORE_INDEX_OF_8(_0,_1,_2,_3,_4,_5,_6,_7,_8,...) _8
#define JCORE_INDEX_OF_9(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,...) _9
#define JCORE_INDEX_OF_10(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,...) _10
#define JCORE_INDEX_OF_11(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,...) _11
#define JCORE_INDEX_OF_12(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,...) _12
#define JCORE_INDEX_OF_13(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,...) _13
#define JCORE_INDEX_OF_14(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,...) _14
#define JCORE_INDEX_OF_15(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,...) _15
#define JCORE_INDEX_OF_16(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,...) _16
#define JCORE_INDEX_OF_17(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,...) _17
#define JCORE_INDEX_OF_18(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,...) _18
#define JCORE_INDEX_OF_19(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,...) _19
#define JCORE_INDEX_OF_20(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,...) _20
#define JCORE_INDEX_OF_21(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,...) _21
#define JCORE_INDEX_OF_22(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,...) _22
#define JCORE_INDEX_OF_23(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,...) _23
#define JCORE_INDEX_OF_24(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,...) _24
#define JCORE_INDEX_OF_25(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,...) _25
#define JCORE_INDEX_OF_26(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,...) _26
#define JCORE_INDEX_OF_27(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,...) _27
#define JCORE_INDEX_OF_28(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,...) _28
#define JCORE_INDEX_OF_29(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,...) _29
#define JCORE_INDEX_OF_30(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,...) _30
#define JCORE_INDEX_OF(index, ...)										\
	JCORE_EXPAND_1(														\
		JCORE_CONCAT_3(													\
			JCORE_INDEX_OF_, index, (__VA_ARGS__)						\
		)																\
	)




// 매크로인자 그룹핑 기능 구현
// JCORE_GROUP_OF(2, 1, 2, 3, 4, 5, 6)
//  => (1,2),(3,4),(5,6)과 같이 확장해준다.
#define JCORE_GROUP_OF_2_2(_1,_2) \
	(_1,_2)
#define JCORE_GROUP_OF_2_4(_1,_2,_3,_4) \
	(_1,_2),(_3,_4)
#define JCORE_GROUP_OF_2_6(_1,_2,_3,_4,_5,_6) \
	(_1,_2),(_3,_4),(_5,_6)
#define JCORE_GROUP_OF_2_8(_1,_2,_3,_4,_5,_6,_7,_8) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8)
#define JCORE_GROUP_OF_2_10(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8),(_9,_10)
#define JCORE_GROUP_OF_2_12(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8),(_9,_10),(_11,_12)
#define JCORE_GROUP_OF_2_14(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8),(_9,_10),(_11,_12),(_13,_14)
#define JCORE_GROUP_OF_2_16(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8),(_9,_10),(_11,_12),(_13,_14),(_15,_16)
#define JCORE_GROUP_OF_2_18(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8),(_9,_10),(_11,_12),(_13,_14),(_15,_16),(_17,_18)
#define JCORE_GROUP_OF_2_20(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8),(_9,_10),(_11,_12),(_13,_14),(_15,_16),(_17,_18),(_19,_20)
#define JCORE_GROUP_OF_2_22(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8),(_9,_10),(_11,_12),(_13,_14),(_15,_16),(_17,_18),(_19,_20),(_21,_22)
#define JCORE_GROUP_OF_2_24(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8),(_9,_10),(_11,_12),(_13,_14),(_15,_16),(_17,_18),(_19,_20),(_21,_22),(_23,_24)
#define JCORE_GROUP_OF_2_26(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8),(_9,_10),(_11,_12),(_13,_14),(_15,_16),(_17,_18),(_19,_20),(_21,_22),(_23,_24),(_25,_26)
#define JCORE_GROUP_OF_2_28(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8),(_9,_10),(_11,_12),(_13,_14),(_15,_16),(_17,_18),(_19,_20),(_21,_22),(_23,_24),(_25,_26),(_27,_28)
#define JCORE_GROUP_OF_2_30(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8),(_9,_10),(_11,_12),(_13,_14),(_15,_16),(_17,_18),(_19,_20),(_21,_22),(_23,_24),(_25,_26),(_27,_28),(_29,_30)
#define JCORE_GROUP_OF_2_32(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8),(_9,_10),(_11,_12),(_13,_14),(_15,_16),(_17,_18),(_19,_20),(_21,_22),(_23,_24),(_25,_26),(_27,_28),(_29,_30),(_31,_32)
#define JCORE_GROUP_OF_2_34(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8),(_9,_10),(_11,_12),(_13,_14),(_15,_16),(_17,_18),(_19,_20),(_21,_22),(_23,_24),(_25,_26),(_27,_28),(_29,_30),(_31,_32),(_33,_34)
#define JCORE_GROUP_OF_2_36(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8),(_9,_10),(_11,_12),(_13,_14),(_15,_16),(_17,_18),(_19,_20),(_21,_22),(_23,_24),(_25,_26),(_27,_28),(_29,_30),(_31,_32),(_33,_34),(_35,_36)
#define JCORE_GROUP_OF_2_38(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38) \
	(_1,_2),(_3,_4),(_5,_6),(_7,_8),(_9,_10),(_11,_12),(_13,_14),(_15,_16),(_17,_18),(_19,_20),(_21,_22),(_23,_24),(_25,_26),(_27,_28),(_29,_30),(_31,_32),(_33,_34),(_35,_36),(_37,_38)


#define JCORE_GROUP_OF(group_size, ...)													\
	JCORE_EXPAND_1(																		\
		JCORE_CONCAT_5(																	\
			JCORE_GROUP_OF_, group_size, _, JCORE_ARGS_COUNT(__VA_ARGS__), (__VA_ARGS__)\
		)																				\
	)



// 매크로 적용자 구현
#define JCORE_APPLY_FN_1(fn,_1) \
	fn(_1)
#define JCORE_APPLY_FN_2(fn,_1,_2) \
	fn(_1) fn(_2)
#define JCORE_APPLY_FN_3(fn,_1,_2,_3) \
	fn(_1) fn(_2) fn(_3)
#define JCORE_APPLY_FN_4(fn,_1,_2,_3,_4) \
    fn(_1) fn(_2) fn(_3) fn(_4)
#define JCORE_APPLY_FN_5(fn,_1,_2,_3,_4,_5) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5)
#define JCORE_APPLY_FN_6(fn,_1,_2,_3,_4,_5,_6) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6)
#define JCORE_APPLY_FN_7(fn,_1,_2,_3,_4,_5,_6,_7) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7)
#define JCORE_APPLY_FN_8(fn,_1,_2,_3,_4,_5,_6,_7,_8) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8)
#define JCORE_APPLY_FN_9(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9)
#define JCORE_APPLY_FN_10(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10)
#define JCORE_APPLY_FN_11(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11)
#define JCORE_APPLY_FN_12(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12)
#define JCORE_APPLY_FN_13(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13)
#define JCORE_APPLY_FN_14(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14)
#define JCORE_APPLY_FN_15(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15)
#define JCORE_APPLY_FN_16(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15) fn(_16)
#define JCORE_APPLY_FN_17(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15) fn(_16) fn(_17)
#define JCORE_APPLY_FN_18(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15) fn(_16) fn(_17) fn(_18)
#define JCORE_APPLY_FN_19(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15) fn(_16) fn(_17) fn(_18) fn(_19)
#define JCORE_APPLY_FN_20(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15) fn(_16) fn(_17) fn(_18) fn(_19) fn(_20)
#define JCORE_APPLY_FN_21(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15) fn(_16) fn(_17) fn(_18) fn(_19) fn(_20) fn(_21)
#define JCORE_APPLY_FN_22(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15) fn(_16) fn(_17) fn(_18) fn(_19) fn(_20) fn(_21) fn(_22)
#define JCORE_APPLY_FN_23(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15) fn(_16) fn(_17) fn(_18) fn(_19) fn(_20) fn(_21) fn(_22) fn(_23)
#define JCORE_APPLY_FN_24(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15) fn(_16) fn(_17) fn(_18) fn(_19) fn(_20) fn(_21) fn(_22) fn(_23) fn(_24)
#define JCORE_APPLY_FN_25(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15) fn(_16) fn(_17) fn(_18) fn(_19) fn(_20) fn(_21) fn(_22) fn(_23) fn(_24) fn(_25)
#define JCORE_APPLY_FN_26(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15) fn(_16) fn(_17) fn(_18) fn(_19) fn(_20) fn(_21) fn(_22) fn(_23) fn(_24) fn(_25) fn(_26)
#define JCORE_APPLY_FN_27(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15) fn(_16) fn(_17) fn(_18) fn(_19) fn(_20) fn(_21) fn(_22) fn(_23) fn(_24) fn(_25) fn(_26) fn(_27)
#define JCORE_APPLY_FN_28(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15) fn(_16) fn(_17) fn(_18) fn(_19) fn(_20) fn(_21) fn(_22) fn(_23) fn(_24) fn(_25) fn(_26) fn(_27) fn(_28)
#define JCORE_APPLY_FN_29(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15) fn(_16) fn(_17) fn(_18) fn(_19) fn(_20) fn(_21) fn(_22) fn(_23) fn(_24) fn(_25) fn(_26) fn(_27) fn(_28) fn(_29)
#define JCORE_APPLY_FN_30(fn,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30) \
    fn(_1) fn(_2) fn(_3) fn(_4) fn(_5) fn(_6) fn(_7) fn(_8) fn(_9) fn(_10) fn(_11) fn(_12) fn(_13) fn(_14) fn(_15) fn(_16) fn(_17) fn(_18) fn(_19) fn(_20) fn(_21) fn(_22) fn(_23) fn(_24) fn(_25) fn(_26) fn(_27) fn(_28) fn(_29) fn(_30)

#define JCORE_APPLY_FN(fn, ...)															\
	JCORE_EXPAND_1(																		\
		JCORE_CONCAT_3(																	\
			JCORE_APPLY_FN_, JCORE_ARGS_COUNT(__VA_ARGS__), (fn, __VA_ARGS__)			\
		)																				\
	)

