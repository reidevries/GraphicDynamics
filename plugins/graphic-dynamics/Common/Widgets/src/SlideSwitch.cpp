#include "SlideSwitch.hpp"

START_NAMESPACE_DISTRHO

SlideSwitch::SlideSwitch(NanoWidget *widget, Size<uint> size) noexcept
	: NanoSwitch(widget, size)
{
}

void SlideSwitch::drawSocket()
{
    const float width = getWidth();
    const float height = getHeight();

    const float centerX = width / 2.0f;
    const float centerY = height / 2.0f;

    const float marginLeftRight = 1.0f;
    const float marginTopBottom = 2.0f;
    const float halfMarginTopBottom = marginTopBottom / 2.0f;

    beginPath();
    fillColor(0, 0, 0, 255);

    ellipse(centerX, centerY,
		centerX - marginLeftRight, centerY - halfMarginTopBottom);
    fill();

    closePath();
}

void SlideSwitch::drawHandle()
{
    const float width = getWidth();
    const float height = getHeight();

    const float halfWidth = width / 2.0f;

    const float radius = 6.0f;

    beginPath();

    fillColor(Color(140, 140, 140, 255));

    if (isDown())
        circle(halfWidth, height - radius, radius);
    else
        circle(halfWidth, radius, radius);

    fill();

    closePath();
}

void SlideSwitch::onNanoDisplay()
{
    drawSocket();
    drawHandle();
}

END_NAMESPACE_DISTRHO
