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
	UIEditBox(UIMasterGroup* maserGroup, UIGroup* parent, UIEditBoxInfo* editBoxInfo);
	~UIEditBox() override;

	bool init() override;
	static UIEditBox* create(UIMasterGroup* master, UIGroup* parent, UIEditBoxInfo* editBoxInfo);
	UIElementType_t getElementType() override { return UIElementType::EditBox; }
    SGString toString() override { return SGStringUtil::Format("에딧박스(%d)", m_pInfo->Code); }

    void setTextEditBeginCallback(const SGActionFn<UIEditBox*>& fnTextEditBegin) const;
    void setTextChangedCallback(const SGActionFn<UIEditBox*, const SGString&>& fnTextChanged) const;
    void setReturnCallback(const SGActionFn<UIEditBox*>& fnEditBoxReturn) const;
    void setLoseFocusCallback(const SGActionFn<UIEditBox*, SGEditBoxEndAction>& fnLoseFocus) const;
    void focus() override;
private:
    struct Listener : cocos2d::ui::EditBoxDelegate
	{
        Listener(UIEditBox* editBox) : EditBox(editBox) {}
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
        void editBoxReturn(SGEditBox* editBox) override;

        /**
         * This method is called when an edit box loses focus after keyboard is hidden.
         * @param editBox The edit box object that generated the event.
         * @param type The reason why editing ended.
         */
        void editBoxEditingDidEndWithAction(SGEditBox* /*editBox*/, EditBoxEndAction /*action*/) override;


        SGActionFn<UIEditBox*> FnEditBoxEditingDidBegin;
        SGActionFn<UIEditBox*, const SGString&> FnEditBoxTextChanged;
        SGActionFn<UIEditBox*> FnEditBoxReturn;
        SGActionFn<UIEditBox*, SGEditBoxEndAction> FnEditBoxEditingDidEndWithAction;
        UIEditBox* EditBox;
	};
  
private:
    HWND m_hNativeHandle;
	UIEditBoxInfo* m_pInfo;
    SGLabel* m_pLabel;
    SGLabel* m_pLabelPlaceholder;
    SGEditBoxImplWin* m_pImpl;
	SGEditBox* m_pEditBox;
    Listener* m_pListener;
};


