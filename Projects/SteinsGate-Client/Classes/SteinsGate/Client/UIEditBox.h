/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 7:58:58 PM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/UIElement.h>

class UIEditBox : public UIElement
{
public:
	UIEditBox(UIRootGroup* _pRoot, UIGroup* _pParent);
	UIEditBox(UIRootGroup* _pRoot, UIGroup* _pParent, UIEditBoxInfo* _pEditBoxInfo, bool _infoOwner);
	~UIEditBox() override;

	static UIEditBox* Create(UIRootGroup* _pRoot, UIGroup* _pParent);
	static UIEditBox* Create(UIRootGroup* _pRoot, UIGroup* _pParent, UIEditBoxInfo* _pEditBoxInfo, bool _infoOwner);

	static constexpr UIElementType_t Type() { return UIElementType::EditBox; }

	bool init() override;
	UIElementType_t GetElementType() override { return UIElementType::EditBox; }
	std::string GetText();
	const char* GetTextRaw();
	SGEditBox* Source() const { return pEditBox_; }
	SGString ToString() override { return SGStringUtil::Format("에딧박스(%d)", pInfo_->code_); }

	void SetInitialUISize(SGSize _size) override;
	void SetMaxLength(int _maxLength);
	void SetTextEditBeginCallback(const SGActionFn<UIEditBox*>& _fnTextEditBegin) const;
	void SetTextChangedCallback(const SGActionFn<UIEditBox*, const SGString&>& _fnTextChanged) const;
	void SetReturnCallback(const SGActionFn<UIEditBox*>& _fnEditBoxReturn) const;
	void SetLoseFocusCallback(const SGActionFn<UIEditBox*, SGEditBoxEndAction>& _fnLoseFocus) const;
	void SetUISize(const SGSize& _size) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void SetInfoEditBox(UIEditBoxInfo* _pInfo, bool _infoOwner);

	void Focus() override;
	void SetInputFlag(SGEditBox::InputFlag _inputFlag);
	void SetInputMode(SGEditBox::InputMode _inputMode);
	// ANY 모드에서는 VK_RETURN이 안먹히므로 오류라고 생각하지 말것
protected:
	bool OnMouseUpContainedInternalDetail(SGEventMouse* /*_pMouseEvent*/) override;

private:
	struct Listener : cocos2d::ui::EditBoxDelegate
	{
		Listener(UIEditBox* _pEditBox)
		: pEditBox_(_pEditBox)
		{
		}

		/**
		  * 내가 이번 프로젝트를 하면서 자주보게되는 솔라시스템이 EditBox 다루는 영상을 참고해서 추가함
		  * https://www.youtube.com/watch?v=fzeLTisvDNU&ab_channel=SonarSystems
		  *
		  * This method is called when an edit box gains focus after keyboard is shown.
		  * @param editBox The edit box object that generated the event.
		  */
		void editBoxEditingDidBegin(SGEditBox* /*editBox*/) override;

		void nativeEditBoxFocused(SGEditBox* /*editBox*/) override;

		/**
		 * This method is called when the edit box text was changed.
		 * @param editBox The edit box object that generated the event.
		 * @param text The new text.
		 */
		void editBoxTextChanged(SGEditBox* /*editBox*/, const std::string& /*text*/) override;

		/**
		 * This method is called when the return button was pressed or the outside area of keyboard was touched.
		 * @param editBox The edit box object that generated the event.
		 */
		void editBoxReturn(SGEditBox* /*_pEditBox*/) override;

		/**
		 * This method is called when an edit box loses focus after keyboard is hidden.
		 * @param editBox The edit box object that generated the event.
		 * @param type The reason why editing ended.
		 */
		void editBoxEditingDidEndWithAction(SGEditBox* /*editBox*/, EditBoxEndAction /*action*/) override;

		SGActionFn<UIEditBox*> fnEditBoxEditingDidBegin_;
		SGActionFn<UIEditBox*, const SGString&> fnEditBoxTextChanged_;
		SGActionFn<UIEditBox*> fnEditBoxReturn_;
		SGActionFn<UIEditBox*, SGEditBoxEndAction> fnEditBoxEditingDidEndWithAction_;
		UIEditBox* pEditBox_;
	};

private:
	bool isFontAutoScaling_;
	float fontSizeInitial_;
	float fontSize_;
	float placeholderFontSizeInitial_;
	float placeholderFontSize_;

	HWND nativeHandle_{};
	UIEditBoxInfo* pInfo_;
	SGLabel* pLabel_{};
	SGLabel* pLabelPlaceHolder_{};
	SGEditBoxImplWin* pEditBoxImpl_{};
	SGEditBox* pEditBox_;
	Listener* pListener_;
};
