/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 4:40:33 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/UIElement.h>

class UIStatic : public UIElement
{
public:
	static UIStatic* create(UIRootGroup* _pMaster, UIGroup* _pParent);
	static UIStatic* create(UIRootGroup* _pMaster, UIGroup* _pParent, UIStaticInfo* _pStaticInfo, bool _infoOwner);

	static constexpr UIElementType_t type() { return UIElementType::Static; }

	UIStatic(UIRootGroup* _pMaster, UIGroup* _pParent);
	UIStatic(UIRootGroup* _pMaster, UIGroup* _pParent, UIStaticInfo* _pStaticInfo, bool _infoOwner);
	~UIStatic() override;

	bool init() override;
	void Load() override;
	void Unload() override;

	void setDebugVisible(bool _visible);
	void SetUISize(const SGSize& _contentSize) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void setInfoStatic(UIStaticInfo* _pInfo, bool _infoOwner);

	UIStaticInfo* getInfo() const { return info_; }
	UIElementType_t GetElementType() override { return UIElementType::Static; }
	SGString ToString() override { return SGStringUtil::Format("스태틱(%d)", info_->code_); }

private:
	bool visible_;
	UIStaticInfo* info_;
	FrameTexture* debugTexture_;
	SGSprite* debugSprite_;
};
