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
		NoOption,					//아예 안보여줌
		ShowGrid,					//그리드 만 보여줌
		ShowMousePoint,				//마우스 좌표만 보여줌
		ShowGridAndMousePoint		//그리드와 마우스좌표 모두 보여줌
	};
public:
	GridLayer(const int interval, const GridEvent userGridEvent);
	~GridLayer() override;
private:
	int interval;
	GridEvent userGridEvent = GridEvent::NoOption;
	SGDrawNode* m_pDrawNode;
public:
	void DrawGridWindow(const int interval, const cocos2d::Color4F & color);
	static GridLayer* create(const int interval, const cocos2d::Color4F & color, const GridEvent userGridEvent);

	virtual bool initWithParams(const int interval, const cocos2d::Color4F & color);
	virtual bool onMouseMove(cocos2d::Event*);
	virtual bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchEnded(cocos2d::Touch*, cocos2d::Event*);

	void onExit() override;
};

#endif //_DFGRID_VIWER_H__
