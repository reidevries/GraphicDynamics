#ifndef WOLF_VOLUME_KNOB_HPP_INCLUDED
#define WOLF_VOLUME_KNOB_HPP_INCLUDED

#include <string>

#include "NanoKnob.hpp"
#include "Animation.hpp"
#include "NanoVG.hpp"

#include <memory>

START_NAMESPACE_DISTRHO

class VolumeKnob
	: public NanoKnob,
      public IdleCallback
{
public:
  /* explicit VolumeKnob(Window &parent, Size<uint> size) noexcept; */
  explicit VolumeKnob( NanoWidget *widget, float radius ) noexcept;

	void setShowUnits(bool show_units) {this->show_units = show_units;}
	void setFontId(NanoVG::FontId font_id) {this->font_id = font_id;}
	auto getFontId() const -> NanoVG::FontId {return font_id;}

	void setRange(float min, float max) noexcept override;

protected:
  void idleCallback() override;

  void onMouseHover() override;
  void onMouseLeave() override;
  void onMouseUp() override;
  void onMouseDown() override;

	void onNanoDisplay() override;
	void renderUnits();
private:
  std::unique_ptr<FloatTransition> fGrowAnimation;
  std::unique_ptr<ColorTransition> fHoverAnimation;

  float fKnobDiameter;

  Color fKnobICol;
  Color fKnobOCol;

	bool show_units = false;
	std::string text_max;
	std::string text_min;
	float font_size;
	NanoVG::FontId font_id;

  DISTRHO_LEAK_DETECTOR(VolumeKnob)
};

END_NAMESPACE_DISTRHO

#endif
