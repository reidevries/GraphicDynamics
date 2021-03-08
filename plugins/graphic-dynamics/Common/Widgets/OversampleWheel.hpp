#ifndef WOLF_OVERSAMPLE_WHEEL_HPP_INCLUDED
#define WOLF_OVERSAMPLE_WHEEL_HPP_INCLUDED

#include "NanoWheel.hpp"

START_NAMESPACE_DISTRHO

class OversampleWheel : public NanoWheel
{
public:
    explicit OversampleWheel(NanoWidget *widget, Size<uint> size) noexcept;

    void setFontSize(const float size);
	void setFontId(NanoVG::FontId font_id) {this->font_id = font_id;}
	auto getFontId() const -> NanoVG::FontId {return font_id;}

protected:
    void onNanoDisplay() override;
    void drawTriangles();
    void drawBackground();
    void drawText();
    void drawOutline();
    
private:
    float fFontSize;
	NanoVG::FontId font_id;

	float triangle_m_y, triangle_m_x, triangle_w, triangle_h;

    DISTRHO_LEAK_DETECTOR(OversampleWheel)
};

END_NAMESPACE_DISTRHO

#endif
