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


UI_Test::UI_Test(UIGroupInfo* groupInfo)
	: UIMasterGroup(groupInfo)
	, m_pGroupO1(nullptr)
	, m_pGroupO2(nullptr)
	, m_fScale(0.0f)
	, m_eScaleState(eNone)
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
		//m_pGroupO1->setScale(m_fScale, m_fScale);
		//m_pGroupO2->setScale(m_fScale, m_fScale);
	} else {
		m_fScale -= dt;

		if (m_fScale < 1.0f)
			return;
		m_pGroupO1->setContentSize(m_GroupO1DefaultSize * m_fScale);
		m_pGroupO2->setContentSize(m_GroupO2DefaultSize * m_fScale);
		//m_pGroupO1->setScale(m_fScale, m_fScale);
		//m_pGroupO2->setScale(m_fScale, m_fScale);
	}

	
}

void UI_Test::onToggleStateChanged(UIToggleButton* toggleBtn, ToggleState state) {
	if (toggleBtn->getCode() == UI_TEST_TOGGLEBUTTON_T1) {
		if (state == ToggleState::eToggled) {
			m_fScale = 1.0f;
			m_eScaleState = eIncrease;
		} else {
			m_fScale = 1.25f;
			m_eScaleState = eDecrease;
		}
	}

}
