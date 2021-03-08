#include "DisplayLabel.hpp"
#include "UIConfig.hpp"

START_NAMESPACE_DISTRHO

DisplayLabel::DisplayLabel(NanoWidget *widget, Size<uint> size) noexcept
	: LabelBox(widget, size),
	  display_number(0.f),
	  display_label(""),
	  display_text(""),
	  display_units(""),
	  font_size_units(UIConfig::labelFontSizeSmall()),
	  max_digits(3)
{
	LabelBox::setText(" "); // bit hacky way to draw custom text
}

void DisplayLabel::setDisplayLabel(const std::string& display_label)
{
	if (this->display_label != display_label) {
		this->display_label = display_label;
		updateDisplayText();
	}
}

void DisplayLabel::setDisplayNumber(const float display_number)
{
	this->display_number = display_number;
	updateDisplayText();
}

void DisplayLabel::setDisplayUnits(const std::string& display_units)
{
	if (this->display_units != display_units) {
		this->display_units = display_units;
		updateDisplayText();
	}
}

void DisplayLabel::setMaxDigits(const int max_digits)
{
	updateDisplayText();
	this->max_digits = max_digits;
}

void DisplayLabel::onNanoDisplay()
{
	LabelBox::onNanoDisplay();
	// these four values taken from LabelBox::onNanoDisplay
    const float width = getWidth();
    const float height = getHeight();
    const float verticalMargin = 6.0f;
    const float boxOutlineWidth = 2.0f;

	// left-aligned label text
	beginPath();

	fontFaceId(getFontId());
	fontSize(getFontSize());
	fillColor(Color(255,255,255));
	textAlign(ALIGN_LEFT | ALIGN_MIDDLE);

	text(std::round(boxOutlineWidth + 8),
		std::round(height/2.0f + verticalMargin/2.0f - 2),
		display_label.c_str(), NULL);

	closePath();

	// right-aligned unit text
	beginPath();
	fontFaceId(getFontId());
	fontSize(font_size_units);
	fillColor(Color(255,255,255));
	textAlign(ALIGN_RIGHT | ALIGN_MIDDLE);

	text(std::round(width - boxOutlineWidth - 7),
		std::round(height/2.0f + verticalMargin/2.0f - 2),
		display_text.c_str(), NULL);

	closePath();
}

void DisplayLabel::updateDisplayText()
{
	const int int_part = std::floor(display_number);
	const int int_digits = graphdyn::getNumDigits(int_part);
	const int decimal_digits = max_digits - 1 - int_digits;
	const int decimal_part = std::floor(
		(display_number-int_part) * std::pow(10, decimal_digits) );

	std::stringstream ss("");
	ss << std::setfill('0') << std::setw(int_digits) << int_part;
	if (decimal_digits > 0) {
		ss << "." << std::setw(decimal_digits) << decimal_part;
	}
	ss << display_units;
	display_text = ss.str();
}

END_NAMESPACE_DISTRHO
