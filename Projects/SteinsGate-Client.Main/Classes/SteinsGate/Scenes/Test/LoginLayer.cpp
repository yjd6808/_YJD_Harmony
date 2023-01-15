/*
 * 작성자 : 윤정도
 */

#include <SteinsGate/Scenes/Test/LoginLayer.h>

using namespace cocos2d;
using namespace cocos2d::ui;

Node* a;
DrawNode* b;
Node* c;
JCore::Vector<Node*> TestNodes;
bool LoginLayer::init() {

	if (!Layer::init()) {
		return false;
	}

	a = Sprite::create("rect.png");
	a->setContentSize({ 300, 300 });
	a->setColor(Color3B::GRAY);
	a->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	a->setPosition({ 600, 300.0f });
	this->addChild(a);

	Vec2 poly[4];
	poly[0] = { 0.0f, 60.0f };
	poly[1] = { 100.0f, 60.0f };
	poly[2] = { 100, 0 };
	poly[3] = { 0, 0 };

	b = DrawNode::create();
	b->drawPolygon(poly, 4, {}, 1, Color4F::WHITE);
	b->setContentSize({ 100, 60.0f });
	b->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);		// 0.0, 1.0
	a->addChild(b);

	Rect r = b->getBoundingBox();
	printf("x = %d, y = %d, width = %d, height = %d\n", 
		int(r.origin.x), int(r.origin.y), int(r.size.width), int(r.size.height));
	r = a->getBoundingBox();
	printf("x = %d, y = %d, width = %d, height = %d\n", 
		int(r.origin.x), int(r.origin.y), int(r.size.width), int(r.size.height));

	/*c = Sprite::create("rect3.png");
	c->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
	c->setPosition({ 200.0f, 200.0f });
	b->addChild(c);

	auto c1 = Sprite::create("rect6.png");
	auto c2 = Sprite::create("rect6.png");
	auto c3 = Sprite::create("rect6.png");

	c1->setPosition(Vec2{ 50.0f, 50.0f });
	c2->setPosition(Vec2{ 50.0f, 50.0f });
	c3->setPosition(Vec2{ 100.0f, 50 });

	a->addChild(c1);
	b->addChild(c2);
	c->addChild(c3);

	Vec2 v1;
	Vec2 v2;
	Vec2 v3;
	Vec2 v4;

	printf("키 입력됨\n");

	v3 = a->convertToWorldSpace(b->getPosition());
	v4 = a->convertToWorldSpaceAR(b->getPosition());

	printf("a->convertToWorldSpace(b->getPosition())   %d, %d\n", (int)v3.x, (int)v3.y);
	printf("a->convertToWorldSpaceAR(b->getPosition()) %d, %d\n", (int)v4.x, (int)v4.y);
	printf("\n===================================================\n\n");

	v3 = b->convertToWorldSpace(c->getPosition());
	v4 = b->convertToWorldSpaceAR(c->getPosition());

	printf("b->convertToWorldSpace(c->getPosition())   %d, %d\n", (int)v3.x, (int)v3.y);
	printf("b->convertToWorldSpaceAR(c->getPosition()) %d, %d\n", (int)v4.x, (int)v4.y);
	printf("\n===================================================\n\n");

	*/
	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(LoginLayer::onKeyPressed2, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(LoginLayer::onKeyReleased2, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);


	this->scheduleUpdate();
	return true;
}
bool bKeyPressed = false;
void LoginLayer::update(float delta) {
	if (bKeyPressed) {
		Rect r = b->getBoundingBox();
		printf("x = %d, y = %d, width = %d, height = %d\n", int(r.origin.x), int(r.origin.y), int(r.size.width), int(r.size.height));
	}
}



void LoginLayer::onKeyPressed2(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_A)
		bKeyPressed = true;

	if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
		for (int i = 0; i < TestNodes.Size(); i++) {
			TestNodes[i]->setPosition(TestNodes[i]->getPositionX() - 30.1f, TestNodes[i]->getPositionY());;
		}
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
		for (int i = 0; i < TestNodes.Size(); i++) {
			TestNodes[i]->setPosition(TestNodes[i]->getPositionX() + 30.1f, TestNodes[i]->getPositionY());;
		}
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW) {
		for (int i = 0; i < TestNodes.Size(); i++) {
			TestNodes[i]->setPosition(TestNodes[i]->getPositionX(), TestNodes[i]->getPositionY() + 30.1f);
		}
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
		for (int i = 0; i < TestNodes.Size(); i++) {
			TestNodes[i]->setPosition(TestNodes[i]->getPositionX(), TestNodes[i]->getPositionY() - 30.1f);
		}
	}
}

void LoginLayer::onKeyReleased2(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_A)
		bKeyPressed = false;


}
