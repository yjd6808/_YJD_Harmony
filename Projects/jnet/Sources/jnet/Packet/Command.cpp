/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 7:24:50 PM
 * =====================
 *
 */


#include "jnet/Packet/Command.h"


NS_JNET_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
ICommand* ICommand::Clone() const
{
	ICommand* pCopy = reinterpret_cast<ICommand*>(dbg_new char[cmdLength_]);
	jc::Memory::CopyUnsafe(pCopy, this, cmdLength_); // 데이터영역 복사
	return pCopy;
}

NS_END
