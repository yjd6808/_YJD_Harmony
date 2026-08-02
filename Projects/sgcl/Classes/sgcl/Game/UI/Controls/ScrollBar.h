/*
 * 작성자: 윤정도
 * =====================
 * sgui - 수직 스크롤바 (WPF ScrollBar + 기존 UIScrollBar의 행(row) 단위 API 유지)
 * 위/아래 버튼 + 트랙 + Thumb로 구성되며, 행 위치 변경 시 ueScrollChanged를 발생시킨다.
 * SetLinkElement()로 연결된 요소의 휠 이벤트를 받아 스크롤한다.
 */

#pragma once

#include "sgcl/Game/UI/Controls/Control.h"

namespace sgui
{

class Button;
class Thumb;

class ScrollBar : public Control
{
public:
	static constexpr float MinThumbHeight_v = 8.0f;
	static constexpr float ButtonHeight_v = 16.0f;

	static ScrollBar* Create();

	// 행 단위 API (기존 UIScrollBar 호환)
	void SetRowCount(int _rowCount);
	int GetRowCount() const { return rowCount_; }

	void SetRowCountPerPage(int _rowCountPerPage);
	int GetRowCountPerPage() const { return rowCountPerPage_; }

	void SetRowPos(int _rowPos);
	int GetRowPos() const { return rowPos_; }
	int GetEndPos() const { return std::max(0, rowCount_ - rowCountPerPage_); }

	// 연결된 요소에서 휠 이벤트가 발생하면 이 스크롤바가 반응한다.
	void SetLinkElement(UIElement* _pLink);

	virtual int GetChildElementCount() const override { return 3; }
	virtual UIElement* GetChildElementAt(int _index) const override;

	virtual void OnEventRaised(int _id, UIEvent* _pEvent) override;
	virtual void RefreshThemeVisuals() override;

protected:
	ScrollBar() = default;

	virtual cc::size MeasureOverride(const cc::size& _constraint) override;
	virtual void ArrangeOverride(const cc::size& _finalSize) override;

	void BuildParts();
	void SyncThumb();
	void RaiseScrollChanged();

	int rowCount_ = 0;
	int rowCountPerPage_ = 1;
	int rowPos_ = 0;

	Button* pUpButton_ = nullptr;
	Button* pDownButton_ = nullptr;
	Thumb* pThumb_ = nullptr;

	float dragStartRatio_ = 0.0f;

	_u32 linkElementId_ = 0;
	int linkHandlerToken_ = 0;
};

} // namespace sgui
