/*
 * 작성자: 윤정도
 * 생성일: 12/18/2022 7:16:38 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/Vector.h>

namespace JCore {

	struct ThreadLocalCopy
	{
		ThreadLocalCopy(void* original): OriginalOwner(original) {}
		void* OriginalOwner;
		Vector<void*> Copies;
	};

} // namespace JCore