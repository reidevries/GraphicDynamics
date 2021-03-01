#include "DisplayLabel.hpp"

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
	// need at least three chars for 0.0 to be rendered
	if (max_chars > 3) {
		this->max_chars = max_chars;
		updateDisplayText();
	}
}

void DisplayLabel::updateDisplayText()
{
	const int int_part = std::round(display_number);
	const int int_digits = graphdyn::getNumDigits(int_part);
	const int decimal_digits = max_chars - 1 - int_digits;
	const int decimal_part = std::round(
		(display_number-int_part) * std::pow(10, decimal_digits) );

	std::stringstream ss("");
	ss << display_label
	   << " " << std::setw(int_digits) << int_part
	   << "." << std::setw(decimal_digits) << decimal_part
	   << " " << display_units;
	LabelBox::setText(ss.str().c_str());
}
