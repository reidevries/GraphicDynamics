#include "DistrhoUI.hpp"

#include "GraphicDynamicsUI.hpp"
#include "Window.hpp"
#include "Margin.hpp"
#include "Fonts/chivo_bold.hpp"

#include <string>
#include <memory>
#include <sstream>
#include <iostream>

#if defined(DISTRHO_OS_WINDOWS)
#include "windows.h"
#endif

START_NAMESPACE_DISTRHO


GraphicDynamicsUI::GraphicDynamicsUI() : UI(1280, 662), fBottomBarVisible(true)
{
    loadSharedResources();

    using namespace WOLF_FONTS;
    NanoVG::FontId chivoBoldId = createFontFromMemory(
		"chivo_bold", (const uchar *)chivo_bold, chivo_bold_size, 0);
    NanoVG::FontId dejaVuSansId = findFont(NANOVG_DEJAVU_SANS_TTF);

    const float width = getWidth();
    const float height = getHeight();

    graph_widget = std::make_unique<GraphWidget>(
		this,
		Size<uint>(width - 4 * 2, height - 4 * 2 - 122)
	);

	graph_bar = std::make_unique<WidgetBar>(
		this,
		Size<uint>(width, UIConfig::graph_bar_h)
	);
	graph_bar->setStrokePaint( linearGradient(
		0, //start x coord
		0, //start y coord
		0, //end x coord
		UIConfig::graph_bar_h, //end y coord
		Color(43, 43, 43, 255), // start colour
		Color(34, 34, 34, 255)  // end colour
	) );
    graph_bar->setStrokeWidth(4.0f);

    sw_remove_dc = std::make_unique<RemoveDCSwitch>(
		this,
		Size<uint>(30, 29)
	);
    sw_remove_dc->setCallback(this);
    sw_remove_dc->setId(p_remove_dc);

    label_remove_dc = std::make_unique<NanoLabel>(
		this,
		Size<uint>(100, 29)
	);
    label_remove_dc->setText("CENTER");
    label_remove_dc->setFontId(chivoBoldId);
    label_remove_dc->setFontSize(14.0f);
    label_remove_dc->setAlign(ALIGN_LEFT | ALIGN_MIDDLE);
    label_remove_dc->setMargin(
		Margin(3, 0, sw_remove_dc->getWidth() / 2.0f, 0));

	sw_linearity = std::make_unique<SlideSwitch>(this, Size<uint>(16,34));
	sw_linearity->setCallback(this);
	sw_linearity->setId(p_linearity);
    label_linearity = std::make_unique<GlowingLabelsBox>(
		this,
		Size<uint>(34, 42)
	);
    label_linearity->setLabels({"LIN", "EXP"});

    label_pre = std::make_unique<LabelBox>(this, UIConfig::labelSize());
    label_pre->setText("PRE");

    knob_pre = std::make_unique<VolumeKnob>(this, UIConfig::knob_s);
    knob_pre->setCallback(this);
    knob_pre->setRange(0.0f, 2.0f);
    knob_pre->setId(p_pre_gain);
    knob_pre->setColor(UIConfig::gain_col);

    label_wet = std::make_unique<LabelBox>(this, UIConfig::labelSize());
    label_wet->setText("WET");

    knob_wet = std::make_unique<VolumeKnob>(this, UIConfig::knob_s);
    knob_wet->setCallback(this);
    knob_wet->setRange(0.0f, 1.0f);
    knob_wet->setId(p_wet);
    knob_wet->setColor(UIConfig::wet_col);

    label_post = std::make_unique<LabelBox>(this, UIConfig::labelSize());
    label_post->setText("POST");

    knob_post = std::make_unique<VolumeKnob>(this, UIConfig::knob_s);
    knob_post->setCallback(this);
    knob_post->setRange(0.0f, 1.0f);
    knob_post->setId(p_post_gain);
    knob_post->setColor(UIConfig::gain_col);

    knob_hor_warp = std::make_unique<VolumeKnob>(this, UIConfig::knob_s);
    knob_hor_warp->setCallback(this);
    knob_hor_warp->setRange(0.0f, 1.0f);
    knob_hor_warp->setId(p_hor_warp_amt);
    knob_hor_warp->setColor(UIConfig::warp_col);

    label_hor_warp = std::make_unique<LabelBoxList>(
		this,
		Size<uint>(UIConfig::label_w + 3, UIConfig::label_h)
	);
    label_hor_warp->setLabels({
		"–",
		"BEND +", "BEND -", "BEND +/-",
		"SKEW +", "SKEW -", "SKEW +/-" 
	});

    knob_ver_warp = std::make_unique<VolumeKnob>(this, UIConfig::knob_s);
    knob_ver_warp->setCallback(this);
    knob_ver_warp->setRange(0.0f, 1.0f);
    knob_ver_warp->setId(p_ver_warp_amt);
    knob_ver_warp->setColor(UIConfig::warp_col);

    label_ver_warp = std::make_unique<LabelBoxList>(
		this,
		Size<uint>(UIConfig::label_w + 3, UIConfig::label_h)
	);
    label_ver_warp->setLabels({
		"–",
		"BEND +", "BEND -", "BEND +/-",
		"SKEW +", "SKEW -", "SKEW +/-"
	});

    button_l_hor_warp_mode = std::make_unique<ArrowButton>(
		this,
		UIConfig::lrButtonSize()
	);
    button_l_hor_warp_mode->setCallback(this);
    button_l_hor_warp_mode->setId(p_hor_warp_mode);
    button_l_hor_warp_mode->setArrowDirection(ArrowButton::Left);

    button_r_hor_warp_mode = std::make_unique<ArrowButton>(
		this,
		UIConfig::lrButtonSize()
	);
    button_r_hor_warp_mode->setCallback(this);
    button_r_hor_warp_mode->setId(p_ver_warp_mode);
    button_r_hor_warp_mode->setArrowDirection(ArrowButton::Right);

    button_l_ver_warp_mode = std::make_unique<ArrowButton>(
		this,
		UIConfig::lrButtonSize()
	);
    button_l_ver_warp_mode->setCallback(this);
    button_l_ver_warp_mode->setId(p_hor_warp_mode);
    button_l_ver_warp_mode->setArrowDirection(ArrowButton::Left);

    button_r_ver_warp_mode = std::make_unique<ArrowButton>(
		this,
		UIConfig::lrButtonSize()
	);
    button_r_ver_warp_mode->setCallback(this);
    button_r_ver_warp_mode->setId(p_ver_warp_mode);
    button_r_ver_warp_mode->setArrowDirection(ArrowButton::Right);

    label_attack = std::make_unique<DisplayLabel>(this, UIConfig::labelSize());
	label_attack->setDisplayLabel("ATK ");
	label_attack->setDisplayUnits("ms");
	label_attack->setMaxDigits(4);
	knob_attack = std::make_unique<VolumeKnob>(this, UIConfig::knob_s);
	knob_attack->setCallback(this);
	knob_attack->setRange(0.1f, 9999.0f);
	knob_attack->setId(p_atk_ms);
	knob_attack->setColor(UIConfig::time_col);
	knob_attack->setExponential(true);
	knob_attack->setVariableResistance(true);
	knob_attack->setMinResistance(0.1f);

    label_release = std::make_unique<DisplayLabel>(this, UIConfig::labelSize());
	label_release->setDisplayLabel("RLS ");
	label_release->setDisplayUnits("ms");
	label_release->setMaxDigits(4);
	knob_release = std::make_unique<VolumeKnob>(this, UIConfig::knob_s);
	knob_release->setCallback(this);
	knob_release->setRange(0.1f, 9999.0f);
	knob_release->setId(p_rls_ms);
	knob_release->setColor(UIConfig::time_col);
	knob_release->setExponential(true);
	knob_release->setVariableResistance(true);
	knob_release->setMinResistance(0.1f);

    handle_resize = std::make_unique<ResizeHandle>(this, Size<uint>(18, 18));
    handle_resize->setCallback(this);
    handle_resize->setMinSize(UIConfig::min_w, UIConfig::min_h);

    button_reset_graph = std::make_unique<ResetGraphButton>(
		this, Size<uint>(32, 32));
    button_reset_graph->setCallback(this);

    label_reset_graph = std::make_unique<NanoLabel>(
		this,
		Size<uint>(50, button_reset_graph->getHeight())
	);
    label_reset_graph->setText("RESET");
    label_reset_graph->setFontId(dejaVuSansId);
    label_reset_graph->setFontSize(15.0f);
    label_reset_graph->setAlign(ALIGN_LEFT | ALIGN_MIDDLE);
    label_reset_graph->setMargin( Margin(
		6, 0,
		std::round(button_reset_graph->getHeight() / 2.0f) + 1, 0
    ) );

    wheel_oversample = std::make_unique<OversampleWheel>(this, Size<uint>(47, 26));
    wheel_oversample->setCallback(this);
    wheel_oversample->setRange(0, 4);

    label_oversample = std::make_unique<NanoLabel>(this, Size<uint>(85, 26));
    label_oversample->setText("OVERSAMPLE");
    label_oversample->setFontId(chivoBoldId);
    label_oversample->setFontSize(14.0f);
    label_oversample->setAlign(ALIGN_LEFT | ALIGN_MIDDLE);
    label_oversample->setMargin( Margin(
		0, 0,
		label_oversample->getHeight() / 2.0f, 0
	));
    
    positionWidgets(width, height);
}

