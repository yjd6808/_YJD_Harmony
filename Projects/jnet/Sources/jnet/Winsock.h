/*
 *	작성자 : 윤정도
 */

#pragma once

#include <jc/Type.h>
#include <jc/Primitives/String.h>

#include <jnet/Namespace.h>

NS_JNET_BEGIN

struct Winsock
{
	static bool			Initialize(Byte _highVersion, Byte _lowVersion);
	static bool			IsInitialized() { return Initialized; }
	static bool			Finalize();
	static bool			IsFinalized() { return Finalized; }
	static Int32U		LastError();
	static jc::String	LastErrorMessageUTF8();
	static jc::String	ErrorMessageMBCS(Int32U _errorCode);
	static jc::String	ErrorMessageUTF8(Int32U _errorCode);
private:
	static bool Initialized;
	static bool Finalized;
};

NS_END