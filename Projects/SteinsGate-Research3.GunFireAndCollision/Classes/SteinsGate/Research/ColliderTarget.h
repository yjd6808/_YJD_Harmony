/*
 * 작성자: 윤정도
 * 생성일: 1/15/2023 10:39:16 PM
 * =====================
 * 테스트용이니까 대충 암거나 만듬
 * 다음 리서치에서 구조 개편해야할 듯
 * 추후 추상클래스로 다룰예정
 */




#pragma once

#include <SteinsGate/Research/Tutturu.h>




class Collider;
class ColliderTarget : public cocos2d::Sprite {	// Node
public:
	static ColliderTarget* create();

	bool init() override;

	cocos2d::Node* getCanvas() { return m_pCanvas; }
	cocos2d::DrawNode* getSkinBoundingBox() { return m_pSkinBoundingBox; }
	cocos2d::Size getCanvasSize() { return m_pCanvas->getContentSize(); }
	cocos2d::Sprite* getSkinPart() { return m_pSkinPart; }

protected:
	Node* m_pCanvas;
	Sprite* m_pSkinPart;
	cocos2d::DrawNode* m_pSkinBoundingBox;
};


