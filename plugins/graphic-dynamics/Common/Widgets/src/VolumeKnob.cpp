#include "VolumeKnob.hpp"
#include "UIConfig.hpp"

START_NAMESPACE_DISTRHO

VolumeKnob::VolumeKnob( NanoWidget *widget, float radius ) noexcept
	: NanoKnob(widget, radius),
	  font_size(UIConfig::labelFontSizeSmall())
{
    const float gaugeWidth = 3.5f;
    const float diameter = (radius - gaugeWidth) * 2.0f - 4;

    fKnobICol = Color(86, 92, 95, 255);

    fKnobOCol = Color(39, 42, 43, 255);
    const Color fKnobTargetOCol = Color(59, 62, 63, 255);

    fKnobDiameter = diameter;

    fGrowAnimation = std::make_unique<FloatTransition>(
		0.100f,
		&fKnobDiameter,
		fKnobDiameter*7/8
	);
    fHoverAnimation = std::make_unique<ColorTransition>(
		0.200f,
		&fKnobOCol,
		fKnobTargetOCol
	);

    widget->getParentWindow().addIdleCallback(this);
}

void VolumeKnob::setRange(float min, float max) noexcept
{
	NanoKnob::setRange(min, max);
	this->text_min = std::to_string(min);
	this->text_max = std::to_string(max);
}

void VolumeKnob::idleCallback()
{
    bool mustRepaint = false;

    if (fGrowAnimation->isPlaying())
    {
        fGrowAnimation->run();
        mustRepaint = true;
    }

    if (fHoverAnimation->isPlaying())
    {
        fHoverAnimation->run();
        mustRepaint = true;
    }

    if (mustRepaint)
        repaint();
}

void VolumeKnob::onMouseHover()
{
    if (!canBeFocused())
        return;

    getParentWindow().setCursorStyle(Window::CursorStyle::Grab);

    fHoverAnimation->play(Animation::Forward);
}

void VolumeKnob::onMouseLeave()
{
    if (!canBeFocused())
        return;
        
    getParentWindow().setCursorStyle(Window::CursorStyle::Default);

    fHoverAnimation->play(Animation::Backward);
}

void VolumeKnob::onMouseDown()
{
    fGrowAnimation->pause();
    fGrowAnimation->setDuration(0.100f);
    fGrowAnimation->seek(fGrowAnimation->getCurrentTime() / 2.0f);
    fGrowAnimation->play(Animation::Forward);
}

void VolumeKnob::onMouseUp()
{
    fGrowAnimation->pause();
    fGrowAnimation->setDuration(0.400f);
    fGrowAnimation->seek(fGrowAnimation->getCurrentTime() * 2.0f);
    fGrowAnimation->play(Animation::Backward);
}

void VolumeKnob::onNanoDisplay()
{
    const float height = getHeight();
    const Color color = getColor();

    const float value = getValue();
    const float min = getMin();
    const float max = getMax();

    const float percentFilled = (value - min) / (max - min);

    const float radius = height / 2.0f;

    const float indicatorLineHeight = fKnobDiameter / 2.0f - 8;
    const float indicatorLineWidth = 3.0f;
    const float indicatorLineMarginTop = 4.0f;

    const float gaugeWidth = 3.5f;
    Color gaugeColor = Color(0, 0, 40, 255);
    gaugeColor.interpolate(color, 0.4f);

    const float margin = 3.0f;

    //Gauge (empty)
    beginPath();

    strokeWidth(gaugeWidth);
    strokeColor(gaugeColor);
    arc(radius, radius, radius - margin, 0.75f * M_PI, 0.25f * M_PI, NanoVG::Winding::CW);
    stroke();

    //Gauge (value)
    beginPath();

    strokeWidth(gaugeWidth);
    strokeColor(color);
    arc(radius, radius, radius - margin, 0.75f * M_PI, (0.75f + 1.5f * percentFilled) * M_PI, NanoVG::Winding::CW);
    stroke();

    //Knob
    beginPath();

    strokeWidth(2.0f);
    strokePaint(linearGradient(0, 0, 0, height - 10, Color(190, 190, 190, 30), Color(23, 23, 23, 255)));

    Paint knobPaint = linearGradient(radius, gaugeWidth, radius, fKnobDiameter, fKnobICol, fKnobOCol);
    fillPaint(knobPaint);

    circle(radius, radius, fKnobDiameter / 2.0f);
    fill();
    stroke();

    //Indicator line
    beginPath();
    save();

    translate(radius, radius);
    rotate((2.0f + ((percentFilled - 0.5f) * 1.5f)) * M_PI);
    translate(-radius, -radius);

    fillColor(color);
    rect(radius - indicatorLineWidth / 2.0f, margin + indicatorLineMarginTop + fKnobDiameter / 2.0f - radius, indicatorLineWidth, indicatorLineHeight);
    fill();

    restore();

    closePath();

	if (show_units) {
		beginPath();
		fontFace("chivo_bold");
		fontSize(font_size);
		fillColor(Color(255,255,255));
		textAlign(ALIGN_CENTER | ALIGN_MIDDLE);
		text(0,height, text_min.c_str(), NULL);
		closePath();
	}
}

END_NAMESPACE_DISTRHO
