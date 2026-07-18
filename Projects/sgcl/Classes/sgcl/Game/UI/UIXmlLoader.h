#pragma once

#include "sg/Struct/SteinsGate_UI.h"
#include "tinyxml2.h"

struct UIXmlLoader
{
    static UIGroupInfo* LoadFromFile(const char* _filePath);
    static UIGroupInfo* LoadFromString(const char* _xmlContent);

    static UIElementInfo* ParseElement(tinyxml2::XMLElement* _xmlElement);
    static UIGroupInfo* ParseGroup(tinyxml2::XMLElement* _xmlElement);

private:
    static UIElementInfo* ParseButton(tinyxml2::XMLElement* _elem);
    static UIElementInfo* ParseLabel(tinyxml2::XMLElement* _elem);
    static UIElementInfo* ParseSprite(tinyxml2::XMLElement* _elem);
    static UIElementInfo* ParseEditBox(tinyxml2::XMLElement* _elem);
    static UIElementInfo* ParseCheckBox(tinyxml2::XMLElement* _elem);
    static UIElementInfo* ParseToggleButton(tinyxml2::XMLElement* _elem);
    static UIElementInfo* ParseProgressBar(tinyxml2::XMLElement* _elem);
    static UIElementInfo* ParseScrollBar(tinyxml2::XMLElement* _elem);
    static UIElementInfo* ParseStatic(tinyxml2::XMLElement* _elem);
};