/*
 *	작성자 : 윤정도
 */

#pragma once

#include <JCore/Type.h>
#include <JCore/Primitives/String.h>

#include <JNetwork/Namespace.h>

NS_JNET_BEGIN

struct Winsock
{
	static bool			Initialize(Byte _highVersion, Byte _lowVersion);
	static bool			IsInitialized() { return Initialized; }
	static bool			Finalize();
	static bool			IsFinalized() { return Finalized; }
	static Int32U		LastError();
	static JCore::String	LastErrorMessageUTF8();
	static JCore::String	ErrorMessageMBCS(Int32U _errorCode);
	static JCore::String	ErrorMessageUTF8(Int32U _errorCode);
private:
	static bool Initialized;
	static bool Finalized;
};

NS_JNET_END