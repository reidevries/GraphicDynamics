#include "VolumeKnob.hpp"
#include "UIConfig.hpp"
#include "UIFonts.hpp"

START_NAMESPACE_DISTRHO

VolumeKnob::VolumeKnob( NanoWidget *widget, float radius ) noexcept
	: NanoKnob(widget, radius)
{
    gauge_w = UIConfig::knob_gauge_w;
    knob_diameter = (radius-gauge_w)*1.92f;

    const Color gradient_fill_i = UIConfig::knob_gradient_fill_i;
	const Color gradient_fill_o = UIConfig::knob_gradient_fill_o;
    const Color transition_target_o = Color::brighten(gradient_fill_o, 1.2);

	gradient_stroke = linearGradient(
		0, 0, 0, getHeight()*4.f/5.f,
		UIConfig::knob_gradient_stroke_i,
		UIConfig::knob_gradient_stroke_o
	);
	gradient_fill = linearGradient(
		radius, gauge_w, radius, knob_diameter,
		gradient_fill_i,
		gradient_fill_o
	);

    fGrowAnimation = std::make_unique<FloatTransition>(
		0.100f,
		&knob_diameter,
		knob_diameter*7/8
	);
    fHoverAnimation = std::make_unique<ColorTransition>(
		0.200f,
		&gradient_fill.outerColor,
		transition_target_o
	);

    widget->getParentWindow().addIdleCallback(this);
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
    const float h = getHeight();
    const Color color = getColor();

    const float value = getValue();
    const float min = getMin();
    const float max = getMax();

    const float value_normal = (value - min) / (max - min);

    const float radius = h / 2.0f;

    const float indicator_h = knob_diameter*2.f/5.f;
    const float indicator_w = gauge_w*6.f/7.f;
	const float margin = indicator_w;
    const float indicator_top_m = gauge_w*8.f/7.f;

    Color gaugeColor = UIConfig::knob_gauge_stroke;
    gaugeColor.interpolate(color, 0.4f);

    //Gauge (empty)
    beginPath();

    strokeWidth(gauge_w);
    strokeColor(gaugeColor);
    arc(radius, radius, radius - margin,
		0.75f * M_PI, 0.25f * M_PI,
		NanoVG::Winding::CW);
    stroke();

    //Gauge (value)
    beginPath();

    strokeWidth(gauge_w);
    strokeColor(color);
    arc(radius, radius, radius - margin,
		0.75f * M_PI, (0.75f + 1.5f * value_normal) * M_PI,
		NanoVG::Winding::CW);
    stroke();

    //Knob
    beginPath();

    strokeWidth(2.0f);
    strokePaint(gradient_stroke);

    fillPaint(gradient_fill);

    circle(radius, radius, knob_diameter / 2.0f);
    fill();
    stroke();

    //Indicator line
    beginPath();
    save();

    translate(radius, radius);
    rotate((2.0f + ((value_normal - 0.5f) * 1.5f)) * M_PI);
    translate(-radius, -radius);

    fillColor(color);
    rect(radius - indicator_w / 2.0f,
		 margin + indicator_top_m + knob_diameter / 2.0f - radius,
		 indicator_w, indicator_h);
    fill();

    restore();

    closePath();
}

END_NAMESPACE_DISTRHO
