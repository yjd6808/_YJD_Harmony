/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:30:51 PM
 * =====================
 *
 */


#pragma once

#include <TF/Client/Game/Scene/BaseScene.h>

struct CharacterInfo;
struct RoomInfo;

class TextButton;
class ChatBox;
class ColoredListView;

class LobbyScene : public BaseScene
{
public:
	LobbyScene();
	~LobbyScene() override;
	

	CREATE_FUNC(LobbyScene)

	bool init() override;
	void onEnterTransitionDidFinish() override;

	void onClickedCreateRoomButton(TextButton* btn);				// 방생성	
	void onClickedTerminateGameButton(TextButton* btn);				// 게임 종료 
	void onClickedSelectChannelButton(TextButton* btn);				// 채널 선택
	void onClickedSelectCharacterButton(TextButton* btn);			// 캐릭터 선택
	void onClickedJoinRoomButton(TextButton* btn);					// 방 참가
	void onClickedFriendListButton(TextButton* btn);				// 친구 목록 클릭시
	void onClickedAddFriendButton(TextButton* btn);					// 친구 추가
	void onClickedDeleteFriendButton(TextButton* btn);				// 친구 제거
	void onClickedMyInfoButton(TextButton* btn);					// 내 정보 클릭시
	void onClickedChatSendButton(ChatBox* chatBox);					// 채팅 전송

	void refreshPlayerList(CharacterInfo* characterList, int count);
	void refreshCharacterInfo(const CharacterInfo& info);
	void refreshFriendList(FriendCharacterInfo* characterList, int count);
	void refreshRoomList(RoomInfo* roomList, int count);
	void addChatMssage(const char* str);

	Type getType() const override { return Type::Login; }
private:
	ChatBox* m_pChatBox;
	TextButton* m_pRoomListText;
	TextButton* m_pPlayerListText;
	ColoredListView* m_pRoomListView;
	ColoredListView* m_pPlayerListView;
	cocos2d::ui::EditBox* m_pRoomTitleEditBox;
	TextButton* m_pCreateRoomBtn;
	cocos2d::ui::EditBox* m_pFriendNameEditBox;
	TextButton* m_AddFriendNameButton;
	TextButton* m_pFriendListText;
	ColoredListView* m_pFriendListView;
	TextButton* m_pTerminateGameButton;
	TextButton* m_pSelectChannelButton;
	TextButton* m_pSelectCharacterButton;
	TextButton* m_pNyInfoButton;
};