GraphicDynamicsUI::~GraphicDynamicsUI()
{
}

void GraphicDynamicsUI::positionWidgets(uint width, uint height)
{
	const uint control_bar_h = fBottomBarVisible ? 102 : 0;
	const uint graph_bar_h = graph_bar->getHeight();

	// Graph Widget ---------------------------------------------------------//
	graph_widget->setSize(
		width - UIConfig::graph_bar_m * 2,
		height - UIConfig::graph_bar_m * 2 - control_bar_h - graph_bar_h );
	graph_widget->setAbsolutePos(UIConfig::graph_bar_m, UIConfig::graph_bar_m);

    const float graphBottom = graph_widget->getAbsoluteY()
		+ graph_widget->getHeight();

    graph_bar->setWidth(width);
    graph_bar->setAbsolutePos(0, graphBottom + UIConfig::graph_bar_m);
    graph_bar->setFillPaint( radialGradient(
		width / 2.0f,          // centre x
		graph_bar_h / 2.0f, //centre y
		graph_bar_h, // inner radius
		width / 2.0f,   // outer radius
		Color(71, 74, 80, 255), // inner colour
		Color(40, 42, 46, 255)  // outer colour
	) );

    const float graphBarMiddleY = graph_bar->getAbsoluteY()
		+ graph_bar->getHeight() / 2.0f;

	// Reset Graph Button ---------------------------------------------------//
    button_reset_graph->setAbsolutePos(
		20, graphBarMiddleY - button_reset_graph->getHeight() / 2.0f );
    label_reset_graph->setAbsolutePos(
		button_reset_graph->getAbsoluteX() + button_reset_graph->getWidth(),
		button_reset_graph->getAbsoluteY() );

	// Oversample Wheel -----------------------------------------------------//
    wheel_oversample->setAbsolutePos(
		width - wheel_oversample->getWidth() - 35,
		graphBarMiddleY - wheel_oversample->getHeight() / 2.0f );
    label_oversample->setAbsolutePos(
		wheel_oversample->getAbsoluteX() - label_oversample->getWidth(),
		wheel_oversample->getAbsoluteY() );
	
	// right-aligned controls
	const uint x_grid_wide = UIConfig::x_grid*1.5f;
	const uint right_edge = width - UIConfig::bar_m;

	const uint verwarp_x   = right_edge        - x_grid_wide;
	const uint horwarp_x   = verwarp_x         - x_grid_wide;
	const uint postgain_x  = horwarp_x         - x_grid_wide;
	const uint wetdry_x    = postgain_x        - UIConfig::x_grid;
	const uint pregain_x   = wetdry_x          - UIConfig::x_grid;

	// left-aligned
	const uint atk_x       = UIConfig::bar_m   + UIConfig::x_grid;
	const uint rls_x       = atk_x             + UIConfig::x_grid;
	const uint buttons_x   = rls_x             + UIConfig::x_grid;
	
	// DC Removal Toggle ----------------------------------------------------//
	sw_remove_dc->setAbsolutePos(buttons_x, height - 38);
	label_remove_dc->setAbsolutePos(
		buttons_x + sw_remove_dc->getWidth(), height - 38 );

	// Linearity Toggle -----------------------------------------------------//
	sw_linearity->setAbsolutePos(buttons_x+7, height-86);
    label_linearity->setAbsolutePos( buttons_x+29, height - 90 );

	// Pre Gain Knob --------------------------------------------------------//
    float align_label = 
		(label_pre->getWidth() - knob_pre->getWidth()) / 2.0f;
    knob_pre->setAbsolutePos(pregain_x, height - 90);
    label_pre->setAbsolutePos(pregain_x - align_label,
		height - label_pre->getHeight() - UIConfig::label_m );

	// Wet Dry Knob ---------------------------------------------------------//
	align_label = 
		(label_wet->getWidth() - knob_wet->getWidth()) / 2.0f;
	knob_wet->setAbsolutePos(wetdry_x, height - 90);
	label_wet->setAbsolutePos(wetdry_x - align_label,
		height - label_wet->getHeight() - UIConfig::label_m );

	// Post Gain Knob -------------------------------------------------------//
    align_label =
		(label_post->getWidth() - knob_post->getWidth()) / 2.0f;
    knob_post->setAbsolutePos(postgain_x, height - 90);
	label_post->setAbsolutePos(postgain_x - align_label,
		height - label_post->getHeight() - UIConfig::label_m );

	// Horizontal Warp Control ----------------------------------------------//
	align_label = (label_hor_warp->getWidth()
		- knob_hor_warp->getWidth()) / 2.0f;
	knob_hor_warp->setAbsolutePos(horwarp_x, height - 90);
	label_hor_warp->setAbsolutePos( horwarp_x- align_label,
		height - label_hor_warp->getHeight() - UIConfig::label_m );

    button_l_hor_warp_mode->setAbsolutePos(
		label_hor_warp->getAbsoluteX() -
			button_l_hor_warp_mode->getWidth(),
		label_hor_warp->getAbsoluteY() );
	button_r_hor_warp_mode->setAbsolutePos(
		label_hor_warp->getAbsoluteX()
			+ label_hor_warp->getWidth(),
		label_hor_warp->getAbsoluteY() );

	// Vertical Warp Control ------------------------------------------------//
    align_label = (label_ver_warp->getWidth()
		- knob_ver_warp->getWidth()) / 2.0f;

    knob_ver_warp->setAbsolutePos(verwarp_x, height - 90);
    label_ver_warp->setAbsolutePos( verwarp_x - align_label,
		height - label_ver_warp->getHeight() - UIConfig::label_m);

	button_l_ver_warp_mode->setAbsolutePos(
		label_ver_warp->getAbsoluteX() -
			button_l_ver_warp_mode->getWidth(),
		label_ver_warp->getAbsoluteY() );
    button_r_ver_warp_mode->setAbsolutePos(
		label_ver_warp->getAbsoluteX()
			+ label_ver_warp->getWidth(),
		label_ver_warp->getAbsoluteY() );

	// Attack Time Knob -----------------------------------------------------//
	align_label = 
		(label_attack->getWidth() - knob_attack->getWidth()) / 2.0f;
	knob_attack->setAbsolutePos(atk_x, height - 90);
	label_attack->setAbsolutePos(atk_x - align_label,
		height - label_attack->getHeight() - UIConfig::label_m );

	// Release Time Knob ----------------------------------------------------//
    align_label =
		(label_release->getWidth() - knob_release->getWidth()) / 2.0f;
    knob_release->setAbsolutePos(rls_x, height - 90);
	label_release->setAbsolutePos(rls_x- align_label,
		height - label_release->getHeight() - UIConfig::label_m );

	// Window Resize Handle -------------------------------------------------//
    handle_resize->setAbsolutePos(
		width - handle_resize->getWidth(),
		height - handle_resize->getHeight() );
}

