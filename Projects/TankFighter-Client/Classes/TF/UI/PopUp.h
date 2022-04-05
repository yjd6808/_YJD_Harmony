/*
	생성일 : 2019/03/05
	작성자 : 윤정도
 */

#ifndef _DF_POPUP_H__
#define _DF_POPUP_H__

#include <TF/SourceHeader.h>

class PopUp : public Layer
{
public:
	enum Type
	{
		CONFIRM,
		YESNO
	};
private:
	CC_SYNTHESIZE(int,						popUpKey,			Key);
	CC_SYNTHESIZE(Type,						popUpType,			Type);
	CC_SYNTHESIZE(std::string,				popUpText,			Text);
	CC_SYNTHESIZE(cocos2d::Node*,			popUpParent,		Parent);
	CC_SYNTHESIZE(std::function<void()>,	popUpConfirmFunc,	Confirm);
	CC_SYNTHESIZE(std::function<void()>,	popUpYesFunc,		Yes);
	CC_SYNTHESIZE(std::function<void()>,	popUpNoFunc,		No);

	Vec2 CENTER		= { 0.5f, 0.5f };
	Vec2 ZERO		= { 0.0f, 0.0f };

public:
	static  PopUp* createInParent(const char* text, Node* paren, bool bgClickable, std::function<void()> callback = nullptr);
	static  PopUp* createInParent(const char* text, Node* parent, bool bgClickable, std::function<void()> yesFunc, std::function<void()> noFunc);

	bool init(bool bgClickable);
	bool init() override;

	CREATE_FUNC(PopUp);
	
};

#endif // _DF_POPUP_H__
