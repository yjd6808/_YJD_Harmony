/*
 * 작성자: 윤정도
 * 생성일: 12/17/2022 7:17:32 PM
 * =====================
 * 생성자 작업용
 */


#pragma once

namespace JCore {

	template <typename... TParam>
	struct ConstructorCall {

		ConstructorCall( TParam&&... params) {
		}
	};

} // namespace JCore