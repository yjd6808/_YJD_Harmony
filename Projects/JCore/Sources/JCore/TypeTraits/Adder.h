/*
 * 작성자: 윤정도
 * 생성일: 12/16/2022 12:53:30 PM
 * =====================
 *
 */


#pragma once

namespace JCore {

	namespace Detail
	{
		template <typename T>
		struct AddLValueReference
		{
			using Type = T&;
		};

		template <typename T>
		struct AddRValueReference
		{
			using Type = T&&;
		};
	}


	template <typename T>
	using AddLValueRef_t = typename Detail::AddLValueReference<T>::Type;

	template <typename T>
	using AddRValueRef_t = typename Detail::AddRValueReference<T>::Type;


} // namespace JCore