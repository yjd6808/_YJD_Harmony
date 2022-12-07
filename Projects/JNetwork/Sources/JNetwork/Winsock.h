/*
 *	작성자 : 윤정도
 */

#pragma once

#include <JCore/Type.h>
#include <JCore/Primitives/String.h>

namespace JNetwork {

struct Winsock
{
	static bool				Initialize(Byte highVersion, Byte lowVersion);
	static bool				IsInitialized() { return ms_bInitailized; }
	static bool				Finalize();
	static bool				IsFinalized() { return ms_bFinalized; }
	static Int32U			LastError();
	static JCore::String	LastErrorMessage();
private:
	static bool ms_bInitailized;
	static bool ms_bFinalized;
};



}

