#ifndef __DISPLAYLABEL_HPP_
#define __DISPLAYLABEL_HPP_

#include "LabelBox.hpp"
#include "Mathf.hpp"

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

START_NAMESPACE_DISTRHO

// extends NanoLabel by allowing a value to be displayed dynamically
class DisplayLabel : public LabelBox
{
public:
	explicit DisplayLabel(NanoWidget *widget, Size<uint> size) noexcept;

	void setDisplayLabel(const std::string& display_label);
	void setDisplayNumber(const float display_number);
	void setDisplayUnits(const std::string& display_units);
	void setMaxChars(const int max_chars);

private:
	void updateDisplayText();

	std::string display_label;
	float display_number;
	std::string display_units;

	int max_chars;
};

END_NAMESPACE_DISTRHO

#endif // __DISPLAYLABEL_HPP_
