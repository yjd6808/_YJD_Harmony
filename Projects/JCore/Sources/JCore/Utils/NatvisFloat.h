#pragma once


// 23년 02월 06일 오전 11시
// https://stackoverflow.com/questions/31689857/natvis-floating-point-format
// 프리시전용 float 추가, 이 생각을 왜 못했을까 ㄷㄷ
namespace JCore {
	namespace Detail {
		struct NatvisFloat6 { float f{}; };
		struct NatvisFloat5 { float f{}; };
		struct NatvisFloat4 { float f{}; };
		struct NatvisFloat3 { float f{}; };
		struct NatvisFloat2 { float f{}; };
		struct NatvisFloat1 { float f{}; };

	}
}