void GraphicDynamicsUI::parameterChanged(uint32_t index, float value)
{
	switch (index) {
	case p_pre_gain:
		knob_pre->setValue(value);
		break;
	case p_wet:
		knob_wet->setValue(value);
		break;
	case p_post_gain:
		knob_post->setValue(value);
		break;
	case p_remove_dc:
		sw_remove_dc->setDown(value >= 0.50f);
		break;
	case p_linearity:
		sw_linearity->setDown(value >= 0.50f);
		break;
	case p_smooth:
		wheel_oversample->setValue(value);
		break;
	case p_hor_warp_mode:
		label_hor_warp->setSelectedIndex(std::round(value));
		break;
	case p_hor_warp_amt:
		knob_hor_warp->setValue(value);
		break;
	case p_ver_warp_mode:
		label_ver_warp->setSelectedIndex(std::round(value));
		break;
	case p_ver_warp_amt:
		knob_ver_warp->setValue(value);
		break;
	case p_atk_ms:
		knob_attack->setValue(value);
		break;
	case p_rls_ms:
		knob_release->setValue(value);
		break;
	case p_out:
		graph_widget->updateInput(value);
		break;
	default:
		break;
	}
	updateOnParamChange(index, value);
}

void GraphicDynamicsUI::updateOnParamChange(uint32_t index, float value)
{
	switch(index) {
	case p_linearity:
		label_linearity->setSelectedIndex(value >= 0.5f ? 1 : 0);
		break;
	case p_hor_warp_amt:
		graph_widget->setHorWarpAmt(value);
		break;
	case p_ver_warp_amt:
		graph_widget->setVerWarpAmt(value);
		break;
	case p_hor_warp_mode:
		graph_widget->setHorWarpMode((graphdyn::WarpMode)std::round(value));
		break;
	case p_ver_warp_mode:
		graph_widget->setVerWarpMode((graphdyn::WarpMode)std::round(value));
		break;
	case p_atk_ms:
		label_attack->setDisplayNumber(value);
		label_attack->repaint();
		break;
	case p_rls_ms:
		label_release->setDisplayNumber(value);
		label_release->repaint();
		break;
	default:
		break;
	}
}

