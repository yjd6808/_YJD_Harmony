/*
 * 작성자: 윤정도
 * =====================
 * sgui 통합 헤더 - WPF 유사 UI 프레임워크 전체를 포함한다.
 */

#pragma once

// Core
#include "sgcl/Game/UI/Core/UITypes.h"
#include "sgcl/Game/UI/Core/UIEvent.h"
#include "sgcl/Game/UI/Core/ElementRegistry.h"
#include "sgcl/Game/UI/Core/Brush.h"
#include "sgcl/Game/UI/Core/BrushVisual.h"
#include "sgcl/Game/UI/Core/UIElement.h"
#include "sgcl/Game/UI/Core/InputDispatcher.h"

// Panels
#include "sgcl/Game/UI/Panels/Panel.h"
#include "sgcl/Game/UI/Panels/Canvas.h"
#include "sgcl/Game/UI/Panels/StackPanel.h"
#include "sgcl/Game/UI/Panels/DockPanel.h"
#include "sgcl/Game/UI/Panels/WrapPanel.h"
#include "sgcl/Game/UI/Panels/UniformGrid.h"
#include "sgcl/Game/UI/Panels/Grid.h"

// Controls
#include "sgcl/Game/UI/Controls/Control.h"
#include "sgcl/Game/UI/Controls/ContentControl.h"
#include "sgcl/Game/UI/Controls/TextBlock.h"
#include "sgcl/Game/UI/Controls/Image.h"
#include "sgcl/Game/UI/Controls/Border.h"
#include "sgcl/Game/UI/Controls/Button.h"
#include "sgcl/Game/UI/Controls/ToggleButton.h"
#include "sgcl/Game/UI/Controls/Thumb.h"
#include "sgcl/Game/UI/Controls/RangeBase.h"
#include "sgcl/Game/UI/Controls/ProgressBar.h"
#include "sgcl/Game/UI/Controls/ScrollBar.h"
#include "sgcl/Game/UI/Controls/Slider.h"
#include "sgcl/Game/UI/Controls/ScrollViewer.h"
#include "sgcl/Game/UI/Controls/TextBox.h"
#include "sgcl/Game/UI/Controls/Window.h"

// Host
#include "sgcl/Game/UI/Host/UIHost.h"
#include "sgcl/Game/UI/Host/UIManager.h"
#include "sgcl/Game/UI/Host/WindowRegister.h"
