/*
 * 작성자: 윤정도
 * 생성일: 2/22/2023 7:58:58 PM
 * =====================
 *
 */

#pragma once

#include <sgcl/UIElement.h>

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
	cc_ui::EditBox* Source() const { return pEditBox_; }
	jc::String ToString() override { return jc::StringUtil::Format("에딧박스(%d)", pInfo_->code_); }

	void SetInitialUISize(cc::size _size) override;
	void SetMaxLength(int _maxLength);
	void SetTextEditBeginCallback(const jc::Action<UIEditBox*>& _fnTextEditBegin) const;
	void SetTextChangedCallback(const jc::Action<UIEditBox*, const jc::String&>& _fnTextChanged) const;
	void SetReturnCallback(const jc::Action<UIEditBox*>& _fnEditBoxReturn) const;
	void SetLoseFocusCallback(const jc::Action<UIEditBox*, cc_ui::EditBoxDelegate::EditBoxEndAction>& _fnLoseFocus) const;
	void SetUISize(const cc::size& _size) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void SetInfoEditBox(UIEditBoxInfo* _pInfo, bool _infoOwner);

	void Focus() override;
	void SetInputFlag(cc_ui::EditBox::InputFlag _inputFlag);
	void SetInputMode(cc_ui::EditBox::InputMode _inputMode);
	// ANY 모드에서는 VK_RETURN이 안먹히므로 오류라고 생각하지 말것
protected:
	bool OnMouseUpContainedInternalDetail(cc::EventMouse* /*_pMouseEvent*/) override;

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
		void editBoxEditingDidBegin(cc_ui::EditBox* /*editBox*/) override;

		void nativeEditBoxFocused(cc_ui::EditBox* /*editBox*/) override;

		/**
		 * This method is called when the edit box text was changed.
		 * @param editBox The edit box object that generated the event.
		 * @param text The new text.
		 */
		void editBoxTextChanged(cc_ui::EditBox* /*editBox*/, const std::string& /*text*/) override;

		/**
		 * This method is called when the return button was pressed or the outside area of keyboard was touched.
		 * @param editBox The edit box object that generated the event.
		 */
		void editBoxReturn(cc_ui::EditBox* /*_pEditBox*/) override;

		/**
		 * This method is called when an edit box loses focus after keyboard is hidden.
		 * @param editBox The edit box object that generated the event.
		 * @param type The reason why editing ended.
		 */
		void editBoxEditingDidEndWithAction(cc_ui::EditBox* /*editBox*/, EditBoxEndAction /*action*/) override;

		jc::Action<UIEditBox*> fnEditBoxEditingDidBegin_;
		jc::Action<UIEditBox*, const jc::String&> fnEditBoxTextChanged_;
		jc::Action<UIEditBox*> fnEditBoxReturn_;
		jc::Action<UIEditBox*, cc_ui::EditBoxDelegate::EditBoxEndAction> fnEditBoxEditingDidEndWithAction_;
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
	cc::Label* pLabel_{};
	cc::Label* pLabelPlaceHolder_{};
	cc_ui::EditBoxImplWin* pEditBoxImpl_{};
	cc_ui::EditBox* pEditBox_;
	Listener* pListener_;
};
