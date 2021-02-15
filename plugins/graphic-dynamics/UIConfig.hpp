#ifndef __UICONFIG_H_
#define __UICONFIG_H_

#include "DistrhoUI.hpp"

START_NAMESPACE_DISTRHO

//config for graphic dynamics UI Visual parameters
struct UIConfig {
	uint min_w = 1280;
	uint min_h = 420;

	uint graph_m = 8;
	uint graph_bar_m = 6;
	uint graph_bar_h = 42;

	uint label_m = 12;
	uint label_w = 96;
	uint label_h = 24;

	uint x_grid = 100;
	uint bar_m = 16;

	Size<uint> knob_s = Size<uint>(54,54);

	Color time_col = Color(136, 228, 255);
	Color gain_col = Color(250, 250, 250);
	Color wet_col  = Color(255, 255, 169);
	Color warp_col = Color(143, 255, 147);

	auto labelSize() const -> Size<uint>
	{
		return Size<uint>(label_w, label_h);
	}

	auto lrButtonSize() const -> Size<uint>
	{
		return Size<uint>(label_h, label_h);
	}
};

END_NAMESPACE_DISTRHO

#endif // __UICONFIG_H_
