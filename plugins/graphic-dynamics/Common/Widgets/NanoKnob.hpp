#ifndef WOLF_NANO_KNOB_HPP_INCLUDED
#define WOLF_NANO_KNOB_HPP_INCLUDED

#include "Widget.hpp"
#include "NanoVG.hpp"
#include "Window.hpp"
#include "WolfWidget.hpp"

START_NAMESPACE_DISTRHO

class NanoKnob : public WolfWidget
{
public:
  class Callback
  {
  public:
    virtual ~Callback() {}
    virtual void nanoKnobValueChanged(NanoKnob *nanoKnob, const float value) = 0;
  };

  explicit NanoKnob(NanoWidget *widget, Size<uint> size) noexcept;

  float getValue() const noexcept;
  void setValue(float value, bool sendCallback = false) noexcept;

  void setDefault(float def) noexcept;
  void setRange(float min, float max) noexcept;
  void setStep(float step) noexcept;
  void setExponential(const bool is_exponential) noexcept;
  void setVariableResistance(const bool variable_resistance) noexcept;
  void setCallback(Callback *callback) noexcept;
  void setColor(Color color) noexcept;

protected:
  void onNanoDisplay() override;
  bool onMouse(const MouseEvent &) override;
  bool onMotion(const MotionEvent &) override;
  bool onScroll(const ScrollEvent &) override;
  
  Color getColor() noexcept;
  float getMin() noexcept;
  float getMax() noexcept;

  virtual void onMouseHover();
  virtual void onMouseLeave();
  virtual void onMouseUp();
  virtual void onMouseDown();

  virtual void draw() = 0;

private:
	const float motion_resistance = 1200.f;
	const float scroll_resistance = 80.f;
	const float min_resistance = 0.1f;
	// if is_exponential is set, getValueScaled returns exponentially
	// scaled fValue
	bool is_exponential = false;
	auto getValueScaled() const -> float;
	// you can set the knob to have exponential resistance, i'm not sure if that
	// is a good idea yet for user interface design
	bool variable_resistance = false;
	auto calcMotionDelta(const float d_y, const float resistance) const
		-> float;
  float fMin;
  float fMax;
  float fStep;
  float fValue;

  bool fLeftMouseDown;
  Point<int> fLeftMouseDownLocation;
  bool fIsHovered;
  
  int fRotationAngle;
  bool fDragging;
  int fLastX;
  int fLastY;

  Color fColor;

  Callback *fCallback;

  DISTRHO_LEAK_DETECTOR(NanoKnob)
};
;
END_NAMESPACE_DISTRHO

#endif
