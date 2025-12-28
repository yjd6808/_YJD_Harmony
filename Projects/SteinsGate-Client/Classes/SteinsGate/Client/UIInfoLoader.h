/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 1:30:10 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/UIInfo.h>
#include <SteinsGate/Common/ConfigFileLoaderAbstract.h>

struct UIInfoLoader : ConfigFileLoaderAbstract
{
public:
	UIInfoLoader(DataManagerAbstract* _pManager);
	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::UI; }

	bool Load() override;

	static void ReadElementCommon(Json::Value& _elementRoot, OUT UIElementInfo* _pElementInfo);
	static void ReadElementGroup(Json::Value& _groupRoot, OUT UIGroupInfo* _pGroupInfo);
	static UIElementInfo* ReadElementButton(Json::Value& _buttonRoot);
	static UIElementInfo* ReadElementLabel(Json::Value& _labelRoot);
	static UIElementInfo* ReadElementSprite(Json::Value& _spriteRoot);
	static UIElementInfo* ReadElementEditBox(Json::Value& _editBoxRoot);
	static UIElementInfo* ReadElementCheckBox(Json::Value& _checkBoxRoot);
	static UIElementInfo* ReadElementToggleButton(Json::Value& _toggleButtonRoot);
	static UIElementInfo* ReadElementScrollBar(Json::Value& _scrollBarRoot);
	static UIElementInfo* ReadElementProgressBar(Json::Value& _progressBarRoot);
	static UIElementInfo* ReadElementStatic(Json::Value& _staticRoot);

	static constexpr const char* JSON_ELEMENTS_KEY = "elements";
	static constexpr const char* JSON_GROUP_MASTER_KEY = "group_master";
	static constexpr const char* JSON_GROUPS_KEY = "groups";

	// 엘리먼트
	static constexpr const char* JSON_CODE_KEY = "code";
	static constexpr const char* JSON_ELEMENT_TYPE_KEY = "type";
	static constexpr const char* JSON_VISUAL_NAME_KEY = "name";
	static constexpr const char* JSON_VISUAL_SIZE_KEY = "size";
	static constexpr const char* JSON_V_ALIGN_KEY = "valign";
	static constexpr const char* JSON_H_ALIGN_KEY = "halign";
	static constexpr const char* JSON_LINEAR_DODGE_KEY = "linear_dodge";

	// 그룹
	static constexpr const char* JSON_CHILDREN_KEY = "children";

	// 버튼
	static constexpr const char* JSON_SGA_KEY = "sga";
	static constexpr const char* JSON_IMG_KEY = "img";
	static constexpr const char* JSON_SPRITE_KEY = "sprite";

	// 스프라이트
	static constexpr const char* JSON_SCALE9 = "scale9";

	// 라벨
	static constexpr const char* JSON_FONT_KEY = "font";
	static constexpr const char* JSON_FONT_SIZE_KEY = "font_size";
	static constexpr const char* JSON_FONT_COLOR_KEY = "font_color";
	static constexpr const char* JSON_TEXT_KEY = "text";
	static constexpr const char* JSON_TEXT_WRAP_KEY = "text_wrap";
	static constexpr const char* JSON_TEXT_V_ALIGN_KEY = "text_valign";
	static constexpr const char* JSON_TEXT_H_ALIGN_KEY = "text_halign";

	// 에딧 박스
	static constexpr const char* JSON_PLACEHOLDER_TEXT_KEY = "p_text";
	static constexpr const char* JSON_PLACEHOLDER_FONT_COLOR_KEY = "p_font_color";
	static constexpr const char* JSON_PLACEHOLDER_FONT_SIZE_KEY = "p_font_size";
	static constexpr const char* JSON_MAX_LENGTH_KEY = "max_length";
	static constexpr const char* JSON_INPUT_MODE_KEY = "input_mode";

	// 체크 박스
	static constexpr const char* JSON_CHECK = "check";
	static constexpr const char* JSON_BACKGROUND_SGA = "bg_sga";
	static constexpr const char* JSON_BACKGROUND_IMG = "bg_img";
	static constexpr const char* JSON_CROSS_SGA = "cross_sga";
	static constexpr const char* JSON_CROSS_IMG = "cross_img";

	// 토글 버튼
	static constexpr const char* JSON_TOGGLE_SPRITE_KEY = "sprite2";

	// 프로그래스바
	static constexpr const char* JSON_DIRECTION_KEY = "direction";

	// 스크롤바
	static constexpr const char* JSON_TRACK_SIZE_KEY = "track_size";
};