void GraphicDynamicsUI::stateChanged(const char *key, const char *value)
{
    if (std::strcmp(key, "graph") == 0)
        graph_widget->rebuildFromString(value);

    repaint();
}

void GraphicDynamicsUI::onNanoDisplay()
{
	const float width = getWidth();
	const float height = getHeight();

	//background
	beginPath();

	rect(0.f, 0.f, width, height);
	fillColor(UIConfig::plugin_background);

	fill();

	closePath();

	//shadow at the bottom of the graph
	beginPath();

	const int shadowHeight = 8;
	const int shadowMargin = 2;

	const float graphBottom =
		graph_widget->getAbsoluteY() + graph_widget->getHeight();
	const float shadowTop = graphBottom + shadowMargin;
	const float shadowBottom = shadowTop + shadowHeight;

	rect(0.0f, shadowTop, getWidth(), shadowHeight);

	Paint gradient = linearGradient(
		0,
		shadowTop,
		0,
		shadowBottom,
		Color(21, 22, 30, 0),
		Color(21, 22, 30, 180)
	);
	fillPaint(gradient);

	fill();

	closePath();
}

void GraphicDynamicsUI::uiIdle()
{
}

bool GraphicDynamicsUI::onMouse(const MouseEvent &)
{
    return false;
}

