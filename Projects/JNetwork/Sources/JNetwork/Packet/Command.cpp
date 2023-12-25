/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 7:24:50 PM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/Packet/Command.h>


NS_JNET_BEGIN

ICommand* ICommand::Clone() const {
	ICommand * pCopy = (ICommand*)dbg_new char[CmdLen];
	JCore::Memory::CopyUnsafe(pCopy, this, CmdLen);	// 데이터영역 복사
	return pCopy;
}

NS_JNET_END