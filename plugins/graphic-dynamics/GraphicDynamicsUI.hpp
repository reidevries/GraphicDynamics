#ifndef GRAPHIC_DYNAMICS_UI_HPP
#define GRAPHIC_DYNAMICS_UI_HPP

#include "GraphicDynamicsParameters.hpp"
#include "DistrhoUI.hpp"
#include "GraphWidget.hpp"
#include "RemoveDCSwitch.hpp"
#include "ResetGraphButton.hpp"
#include "OversampleWheel.hpp"
#include "VolumeKnob.hpp"
#include "ResizeHandle.hpp"
#include "LabelBox.hpp"
#include "BipolarModeSwitch.hpp"
#include "GlowingLabelsBox.hpp"
#include "NanoLabel.hpp"
#include "WidgetBar.hpp"
#include "ArrowButton.hpp"
#include "LabelBoxList.hpp"
#include "UIConfig.hpp"

#include <memory>

START_NAMESPACE_DISTRHO

class GraphicDynamicsUI : public UI,
	public NanoSwitch::Callback,
	public NanoButton::Callback,
	public NanoWheel::Callback,
	public NanoKnob::Callback,
	public ResizeHandle::Callback
{
private:
	UIConfig ui;

public:
	GraphicDynamicsUI();
	~GraphicDynamicsUI();

	float getParameterValue(uint32_t index) const;

protected:
	void parameterChanged(uint32_t, float value) override;
	void tryRememberSize();
	void positionWidgets(uint width, uint height);

	void nanoSwitchClicked(NanoSwitch *nanoSwitch) override;
	void nanoButtonClicked(NanoButton *nanoButton) override;
	void nanoWheelValueChanged(NanoWheel *nanoWheel, const int value) override;
	void nanoKnobValueChanged(NanoKnob *nanoKnob, const float value) override;

	void resizeHandleMoved(int width, int height) override;

	void stateChanged(const char *key, const char *value) override;
	void onNanoDisplay() override;
	void uiIdle() override;
	void uiReshape(uint width, uint height) override;
	bool onKeyboard(const KeyboardEvent &ev) override;
	bool onMouse(const MouseEvent &ev) override;

private:
	void updateOnParamChange(uint32_t index, float value);
	void toggleBottomBarVisibility();

	std::unique_ptr<RemoveDCSwitch> sw_remove_dc;
	std::unique_ptr<NanoLabel> label_remove_dc;

	std::unique_ptr<GlowingLabelsBox> label_linearity;

	std::unique_ptr<OversampleWheel> wheel_oversample;
	std::unique_ptr<NanoLabel> label_oversample;

	std::unique_ptr<VolumeKnob> knob_pre, knob_wet, knob_post;
	std::unique_ptr<LabelBox> label_pre, label_wet, label_post;

	std::unique_ptr<VolumeKnob> knob_hor_warp;
	std::unique_ptr<LabelBoxList> label_hor_warp;

	std::unique_ptr<VolumeKnob> knob_ver_warp;
	std::unique_ptr<LabelBoxList> label_ver_warp;

	std::unique_ptr<ArrowButton> button_l_hor_warp_mode, button_r_hor_warp_mode;
	std::unique_ptr<ArrowButton> button_l_ver_warp_mode, button_r_ver_warp_mode;
	
	std::unique_ptr<VolumeKnob> knob_attack, knob_release;
	std::unique_ptr<LabelBox> label_attack, label_release;

	std::unique_ptr<ResizeHandle> handle_resize;

	std::unique_ptr<GraphWidget> graph_widget;
	std::unique_ptr<WidgetBar> graph_bar;
	std::unique_ptr<ResetGraphButton> button_reset_graph;
	std::unique_ptr<NanoLabel> label_reset_graph;

	bool fBottomBarVisible;

	DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphicDynamicsUI)
};

END_NAMESPACE_DISTRHO

#endif
