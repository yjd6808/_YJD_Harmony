/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Panels/Grid.h"

#include <algorithm>

namespace sgui
{

Grid* Grid::Create()
{
	Grid* pGrid = dbg_new Grid;
	pGrid->autorelease();
	return pGrid;
}

cc::size Grid::MeasureOverride(const cc::size& _constraint)
{
	const int rowCount = GetRowCount();
	const int columnCount = GetColumnCount();

	rowContent_.assign(rowCount, 0.0f);
	columnContent_.assign(columnCount, 0.0f);

	// 1차: 자식 측정. span 1짜리 자식만 트랙 콘텐츠 크기에 반영한다. (span은 arrange에서만 반영되는 근사치)
	for (size_t i = 0; i < children_.size(); ++i)
	{
		UIElement* pChild = children_[i];

		const int row = std::min(std::max(0, pChild->GetGridRow()), rowCount - 1);
		const int column = std::min(std::max(0, pChild->GetGridColumn()), columnCount - 1);

		// 셀 크기를 정확히 알기 전이므로 전체 제약으로 측정한다.
		cc::size cellConstraint = _constraint;

		if (row < (int)rowDefinitions_.size() && rowDefinitions_[row].height_.type_ == GridUnitType::Pixel)
		{
			cellConstraint.height = rowDefinitions_[row].height_.value_;
		}

		if (column < (int)columnDefinitions_.size() && columnDefinitions_[column].width_.type_ == GridUnitType::Pixel)
		{
			cellConstraint.width = columnDefinitions_[column].width_.value_;
		}

		pChild->Measure(cellConstraint);

		if (pChild->GetVisibility() == Visibility::Collapsed)
			continue;

		const cc::size& desired = pChild->GetDesiredSize();

		if (pChild->GetGridColumnSpan() <= 1)
		{
			columnContent_[column] = std::max(columnContent_[column], desired.width);
		}

		if (pChild->GetGridRowSpan() <= 1)
		{
			rowContent_[row] = std::max(rowContent_[row], desired.height);
		}
	}

	// 2차: 트랙 크기 결정 후 합산
	std::vector<float> columnSizes;
	std::vector<float> rowSizes;
	ResolveTracks(columnSizes, columnContent_, true, _constraint.width);
	ResolveTracks(rowSizes, rowContent_, false, _constraint.height);

	float width = 0.0f;
	float height = 0.0f;

	for (size_t i = 0; i < columnSizes.size(); ++i)
	{
		width += columnSizes[i];
	}

	for (size_t i = 0; i < rowSizes.size(); ++i)
	{
		height += rowSizes[i];
	}

	return cc::size(width, height);
}

void Grid::ArrangeOverride(const cc::size& _finalSize)
{
	const int rowCount = GetRowCount();
	const int columnCount = GetColumnCount();

	if ((int)rowContent_.size() != rowCount)
	{
		rowContent_.assign(rowCount, 0.0f);
	}

	if ((int)columnContent_.size() != columnCount)
	{
		columnContent_.assign(columnCount, 0.0f);
	}

	std::vector<float> columnSizes;
	std::vector<float> rowSizes;
	ResolveTracks(columnSizes, columnContent_, true, _finalSize.width);
	ResolveTracks(rowSizes, rowContent_, false, _finalSize.height);

	// 트랙 오프셋 누적
	std::vector<float> columnOffsets(columnCount, 0.0f);
	std::vector<float> rowOffsets(rowCount, 0.0f);

	for (int idx = 1; idx < columnCount; ++idx)
	{
		columnOffsets[idx] = columnOffsets[idx - 1] + columnSizes[idx - 1];
	}

	for (int idx = 1; idx < rowCount; ++idx)
	{
		rowOffsets[idx] = rowOffsets[idx - 1] + rowSizes[idx - 1];
	}

	for (size_t i = 0; i < children_.size(); ++i)
	{
		UIElement* pChild = children_[i];

		const int row = std::min(std::max(0, pChild->GetGridRow()), rowCount - 1);
		const int column = std::min(std::max(0, pChild->GetGridColumn()), columnCount - 1);
		const int rowSpan = std::min(pChild->GetGridRowSpan(), rowCount - row);
		const int columnSpan = std::min(pChild->GetGridColumnSpan(), columnCount - column);

		float cellWidth = 0.0f;
		float cellHeight = 0.0f;

		for (int c = column; c < column + columnSpan; ++c)
		{
			cellWidth += columnSizes[c];
		}

		for (int r = row; r < row + rowSpan; ++r)
		{
			cellHeight += rowSizes[r];
		}

		pChild->Arrange(Rect(columnOffsets[column], rowOffsets[row], cellWidth, cellHeight));
	}
}

void Grid::ResolveTracks(
	std::vector<float>& _outSizes,
	const std::vector<float>& _contentSizes,
	bool _isColumn,
	float _available) const
{
	const int count = _isColumn ? GetColumnCount() : GetRowCount();
	_outSizes.assign(count, 0.0f);

	float fixedTotal = 0.0f;
	float starTotal = 0.0f;

	// 1차: Pixel/Auto 확정, Star 가중치 합산
	for (int idx = 0; idx < count; ++idx)
	{
		GridLength length = GridLength::Star();
		float minSize = 0.0f;
		float maxSize = Infinity_v;

		if (_isColumn && idx < (int)columnDefinitions_.size())
		{
			length = columnDefinitions_[idx].width_;
			minSize = columnDefinitions_[idx].minWidth_;
			maxSize = columnDefinitions_[idx].maxWidth_;
		}
		else if (!_isColumn && idx < (int)rowDefinitions_.size())
		{
			length = rowDefinitions_[idx].height_;
			minSize = rowDefinitions_[idx].minHeight_;
			maxSize = rowDefinitions_[idx].maxHeight_;
		}

		switch (length.type_)
		{
		case GridUnitType::Pixel:
			_outSizes[idx] = ClampF(length.value_, minSize, maxSize);
			fixedTotal += _outSizes[idx];
			break;
		case GridUnitType::Auto:
			_outSizes[idx] = ClampF(idx < (int)_contentSizes.size() ? _contentSizes[idx] : 0.0f, minSize, maxSize);
			fixedTotal += _outSizes[idx];
			break;
		case GridUnitType::Star:
			starTotal += std::max(0.0f, length.value_);
			break;
		}
	}

	// 2차: Star 트랙에 잔여 공간 비례 분배
	// available이 무한(측정 단계)이면 콘텐츠 크기를 그대로 사용한다.
	const bool infinite = std::isinf(_available);
	const float remain = infinite ? 0.0f : std::max(0.0f, _available - fixedTotal);

	for (int idx = 0; idx < count; ++idx)
	{
		GridLength length = GridLength::Star();
		float minSize = 0.0f;
		float maxSize = Infinity_v;

		if (_isColumn && idx < (int)columnDefinitions_.size())
		{
			length = columnDefinitions_[idx].width_;
			minSize = columnDefinitions_[idx].minWidth_;
			maxSize = columnDefinitions_[idx].maxWidth_;
		}
		else if (!_isColumn && idx < (int)rowDefinitions_.size())
		{
			length = rowDefinitions_[idx].height_;
			minSize = rowDefinitions_[idx].minHeight_;
			maxSize = rowDefinitions_[idx].maxHeight_;
		}

		if (length.type_ != GridUnitType::Star)
			continue;

		if (infinite)
		{
			_outSizes[idx] = ClampF(idx < (int)_contentSizes.size() ? _contentSizes[idx] : 0.0f, minSize, maxSize);
		}
		else if (starTotal > 0.0f)
		{
			_outSizes[idx] = ClampF(remain * (std::max(0.0f, length.value_) / starTotal), minSize, maxSize);
		}
	}
}

} // namespace sgui
