#include "LabelBox.hpp"
#include "Mathf.hpp"
#include "UIConfig.hpp"

START_NAMESPACE_DISTRHO

LabelBox::LabelBox(NanoWidget *widget, Size<uint> size) noexcept : NanoWidget(widget)
{
    setSize(size);
	setFontSize(UIConfig::label_font_size);
}

void LabelBox::onNanoDisplay()
{
    const float width = getWidth();
    const float height = getHeight();
    const float verticalMargin = 6.0f;
    const float boxOutlineWidth = 2.0f;

    //Box background
    beginPath();

    fillColor(Color(34, 34, 34, 255));
    strokeColor(Color(64, 64, 64, 255));
    strokeWidth(boxOutlineWidth);

    rect(0, 0, width, height);
    fill();
    stroke();

    closePath();

    //Shadow at top of box
    beginPath();

    strokeColor(0,0,0,255);
    strokeWidth(boxOutlineWidth);

    moveTo(boxOutlineWidth, boxOutlineWidth);
    lineTo(width - boxOutlineWidth, boxOutlineWidth);
    stroke();

    closePath();

    //Text
    beginPath();

    fontFaceId(fFontId);
    fontSize(fFontSize);
    fillColor(Color(255, 255, 255, 255));
    textAlign(ALIGN_CENTER | ALIGN_MIDDLE);

    text(std::round(width / 2.0f),
		std::round(height / 2.0f + verticalMargin / 2.0f - 2),
		fText.c_str(), NULL);

    closePath();
}

void LabelBox::setFontSize(float fontSize)
{
    fFontSize = fontSize;
}

float LabelBox::getFontSize() const
{
    return fFontSize;
}

void LabelBox::setText(std::string text)
{
    fText = text;
}

const std::string& LabelBox::getText() const
{
    return fText;
}

void LabelBox::setFontId(NanoVG::FontId fontId)
{
    fFontId = fontId;
}

NanoVG::FontId LabelBox::getFontId() const
{
    return fFontId;
}

END_NAMESPACE_DISTRHO
