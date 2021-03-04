#ifndef WOLF_VOLUME_KNOB_HPP_INCLUDED
#define WOLF_VOLUME_KNOB_HPP_INCLUDED

#include "NanoKnob.hpp"
#include "Animation.hpp"

#include <memory>

START_NAMESPACE_DISTRHO

class VolumeKnob
	: public NanoKnob,
      public IdleCallback
{
public:
  /* explicit VolumeKnob(Window &parent, Size<uint> size) noexcept; */
  explicit VolumeKnob( NanoWidget *widget, float radius ) noexcept;

protected:
  void idleCallback() override;

  void onMouseHover() override;
  void onMouseLeave() override;
  void onMouseUp() override;
  void onMouseDown() override;

	void onNanoDisplay() override;

private:
  std::unique_ptr<FloatTransition> fGrowAnimation;
  std::unique_ptr<ColorTransition> fHoverAnimation;
  
  float fKnobDiameter;

  Color fKnobICol;
  Color fKnobOCol;

  DISTRHO_LEAK_DETECTOR(VolumeKnob)
};

END_NAMESPACE_DISTRHO

#endif
