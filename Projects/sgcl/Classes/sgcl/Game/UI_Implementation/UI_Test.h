/*
 * 작성자: 윤정도
 * =====================
 * sgui 테스트 화면 (코드 기반 재구현)
 */

#pragma once

#include "sgcl/Game/UI/Sgui.h"

class UI_Test : public sgui::Window
{
public:
	enum ScaleState
	{
		eIncrease,
		eDecrease
	};

	virtual void OnInit(const jc::CDataMap<>& _param) override;
	virtual void OnLoaded() override;
	virtual void OnUpdate(float _dt) override;

private:
	void OnPopupTest();
	void OnScaleTest();
	void OnFontTest();
	void OnDragTest();

private:
	sgui::Border* pGroup01_ = nullptr;
	sgui::Border* pGroup02_ = nullptr;

	cc::size groupO1DefaultSize_;
	cc::size groupO2DefaultSize_;

	int fontCode_ = 1;
	jc::Vector<cc::Label*> testLabelList_;
	bool testFont_ = false;

	float scale_ = 0.0f;
	ScaleState scaleState_ = eDecrease;
};
