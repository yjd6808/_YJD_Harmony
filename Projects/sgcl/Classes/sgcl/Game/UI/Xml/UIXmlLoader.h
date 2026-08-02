/*
 * 작성자: 윤정도
 * =====================
 * sgui - WPF 스타일 XML 레이아웃 로더
 *
 * 루트 태그는 <Window>. 자식으로 패널/컨트롤 트리를 구성한다.
 * - 태그: Grid / StackPanel / Canvas / DockPanel / Border / TextBlock / Button / CheckBox /
 *         ToggleButton / RadioButton / ProgressBar / ScrollBar / Slider / TextBox / Image
 * - 속성: Width / Height / Margin / HorizontalAlignment / VerticalAlignment / Visibility /
 *         IsEnabled / Name / Foreground / Background / FontSize / FontCode,
 *         부착 속성 Grid.Row / Grid.Column / Canvas.Left / Canvas.Top / Dock 등
 * - 바인딩: 값에 중괄호({...})/백틱(``)이 포함되면 트리 완성 후 지연 적용된다.
 * - 데이터: <Window> 하위 <DataList>/<Data> 선언 (UIValue.h 참조)
 */

#pragma once

#include "sgcl/Game/UI/Controls/Window.h"

namespace sgui
{

class UIXmlLoader
{
public:
	// XML 파일에서 WPF 스타일 윈도우를 생성한다. 실패 시 nullptr. (호출 후 autorelease 상태)
	static Window* LoadWindowFromFile(const char* _filePath);

	// 문자열 XML에서 윈도우를 생성한다.
	static Window* LoadWindowFromString(const char* _xmlContent);

	// 기존 윈도우 인스턴스에 XML 파일의 레이아웃 트리를 병합한다. (UI_* 코드 빌드 윈도우 + XML 우선 전환용)
	// 성공 시 해당 윈도우의 IsXmlLoaded()를 true로 설정한다. 실패 시 false.
	static bool LoadWindowInto(Window* _pWindow, const char* _filePath);

	// 기존 윈도우 인스턴스에 문자열 XML을 병합한다.
	static bool LoadWindowIntoString(Window* _pWindow, const char* _xmlContent);
};

} // namespace sgui
