/*
 * 작성자: 윤정도
 * 생성일: 4/27/2023 10:51:41 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UI_Test.h"

#include <SteinsGate/Client/UIDefine.h>

USING_NS_CC;
USING_NS_JC;

UI_Test::UI_Test(UIGroupInfo* groupInfo)
	: UIMasterGroup(groupInfo)
	, m_pGroupO1(nullptr)
	, m_pGroupO2(nullptr)
	, m_iFontCode(1)
	, m_fScale(0.0f)
	, m_eScaleState(eDecrease)
{}

void UI_Test::onInit() {

	
}

void UI_Test::onLoaded() {
	m_pGroupO1 = CoreUIManager_v->getGroup(UI_TEST_GROUP_O1);
	m_pGroupO2 = CoreUIManager_v->getGroup(UI_TEST_GROUP_O2);

	m_GroupO1DefaultSize = m_pGroupO1->getContentSize();
	m_GroupO2DefaultSize = m_pGroupO2->getContentSize();

}

void UI_Test::onUpdate(float dt) {
	if (m_eScaleState == eIncrease) {
		m_fScale += dt;

		if (m_fScale > 1.25f)
			return;

		
		m_pGroupO1->setContentSize(m_GroupO1DefaultSize * m_fScale);
		m_pGroupO2->setContentSize(m_GroupO2DefaultSize * m_fScale);
	} else {
		m_fScale -= dt;

		if (m_fScale < 1.0f)
			return;

		m_pGroupO1->setContentSize(m_GroupO1DefaultSize * m_fScale);
		m_pGroupO2->setContentSize(m_GroupO2DefaultSize * m_fScale);
	}

	
}

void UI_Test::onToggleStateChanged(UIToggleButton* toggleBtn, ToggleState state) {
	
}

void UI_Test::onMouseUp(UIElement* element, SGEventMouse* mouseEvent) {

	if (element->getCode() == UI_TEST_LABEL_POPUP_TEST) {
		CorePopupManager_v->showOk("안녕하세요");
	}

	else if (element->getCode() == UI_TEST_LABEL_SCALE_TEST) {
		if (m_eScaleState == eDecrease) {
			m_fScale = 1.0f;
			m_eScaleState = eIncrease;
		} else {
			m_fScale = 1.25f;
			m_eScaleState = eDecrease;
		}
	}

	else if (element->getCode() == UI_TEST_LABEL_FONT_TEST) {

		for (int i = 0; i < m_vTestLabelList.Size(); ++i) {
			CoreWorld_v->removeChild(m_vTestLabelList[i]);
		}
		m_vTestLabelList.Clear();


		// 1: DF.ttf
		// 2: DM.ttf
		// 3: DMB.ttf
		// 4: DML.ttf
		// 5: N2G.ttf
		// 6: N2GB.ttf
		// 7: N2GM.ttf

		float fHeight = 0;
		const SGString fontName = CoreFont_v->getFontName(m_iFontCode);
		const SGString fontPath = Path::Combine(CoreCommonInfo_v->DataPath, FontDirName_v, fontName);
		std::string txt;

		_LogDebug_("폰트이름: %s", fontName.Source());
		for (int i = 10; i <= 32; ++i) {
			txt = StringUtils::format("안녕하세요 %d", i);
			auto lb = Label::createWithTTF(txt, fontPath.Source(), i, Size::ZERO);
			auto atlas = lb->getFontAtlas();
			lb->setPosition(150.0f, fHeight);
			lb->setAnchorPoint(Vec2::ZERO);
			_LogDebug_("txt: %s, lineHeight: %.f, width: %.f", txt.data(), lb->getLineHeight(), lb->getWidth());
			fHeight += i;	// 폰트 크기가 곧 라벨 높이
			CoreWorld_v->addChild(lb);
			m_vTestLabelList.PushBack(lb);
		}

		float fSize = 14;
		txt = StringUtils::format("안녕하세요 1234567890123456789\n안녕하세요 1\n안녕하세요 1\n안녕하세요 1");
		auto lb = Label::createWithTTF(txt, fontPath.Source(), fSize, {100, 0});
		lb->setPosition(350.0f, 200);
		lb->setAnchorPoint(Vec2::ZERO);
		lb->setLineHeight(fSize);
		CoreWorld_v->addChild(lb);
		m_vTestLabelList.PushBack(lb);

		m_iFontCode++;

		_LogDebug_("라인 너비 벡터 크기: %d", lb->getLinesWidth().size());
		_LogDebug_("라인 수: %d", lb->getStringNumLines());
		

		if (m_iFontCode >= CoreFont_v->getFontCount()) {
			m_iFontCode = 1;
		}
	}
}