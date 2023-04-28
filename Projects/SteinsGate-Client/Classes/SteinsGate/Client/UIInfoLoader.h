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
	UIInfoLoader(DataManagerAbstract* manager);
	ConfigFileType_t getConfigFileType() override { return ConfigFileType::UI; }

	bool load() override;

	static void readElementCommon(Json::Value& elementRoot, Out_ UIElementInfo* elementInfo);
	static void readElementGroup(Json::Value& uiGroupRoot, Out_ UIGroupInfo* groupInfo);
	static UIElementInfo* readElementButton(Json::Value& buttonRoot);
	static UIElementInfo* readElementLabel(Json::Value& labelRoot);
	static UIElementInfo* readElementSprite(Json::Value& spriteRoot);
	static UIElementInfo* readElementEditBox(Json::Value& editBoxRoot);
	static UIElementInfo* readElementCheckBox(Json::Value& checkBoxRoot);
    static UIElementInfo* readElementToggleButton(Json::Value& toggleButtonRoot);
    static UIElementInfo* readElementScrollBar(Json::Value& scrollBarRoot);
    static UIElementInfo* readElementProgressBar(Json::Value& progressBarRoot);
    static UIElementInfo* readElementStatic(Json::Value& staticRoot);
    
        
        


    static constexpr const char* JsonElementsKey = "elements";
    static constexpr const char* JsonGroupMasterKey = "group_master";
    static constexpr const char* JsonGroupsKey = "groups";

    // 엘리먼트
    static constexpr const char* JsonCodeKey = "code";
    static constexpr const char* JsonElementTypeKey = "type";
    static constexpr const char* JsonVisualNameKey = "name";
    static constexpr const char* JsonVisualSizeKey = "size";
    static constexpr const char* JsonVAlignKey = "valign";
    static constexpr const char* JsonHAlignKey = "halign";
    static constexpr const char* JsonLinearDodgeKey = "linear_dodge";

    // 그룹
    static constexpr const char* JsonChildrenKey = "children";

    // 버튼
    static constexpr const char* JsonSgaKey = "sga";
    static constexpr const char* JsonImgKey = "img";
    static constexpr const char* JsonSpriteKey = "sprite";

    // 스프라이트
    static constexpr const char* JsonScale9 = "scale9";

    // 라벨
    static constexpr const char* JsonFontKey = "font";
    static constexpr const char* JsonFontSizeKey = "font_size";
    static constexpr const char* JsonFontColorKey = "font_color";
    static constexpr const char* JsonTextKey = "text";
    static constexpr const char* JsonTextWrapKey = "text_wrap";
    static constexpr const char* JsonTextVAlignKey = "text_valign";
    static constexpr const char* JsonTextHAlignKey = "text_halign";

    // 에딧 박스
    static constexpr const char* JsonPlaceholderTextKey = "p_text";
    static constexpr const char* JsonPlaceholderFontColorKey = "p_font_color";
    static constexpr const char* JsonPlaceholderFontSizeKey = "p_font_size";
    static constexpr const char* JsonMaxLengthKey = "max_length";
    static constexpr const char* JsonInputModeKey = "input_mode";

    // 체크 박스
    static constexpr const char* JsonCheck = "check";
    static constexpr const char* JsonBackgroundSga = "bg_sga";
    static constexpr const char* JsonBackgroundImg = "bg_img";
    static constexpr const char* JsonCrossSga = "cross_sga";
    static constexpr const char* JsonCrossImg = "cross_img";

    // 토글 버튼
    static constexpr const char* JsonToggleSpriteKey = "sprite2";

    // 프로그래스바
    static constexpr const char* JsonDirectionKey = "direction";

    // 스크롤바
    static constexpr const char* JsonTrackSizeKey = "track_size";
};



