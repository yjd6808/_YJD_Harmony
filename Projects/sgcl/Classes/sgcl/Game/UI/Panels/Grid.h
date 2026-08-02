/*
 * 작성자: 윤정도
 * =====================
 * sgui - 행/열 정의 기반 그리드 (WPF Grid)
 * RowDefinition/ColumnDefinition의 Pixel/Auto/Star 단위를 지원하며,
 * 자식은 SetGridRow/SetGridColumn/SetGridRowSpan/SetGridColumnSpan 부착 속성으로 배치된다.
 */

#pragma once

#include "sgcl/Game/UI/Panels/Panel.h"

namespace sgui
{

class Grid : public Panel
{
public:
	static Grid* Create();

	void AddRowDefinition(const RowDefinition& _definition) { rowDefinitions_.push_back(_definition); InvalidateLayout(); }
	void AddRowDefinition(const GridLength& _height) { RowDefinition definition; definition.height_ = _height; AddRowDefinition(definition); }
	void AddColumnDefinition(const ColumnDefinition& _definition) { columnDefinitions_.push_back(_definition); InvalidateLayout(); }
	void AddColumnDefinition(const GridLength& _width) { ColumnDefinition definition; definition.width_ = _width; AddColumnDefinition(definition); }

	int GetRowCount() const { return std::max(1, (int)rowDefinitions_.size()); }
	int GetColumnCount() const { return std::max(1, (int)columnDefinitions_.size()); }

protected:
	virtual cc::size MeasureOverride(const cc::size& _constraint) override;
	virtual void ArrangeOverride(const cc::size& _finalSize) override;

	// _lengths: 각 트랙의 확정 크기 결과
	void ResolveTracks(
		std::vector<float>& _outSizes,
		const std::vector<float>& _contentSizes,
		bool _isColumn,
		float _available) const;

	std::vector<RowDefinition> rowDefinitions_;
	std::vector<ColumnDefinition> columnDefinitions_;

	// Measure 단계에서 수집한 트랙별 콘텐츠 크기 (Auto/Star 트랙의 최소 크기 근사치)
	std::vector<float> rowContent_;
	std::vector<float> columnContent_;
};

} // namespace sgui
