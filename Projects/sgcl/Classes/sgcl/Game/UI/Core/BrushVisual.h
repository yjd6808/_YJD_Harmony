/*
 * 작성자: 윤정도
 * =====================
 * sgui - Brush를 실제 코코스 노드로 렌더링하는 래퍼
 * 컨트롤은 BrushVisual을 통해서만 배경/트랙/게이지 등을 그린다.
 * 테마 브러시는 UIThemeTextureBinding을 재사용해 테마 리빌드 시 자동 갱신된다.
 */

#pragma once

#include "sgcl/Game/UI/Core/Brush.h"
#include "sgcl/Game/UI/Theme/UIThemeBinding.h"

namespace sgui
{

class BrushVisual
{
public:
	~BrushVisual();

	void Attach(cc::Node* _pHost, int _zOrder = -1);
	void Detach();

	void SetBrush(const BrushPtr& _brush);
	const BrushPtr& GetBrush() const { return brush_; }

	void SetVisualState(UIVisualState _state);
	UIVisualState GetVisualState() const { return state_; }

	void Resize(const cc::size& _size);
	void RefreshTheme();
	void ApplySize();

	cc::Node* GetNode() const { return pNode_; }

private:
	void Rebuild();
	void ApplyThemeBinding();
	bool ComputeFlatFallback() const;
	void SyncNodeKind();

	cc::Node* pHost_ = nullptr;
	cc::Node* pNode_ = nullptr;
	int zOrder_ = -1;

	BrushPtr brush_;
	UIVisualState state_ = UIVisualState::Normal;
	cc::size size_;

	bool flatFallback_ = false;
	UIColorF flatColor_ = UIColorF::FromRGBA(120, 120, 130);

	// Scale9 9-slice 최소 크기 (현재 노드 타입과 무관하게 기억해 flat 폴백 판단에 사용)
	float sliceMinWidth_ = 0.0f;
	float sliceMinHeight_ = 0.0f;

	UIThemeTextureBinding binding_;
};

} // namespace sgui
