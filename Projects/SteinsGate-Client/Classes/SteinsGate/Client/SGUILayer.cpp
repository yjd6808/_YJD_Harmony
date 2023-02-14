/*
 * 작성자: 윤정도
 * 생성일: 2/14/2023 10:12:23 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGUILayer.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;


// ==============================================================================================================================
// 
// ==============================================================================================================================

SGUILayer* SGUILayer::create() {
	SGUILayer* pUI = dbg_new SGUILayer;

	if (pUI && pUI->init()) {
		pUI->autorelease();
		return pUI;
	}

	DeleteSafe(pUI);
	return nullptr;
}
