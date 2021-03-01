#include "DisplayLabel.hpp"

START_NAMESPACE_DISTRHO

DisplayLabel::DisplayLabel(NanoWidget *widget, Size<uint> size) noexcept
	: LabelBox(widget, size),
	  display_label(""),
	  display_number(0.f),
	  display_units(""),
	  max_chars(3)
{}

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

void DisplayLabel::setMaxChars(const int max_chars)
{
	this->max_chars = max_chars;
	updateDisplayText();
}

void DisplayLabel::updateDisplayText()
{
	const int int_part = std::floor(display_number);
	const int int_digits = graphdyn::getNumDigits(int_part);
	const int decimal_digits = max_chars - 1 - int_digits;
	const int decimal_part = std::round(
		(display_number-int_part) * std::pow(10, decimal_digits) );

	std::stringstream ss("");
	ss << display_label << std::setfill('0')
	   << " " << std::setw(int_digits) << int_part
	   << "." << std::setw(decimal_digits) << decimal_part
	   << " " << display_units;
	LabelBox::setText(ss.str().c_str());
}

END_NAMESPACE_DISTRHO
