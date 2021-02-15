#ifndef WOLF_LABEL_BOX_HPP_INCLUDED
#define WOLF_LABEL_BOX_HPP_INCLUDED

#include <string>

#include "Widget.hpp"
#include "NanoVG.hpp"

START_NAMESPACE_DISTRHO

class LabelBox : public NanoWidget
{
public:
	explicit LabelBox(NanoWidget *widget, Size<uint> size) noexcept;

	void setFontSize(float fontSize);
	float getFontSize() const;

	void setText(std::string text);
	const std::string& getText() const;

	void setFontId(NanoVG::FontId fontId);
	NanoVG::FontId getFontId() const;

protected:
	void onNanoDisplay() override;

private:
	std::string fText;
	float fFontSize;
	NanoVG::FontId fFontId;

	DISTRHO_LEAK_DETECTOR(LabelBox)
};

END_NAMESPACE_DISTRHO

#endif