void GraphicDynamicsUI::uiReshape(uint width, uint height)
{
	//setSize(width, height);
	positionWidgets(width, height);
}

void GraphicDynamicsUI::toggleBottomBarVisibility()
{
	fBottomBarVisible = !fBottomBarVisible;

	label_linearity->setVisible(fBottomBarVisible);
	sw_remove_dc->setVisible(fBottomBarVisible);
	knob_post->setVisible(fBottomBarVisible);
	knob_pre->setVisible(fBottomBarVisible);

	knob_hor_warp->setVisible(fBottomBarVisible);
	label_hor_warp->setVisible(fBottomBarVisible);
	button_l_hor_warp_mode->setVisible(fBottomBarVisible);
	button_r_hor_warp_mode->setVisible(fBottomBarVisible);

	knob_ver_warp->setVisible(fBottomBarVisible);
	label_ver_warp->setVisible(fBottomBarVisible);
	button_l_ver_warp_mode->setVisible(fBottomBarVisible);
	button_r_ver_warp_mode->setVisible(fBottomBarVisible);

	knob_wet->setVisible(fBottomBarVisible);
	label_post->setVisible(fBottomBarVisible);
	label_pre->setVisible(fBottomBarVisible);
	label_wet->setVisible(fBottomBarVisible);
	label_remove_dc->setVisible(fBottomBarVisible);

	positionWidgets(getWidth(), getHeight());
}

