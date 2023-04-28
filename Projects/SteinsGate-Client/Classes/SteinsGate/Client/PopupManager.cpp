/*
 * 작성자: 윤정도
 * 생성일: 4/26/2023 12:33:39 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "PopupManager.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

PopupManager::PopupManager()
	: m_PopupSize(150, 150)
	, m_vPopupList(8)
{}

PopupManager::~PopupManager() {}

void PopupManager::setPopupSize(float width, float height) {
	m_PopupSize.width = width;
	m_PopupSize.height = height;
}

void PopupManager::showYesNo(const std::string& text, const SGActionFn<>& yes, const SGActionFn<>& no) {

}

void PopupManager::showOk(const std::string& text, const SGActionFn<>& ok) {

}
