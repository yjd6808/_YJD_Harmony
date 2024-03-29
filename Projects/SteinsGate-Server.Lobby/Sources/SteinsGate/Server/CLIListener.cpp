/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:26:28 AM
 * =====================
 *
 */


#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "CLIListener.h"


CLIListener::CLIListener() {
	m_Table.Insert("example",		JCORE_CALLBACK_2(CLIListener::CLI_Example, this));
}

bool CLIListener::OnInputProcessing(int argc, JCore::String* argv) {
	if (!CLIListenerCommon::OnInputProcessing(argc, argv)) {
		return false;
	}

	const TCLI_Callback* pCallback = m_Table.Find(argv[0].Source());

	if (pCallback) {
		return (*pCallback)(argc, argv);
	}

	return true;
}

bool CLIListener::CLI_Example(int argc, JCore::String* argv) {
	return true;
}