bool GraphicDynamicsUI::onKeyboard(const KeyboardEvent &ev)
{
	if (ev.press) {
		if (ev.key == 95) { //F11
			toggleBottomBarVisibility();
		}
	}

	return true;
}

void GraphicDynamicsUI::nanoSwitchClicked(NanoSwitch *nanoSwitch)
{
	const uint sw_id = nanoSwitch->getId();
	const int value = nanoSwitch->isDown() ? 1 : 0;

	setParameterValue(sw_id, value);
	updateOnParamChange(sw_id, value);
}

void GraphicDynamicsUI::nanoButtonClicked(NanoButton *nanoButton)
{
	if (nanoButton == button_reset_graph.get()) {
		graph_widget->reset();
		return;
	}

	bool horizontal = false;

	if (nanoButton == button_l_hor_warp_mode.get()) {
		label_hor_warp->goPrevious();
		horizontal = true;
	} else if (nanoButton == button_r_hor_warp_mode.get()) {
		label_hor_warp->goNext();
		horizontal = true;
	} else if (nanoButton == button_l_ver_warp_mode.get()) {
		label_ver_warp->goPrevious();
	} else if (nanoButton == button_r_ver_warp_mode.get()) {
		label_ver_warp->goNext();
	}

	if (horizontal) {
		const int index = label_hor_warp->getSelectedIndex();

		setParameterValue(p_hor_warp_mode, index);
		graph_widget->setHorWarpMode((graphdyn::WarpMode)index);
	} else {
		const int index = label_ver_warp->getSelectedIndex();

		setParameterValue(p_ver_warp_mode, index);
		graph_widget->setVerWarpMode((graphdyn::WarpMode)index);
	}
}

void GraphicDynamicsUI::nanoWheelValueChanged(NanoWheel *nanoWheel, const int value)
{
	const uint id = nanoWheel->getId();
	setParameterValue(id, value);
	updateOnParamChange(id, value);
}

void GraphicDynamicsUI::nanoKnobValueChanged(NanoKnob *nanoKnob, const float value)
{
	const uint id = nanoKnob->getId();
	setParameterValue(id, value);
	updateOnParamChange(id, value);
}

void GraphicDynamicsUI::resizeHandleMoved(int width, int height)
{
	setSize(width, height);
}

UI *createUI()
{
	return new GraphicDynamicsUI();
}

END_NAMESPACE_DISTRHO
