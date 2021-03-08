#include "OversampleWheel.hpp"
#include "UIConfig.hpp"
#include "UIFonts.hpp"

START_NAMESPACE_DISTRHO

OversampleWheel::OversampleWheel(NanoWidget *widget, Size<uint> size) noexcept
	: NanoWheel(widget, size)
{
	setFontSize(UIConfig::label_font_size);
	setFontId(UIFonts::chivo_bold_id);
}

void OversampleWheel::setFontSize(const float size)
{
	fFontSize = size;
	triangle_w = size/2.f;
	triangle_h = 3.f*size/2.f;
	triangle_m_x = size-triangle_w;
	triangle_m_y = size-triangle_h;
}

void OversampleWheel::drawTriangles()
{
    const float w = getWidth();
    const float h = getHeight();

    const float tri_right_x = w - triangle_m_x;
    const float tri_left_x = tri_right_x - triangle_w;
    const float tri_mid_x = tri_right_x - triangle_w / 2.0f;

    //top triangle
    const float top_tri_bottom_y = triangle_m_y + triangle_h;

	const Color label_color_invert = Color::invert(UIConfig::label_font_color);

    beginPath();

    moveTo(tri_mid_x, triangle_m_y);

    lineTo(tri_left_x, top_tri_bottom_y);
    lineTo(tri_right_x, top_tri_bottom_y);
    lineTo(tri_mid_x, triangle_m_y);

    fillColor(UIConfig::label_font_color);
    strokeColor(label_color_invert);
    strokeWidth(1.0f);

    fill();
    stroke();

    closePath();

    //bottom triangle
    beginPath();

    const float bottom_tri_bottom_y = h - triangle_m_y;
    const float bottom_tri_top_y = bottom_tri_bottom_y - triangle_h;

    moveTo(tri_mid_x, bottom_tri_bottom_y);

    lineTo(tri_left_x, bottom_tri_top_y);
    lineTo(tri_right_x, bottom_tri_top_y);
    lineTo(tri_mid_x, bottom_tri_bottom_y);

    fillColor(UIConfig::label_font_color);
    strokeColor(label_color_invert);

    fill();
    stroke();

    closePath();
}

void OversampleWheel::drawBackground()
{
    const float width = getWidth();
    const float height = getHeight();
	const float outline_w = UIConfig::wheel_outline_w;

    beginPath();

    Paint background_gradient = linearGradient(
		outline_w, outline_w, outline_w, height,
		UIConfig::wheel_gradient_i,
		UIConfig::wheel_gradient_o);

    fillPaint(background_gradient);
    rect(outline_w, outline_w,
		 width - outline_w * 2.0f, height - outline_w * 2.0f);
    fill();

    closePath();
}

void OversampleWheel::drawText()
{
    const float w = getWidth();
    const float h = getHeight();

    const char *oversamplingFactors[] = {"", "2x", "4x", "8x", "16x"};

    const int value = getValue();

    if (value > 0)
    {
        beginPath();

        fontFaceId(font_id);
        fontSize(fFontSize);
        fillColor(UIConfig::label_font_color);
        textAlign(ALIGN_RIGHT | ALIGN_CENTER);

		const uint text_x = std::round(w - fFontSize - UIConfig::wheel_text_m);
		const uint text_y = std::round(h/1.45f);

        text(text_x, text_y, oversamplingFactors[getValue()], NULL);

        fontBlur(5.0f);
        fillColor(Color(255, 255, 255, 80));
        text(text_x, text_y, oversamplingFactors[getValue()], NULL);
        closePath();
    }
}

void OversampleWheel::drawOutline()
{
    beginPath();

    fillColor(UIConfig::wheel_outline_fill);
    roundedRect(0, 0, getWidth(), getHeight(), 5.0f);
    fill();

    closePath();
}

void OversampleWheel::onNanoDisplay()
{
    const float width = getWidth();
	const float outline_w = UIConfig::wheel_outline_w;
	const float top_line_w = 0.7*outline_w;

    //outline
    drawOutline();

    //background
    drawBackground();

    //line at top of display
    beginPath();

    strokeColor(UIConfig::wheel_top_line_stroke);
    strokeWidth(top_line_w);

    moveTo(outline_w, outline_w);
    lineTo(width - outline_w, outline_w);

    stroke();

    closePath();

    //reflection at bottom of wheel
    /*beginPath();

    Paint wheelShadow = linearGradient(width / 2.0f, height - outline_w, width / 2.0f, height, Color(0,0,0,255), Color(0,0,0,0));
    rect(0, height - outline_w, width, height);

    fillPaint(wheelShadow);

    fill(); 

    closePath();*/

    //triangles
    drawTriangles();

    //text
    drawText();
}

END_NAMESPACE_DISTRHO
