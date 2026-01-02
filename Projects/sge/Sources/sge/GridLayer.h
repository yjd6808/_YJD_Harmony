/*
	생성일 : 2019/03/05
	작성자 : 윤정도
 */

#ifndef  _DFGRID_VIWER_H__
#define  _DFGRID_VIWER_H__
#include <cocos2d.h>

class GridLayer : public cocos2d::Layer
{
private:
	enum LabelTag
	{
		GridMousePosition,
		TouchMousePosition,
		MouseMovePosition
	};
public:
	enum GridEvent
	{
		NoOption,				//아예 안보여줌
		ShowGrid,				//그리드 만 보여줌
		ShowMousePoint,			//마우스 좌표만 보여줌
		ShowGridAndMousePoint		//그리드와 마우스좌표 모두 보여줌
	};
public:
	GridLayer(const int _interval, const GridEvent _userGridEvent);
	~GridLayer() override;
private:
	int interval_;
	GridEvent userGridEvent_ = GridEvent::NoOption;
	cc::DrawNode* drawNode_;
public:
	void DrawGridWindow(const int _interval, const cocos2d::Color4F& _color);
	static GridLayer* create(const int _interval, const cocos2d::Color4F& _color, const GridEvent _userGridEvent);

	virtual bool InitWithParams(const int _interval, const cocos2d::Color4F& _color);
	virtual bool OnMouseMove(cocos2d::Event* _pEvent);
	virtual bool onTouchBegan(cocos2d::Touch* _pTouch, cocos2d::Event* _pEvent);
	virtual void onTouchEnded(cocos2d::Touch* _pTouch, cocos2d::Event* _pEvent);

	void onExit() override;
};

#endif //_DFGRID_VIWER_H__
