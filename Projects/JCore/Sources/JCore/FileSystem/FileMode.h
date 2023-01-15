/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 4:44:24 AM
 * =====================
 *
 */


#pragma once

namespace JCore {

	enum FileMode
	{
		eCreate,			// 기존에 존재할 경우 걍 덮어쓰고 새로 생성
		eOpen,			// 기존 파일을 연다.
		eAppend			// 이어서 작성용
	};

} // namespace JCore