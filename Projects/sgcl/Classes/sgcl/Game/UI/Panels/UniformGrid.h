/*
 * 작성자: 윤정도
 * =====================
 * sgui - 균등 분할 그리드 (WPF UniformGrid)
 */

#pragma once

#include "sgcl/Game/UI/Panels/Panel.h"

namespace sgui
{

class UniformGrid : public Panel
{
public:
	static UniformGrid* Create(int _rows = 0, int _columns = 0)
	{
		UniformGrid* pPanel = dbg_new UniformGrid;
		pPanel->rows_ = _rows;
		pPanel->columns_ = _columns;
		pPanel->autorelease();
		return pPanel;
	}

	void SetRows(int _rows) { rows_ = _rows; InvalidateLayout(); }
	void SetColumns(int _columns) { columns_ = _columns; InvalidateLayout(); }
	void SetSpacing(float _horizontal, float _vertical) { hSpacing_ = _horizontal; vSpacing_ = _vertical; InvalidateLayout(); }

protected:
	void ResolveGrid(int& _outRows, int& _outColumns) const
	{
		const int count = std::max(1, (int)children_.size());
		int columns = columns_;
		int rows = rows_;

		if (columns <= 0 && rows <= 0)
		{
			columns = (int)std::ceil(std::sqrt((double)count));
			rows = (count + columns - 1) / columns;
		}
		else if (columns <= 0)
		{
			columns = (count + rows - 1) / rows;
		}
		else if (rows <= 0)
		{
			rows = (count + columns - 1) / columns;
		}

		_outRows = std::max(1, rows);
		_outColumns = std::max(1, columns);
	}

	cc::size MeasureOverride(const cc::size& _constraint) override
	{
		int rows, columns;
		ResolveGrid(rows, columns);

		const cc::size cell(
			std::max(0.0f, (_constraint.width - hSpacing_ * (columns - 1)) / columns),
			std::max(0.0f, (_constraint.height - vSpacing_ * (rows - 1)) / rows));

		float maxChildWidth = 0.0f;
		float maxChildHeight = 0.0f;

		for (size_t i = 0; i < children_.size(); ++i)
		{
			children_[i]->Measure(cell);
			maxChildWidth = std::max(maxChildWidth, children_[i]->GetDesiredSize().width);
			maxChildHeight = std::max(maxChildHeight, children_[i]->GetDesiredSize().height);
		}

		return cc::size(
			maxChildWidth * columns + hSpacing_ * (columns - 1),
			maxChildHeight * rows + vSpacing_ * (rows - 1));
	}

	void ArrangeOverride(const cc::size& _finalSize) override
	{
		int rows, columns;
		ResolveGrid(rows, columns);

		const float cellWidth = std::max(0.0f, (_finalSize.width - hSpacing_ * (columns - 1)) / columns);
		const float cellHeight = std::max(0.0f, (_finalSize.height - vSpacing_ * (rows - 1)) / rows);

		for (size_t i = 0; i < children_.size(); ++i)
		{
			const int row = (int)i / columns;
			const int column = (int)i % columns;

			children_[i]->Arrange(Rect(
				column * (cellWidth + hSpacing_),
				row * (cellHeight + vSpacing_),
				cellWidth,
				cellHeight));
		}
	}

	int rows_ = 0;
	int columns_ = 0;
	float hSpacing_ = 0.0f;
	float vSpacing_ = 0.0f;
};

} // namespace sgui
