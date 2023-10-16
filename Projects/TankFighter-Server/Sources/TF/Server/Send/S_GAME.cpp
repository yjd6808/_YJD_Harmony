/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 11:20:15 AM
 * =====================
 *
 */



#include "Pch.h"
#include "S_GAME.h"

#include <TF/Common/Command.h>

#include <TF/Server/Query/Q_GAME.h>

USING_NS_JC;

void S_GAME::SEND_SC_Login(bool success, int accountPrimaryKey) {
	auto sending = SendBegin<SC_Login>();
	sending.Cmd.Result = success;
	sending.Cmd.AccountPrimaryKey = accountPrimaryKey;
}

void S_GAME::SEND_SC_Register(bool success) {
	auto sending = SendBegin<SC_Register>();
	sending.Cmd.Result = success;
}

void S_GAME::SEND_SC_LoadChannelInfo(ChannelInfo* channelInfo, int count) {
	auto sending = SendBegin<SC_LoadChannelInfo>(count);
	for (int i = 0; i < count; ++i) {
		sending.Cmd.Info[i] = channelInfo[i];
	}
}

void S_GAME::SEND_SC_SelectChannel(int channelPrimaryKey) {
	auto sending = SendBegin<SC_SelectChannel>();
	sending.Cmd.ChannelPrimaryKey = channelPrimaryKey;
}

void S_GAME::SEND_SC_LeaveChannel() {
	auto sending = SendBegin<SC_LeaveChannel>();
}

void S_GAME::SEND_SC_LoadCharacterInfo(int accountPrimaryKey, int channelPrimaryKey) {
	Qry::SelectCharacterInfoListResult selectQryResult = Q_GAME::SelectCharacterInfoList(accountPrimaryKey, channelPrimaryKey);
	Vector<CharacterInfo> vCharacterInfoList(selectQryResult.RowCount);

	if (selectQryResult.HasBindedResult) {
		do {
			CharacterInfo info;
			info.PrimaryKey = selectQryResult.PrimaryKey;
			info.Name = selectQryResult.Name;
			info.Win = selectQryResult.Win;
			info.Lose = selectQryResult.Lose;
			info.Kill = selectQryResult.Kill;
			info.Death = selectQryResult.Death;
			info.Money = selectQryResult.Money;
			vCharacterInfoList.PushBack(Move(info));
		} while (selectQryResult.FetchNextRow());
	}
	
	int iCount = vCharacterInfoList.Size();
	auto sending = SendBegin<SC_LoadCharacterInfo>(iCount);
	for (int i = 0; i < iCount; ++i) {
		sending.Cmd.Info[i] = vCharacterInfoList[i];
	}
}

void S_GAME::SEND_SC_SelectCharacter(const CharacterInfo& info) {
	auto sending = SendBegin<SC_SelectCharacter>();
	sending.Cmd.info = info;
}

void S_GAME::SEND_SC_ServerMessage(const char* msg) {
	const int iLen = StringUtil::LengthWithNull(msg);
	auto sending = SendBegin<SC_ServerMessage>(iLen);
	sending.Cmd.Count = iLen;
	sending.Cmd.Message.SetStringUnsafe(msg);
}

void S_GAME::SEND_SC_ServerMessage(const String& msg) {
	const int len = msg.LengthWithNull();
	auto sending = SendBegin<SC_ServerMessage>(len);
	sending.Cmd.Count = len;
	sending.Cmd.Message.SetStringUnsafe(msg);
}
