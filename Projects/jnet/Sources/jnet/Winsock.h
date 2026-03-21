/*
 *	작성자 : 윤정도
 */

#pragma once

#include "jc/Type.h"
#include "jc/Primitives/String.h"

#include "jnet/Namespace.h"

NS_JNET_BEGIN

struct Winsock
{
	static bool			Initialize(_u8 _highVersion, _u8 _lowVersion);
	static bool			IsInitialized() { return Initialized; }
	static bool			Finalize();
	static bool			IsFinalized() { return Finalized; }
	static _u32		LastError();
	static jc::String	LastErrorMessageUTF8();
	static jc::String	ErrorMessageMBCS(_u32 _errorCode);
	static jc::String	ErrorMessageUTF8(_u32 _errorCode);
private:
	static bool Initialized;
	static bool Finalized;
};

NS_END