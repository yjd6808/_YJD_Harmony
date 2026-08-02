#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UIThemeInspectorRow.h"

USING_NS_CC;

static const float ROW_HEIGHT = 28.0f;
static const float LABEL_WIDTH = 160.0f;
static const float EDITOR_WIDTH = 200.0f;
static const float RESET_BUTTON_WIDTH = 24.0f;

UIThemeInspectorRow* UIThemeInspectorRow::Create(
    const UIThemePropertyDescriptor& _desc,
    const UIColorF& _initialColor,
    float _initialValue)
{
    auto* pRow = dbg_new UIThemeInspectorRow();
    pRow->descriptor_ = _desc;

    pRow->setContentSize({ LABEL_WIDTH + EDITOR_WIDTH + RESET_BUTTON_WIDTH + 8, ROW_HEIGHT });

    pRow->pLabel_ = Label::createWithSystemFont(_desc.label, "Arial", 12);
    if (pRow->pLabel_)
    {
        pRow->pLabel_->setAnchorPoint({ 0, 0.5f });
        pRow->pLabel_->setPosition(2, ROW_HEIGHT * 0.5f);
        pRow->addChild(pRow->pLabel_);
    }

    pRow->pValueLabel_ = Label::createWithSystemFont("", "Arial", 11);
    if (pRow->pValueLabel_)
    {
        pRow->pValueLabel_->setAnchorPoint({ 1, 0.5f });
        pRow->pValueLabel_->setPosition(LABEL_WIDTH + EDITOR_WIDTH - 4, ROW_HEIGHT * 0.5f);
        pRow->addChild(pRow->pValueLabel_);
    }

    return pRow;
}

void UIThemeInspectorRow::SetColorValue(const UIColorF& _color)
{
    if (pValueLabel_)
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f %.2f %.2f", _color.r, _color.g, _color.b);
        pValueLabel_->setString(buf);
    }
}

void UIThemeInspectorRow::SetFloatValue(float _value)
{
    if (pValueLabel_)
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f", _value);
        pValueLabel_->setString(buf);
    }
}

void UIThemeInspectorRow::SetOverridden(bool _overridden)
{
    isOverridden_ = _overridden;
    if (pLabel_)
    {
        pLabel_->setTextColor(_overridden
            ? Color4B(255, 200, 100, 255)
            : Color4B(200, 200, 200, 255));
    }
}

void UIThemeInspectorRow::SetEnabled(bool _enabled)
{
    if (pLabel_)
    {
        pLabel_->setOpacity(_enabled ? 255 : 100);
    }
}

void UIThemeInspectorRow::SetHighlightColor(const UIColorF& _color)
{
    if (pEditor_)
    {
        auto* colorNode = dynamic_cast<Sprite*>(pEditor_);
        if (colorNode)
        {
            colorNode->setColor(Color3B(
                (GLubyte)(_color.r * 255),
                (GLubyte)(_color.g * 255),
                (GLubyte)(_color.b * 255)));
        }
    }
}
