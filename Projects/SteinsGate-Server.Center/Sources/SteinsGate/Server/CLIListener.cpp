/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:26:28 AM
 * =====================
 *
 */


#include "Center.h"
#include "CenterCoreHeader.h"
#include "CLIListener.h"


bool CLIListener::OnInputProcessing(int argc, JCore::String* argv) {
	if (!CLIListenerCommon::OnInputProcessing(argc, argv)) {
		return false;
	}

	return true;
}
