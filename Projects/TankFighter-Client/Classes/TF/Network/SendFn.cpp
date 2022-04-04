#include <TF/Network/SendFn.h>
#include <TF/Network/GameClient.h>
#include <Common/Command.h>

#include <cocos2d.h>

using namespace JCore;
using namespace JNetwork;
using namespace cocos2d;

#define _Client GameClient::GetInstance()

void SendFn::SendLoadChannelInfoSyn() {
	auto* pPacket = new Packet<LoadChannelInfoSyn>();
	_Client->SendAsync(pPacket);
}

void SendFn::SendChannelSelectSyn(int selectedChannelUID) {
	auto* pPacket = new Packet<SelectChannelSyn>();
	pPacket->Get<0>()->ChanneldUID = selectedChannelUID;
	_Client->SendAsync(pPacket);
}

void SendFn::SendCreateCharacterSyn(std::string& nick) {
}

void SendFn::SendDeleteCharacterSyn(std::string& nick) {
}

void SendFn::SendLoadCharacterInfoSyn() {
	auto* pPacket = new Packet<LoadCharacterInfoSyn>();
	LoadCharacterInfoSyn* pLoadCharacterInfoSyn = pPacket->Get<0>();
	pLoadCharacterInfoSyn->ChannelUID = _Client->GetAccountUID();
	pLoadCharacterInfoSyn->AccountUID = _Client->GetChannelUID();
	_Client->SendAsync(pPacket);
}

void SendFn::SendSelectCharacterSyn(const int selectedCharacterUID) {
	auto* pPacket = new Packet<SelectCharacterSyn>();
	SelectCharacterSyn* pSelectCharacterSyn = pPacket->Get<0>();
	pSelectCharacterSyn->AccountUID = _Client->GetAccountUID();
	pSelectCharacterSyn->ChannelUID = _Client->GetAccountUID();
	pSelectCharacterSyn->CharacterUID = _Client->();
}

