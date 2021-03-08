#include "NanoKnob.hpp"
#include "Mathf.hpp"

#include <iostream>

START_NAMESPACE_DISTRHO

NanoKnob::NanoKnob(NanoWidget *widget, float radius) noexcept
    : WolfWidget(widget),
      fMin(0.0f),
      fMax(1.0f),
      fStep(0.0f),
      fValue(0.5f),
      fLeftMouseDown(false),
      fIsHovered(false),
      fColor(Color(255, 0, 0, 255)),
      fCallback(nullptr)
{
    setSize(radius*2, radius*2);
}

float NanoKnob::getValue() const noexcept
{
    return fValue;
}

void NanoKnob::setRange(float min, float max) noexcept
{
    DISTRHO_SAFE_ASSERT(min < max);

    fMin = min;
    fMax = max;

    fValue = wolf::clamp(fValue, min, max);
}

void NanoKnob::setStep(float step) noexcept
{
    fStep = step;
}

float NanoKnob::getMin() noexcept
{
    return fMin;
}

float NanoKnob::getMax() noexcept
{
    return fMax;
}

// in pdesaulniers wolf-shaper: value is assumed to be scaled if using log
// this is not true anymore. fValue is scaled when sent to callback function
// if is_exponential is set. I'm not sure where pdesaulniers was planning to
// scale knobs that were supposed to be logarithmic, because we can't set fValue
// to a logarithmic value and expect onMotion() to still work properly
void NanoKnob::setValue(float value, bool sendCallback) noexcept
{
    value = wolf::clamp(value, fMin, fMax);

    if (d_isEqual(fValue, value))
        return;

    fValue = value;

    if (sendCallback && fCallback != nullptr)
        fCallback->nanoKnobValueChanged(this, getValueScaled());

    repaint();
}

void NanoKnob::setExponential(const bool is_exponential) noexcept
{
    this->is_exponential = is_exponential;
}

void NanoKnob::setVariableResistance(const bool variable_resistance) noexcept
{
	this->variable_resistance = variable_resistance;
}

void NanoKnob::setMinResistance(const float min_resistance) noexcept
{
	// doesn't make sense for minimum resistance to be negative
	if (min_resistance > 0.f) this->min_resistance = min_resistance;
}

void NanoKnob::setCallback(Callback *callback) noexcept
{
    fCallback = callback;
}

void NanoKnob::setColor(Color color) noexcept
{
    fColor = color;
}

Color NanoKnob::getColor() noexcept
{
    return fColor;
}

bool NanoKnob::onMouse(const MouseEvent &ev)
{
    if (ev.button != 1)
        return fLeftMouseDown;

    Window &window = getParentWindow();

    if (!ev.press)
    {
        if (fLeftMouseDown == true)
        {
            fLeftMouseDown = false;
            setFocus(false);

            window.unclipCursor();
            window.setCursorPos(this);
            window.showCursor();
            getParentWindow().setCursorStyle(Window::CursorStyle::Grab);

            onMouseUp();

            return true;
        }

        return false;
    }

    if (contains(ev.pos))
    {
        fLeftMouseDownLocation = ev.pos;
        fLeftMouseDown = true;
        
        setFocus(true);
        window.hideCursor();
        window.clipCursor(Rectangle<int>(getAbsoluteX() + getWidth() / 2.0f, 0, 0, (int)window.getHeight()));

        onMouseDown();

        return true;
    }

    return false;
}

void NanoKnob::onMouseHover()
{
}

void NanoKnob::onMouseLeave()
{
}

void NanoKnob::onMouseUp()
{
}

void NanoKnob::onMouseDown()
{
}

bool NanoKnob::onMotion(const MotionEvent &ev)
{
    if (fLeftMouseDown)
    {
		const float d_y = fLeftMouseDownLocation.getY() - ev.pos.getY();
        const float difference = calcMotionDelta(d_y, motion_resistance);

        Window &window = getParentWindow();
        const int windowHeight = window.getHeight();

		// wrap mouse at top/bottom edges of window,
        // and update the stored mouse location
        // this doesn't seem right.
        // TODO: investigate mouse cursor manipulation code for off-by-one error
        if (ev.pos.getY() + getAbsoluteY() >= windowHeight - 1)
        {
            window.setCursorPos(getAbsoluteX(), 2);
            fLeftMouseDownLocation.setY(-getAbsoluteY() + 2);
        }
        else if (ev.pos.getY() + getAbsoluteY() == 0)
        {
            window.setCursorPos(getAbsoluteX(), windowHeight - 2);
            fLeftMouseDownLocation.setY(windowHeight - getAbsoluteY() - 2);
        }
        else
        {
            fLeftMouseDownLocation.setY(ev.pos.getY());
        }

		// update the value
        setValue(fValue + difference, true);

        return true;
    }

    if (contains(ev.pos))
    {
        if (!fIsHovered)
        {
            fIsHovered = true;
            onMouseHover();
        }
    }
    else
    {
        if (fIsHovered)
        {
            fIsHovered = false;
            onMouseLeave();
        }
    }

    return false;
}

bool NanoKnob::onScroll(const ScrollEvent &ev)
{
    if (!contains(ev.pos)) return false;

	const auto d_y = calcMotionDelta(ev.delta.getY(), scroll_resistance);
    setValue(getValue() + d_y, true);

    return true;
}

auto NanoKnob::getValueScaled() const -> float
{
	if (is_exponential) return wolf::logScale(fValue, fMin, fMax);
	else return fValue;
}

auto NanoKnob::calcMotionDelta(const float d_y, const float resistance) const
	-> float
{
	const auto range = fMax - fMin;

	auto scale = 1.0f;
	if (variable_resistance) {
		// calculate knob position scaled within range 0.0f to 1.0f
		const auto value_coef = (fValue - fMin)/range;
		scale = value_coef + min_resistance;
	}

	return d_y * scale * range / resistance;
}

END_NAMESPACE_DISTRHO
