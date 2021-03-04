#ifndef __UICONFIG_H_
#define __UICONFIG_H_

#include "DistrhoUI.hpp"
#include "Margin.hpp"
#include <string>

START_NAMESPACE_DISTRHO


// config for graphic dynamics UI Visual parameters
// m always means margin, w always means width, h always means height
// fg = foreground, bg = background
namespace UIConfig {

// Window minimum width and height
extern uint min_w, min_h;

// graph positioning parameters
extern uint graph_bar_m;
extern uint graph_bar_h;

// label positioning parameters
extern uint label_m;
extern uint label_w, label_h;

// control bar parameters
extern uint x_grid;
extern uint bar_m;

// knob sizes
extern float knob_r, knob_mini_r;

// graph tension handle parameters
extern float tension_handle_radius;
extern float tension_handle_stroke_w;
extern Color tension_handle_normal;
extern Color tension_handle_focus;

// graph vertex parameters
extern float vertex_radius;
extern float vertex_stroke_w;
extern Color vertex_fill_normal;
extern Color vertex_fill_focus;
extern Color vertex_halo;
extern Color vertex_stroke_normal;
extern Color vertex_stroke_focus;


// colors representing various abstract concepts (used for knob colours)
extern Color time_col;
extern Color gain_col;
extern Color wet_col ;
extern Color warp_col;

// plugin colours
extern Color plugin_background;
extern Color top_border;

// graph area parameters
extern Color graph_edges_background_normal;
extern Color graph_edges_background_focus;

// GraphWidget parameters
extern uint  graph_margin;
extern Color graph_m_fill;
extern Color graph_m_stroke;
extern float graph_m_stroke_w;
extern Color graph_top_stroke;
extern float graph_top_w;
extern float graph_edge_w;
extern Color graph_edge_fg_normal;
extern Color graph_edge_fg_focus;

// GraphWidgetInner parameters
extern Color grid_stroke_fg;
extern Color grid_stroke_bg;
extern Color grid_stroke_sub;
extern Color grid_stroke_middle;
extern Color grid_stroke_alignment;
extern Color grid_fill_bg;
extern Color grid_gradient_i, grid_gradient_o;
extern Color in_out_labels;

extern Color volume_indicator_stroke;
extern Color playhead_fill;
extern Color playhead_stroke;

// MenuWidget parameters
extern float menu_font_item_size, menu_font_section_size;
extern Color menu_border_color;
extern Color menu_background_color;
extern Color menu_background_hover_color;
extern Color menu_font_item_color;
extern Color menu_font_item_hover_color;
extern Color menu_font_section_color;

inline auto labelSize() -> Size<uint>
{
	return Size<uint>(label_w, label_h);
}

inline auto lrButtonSize() -> Size<uint>
{
	return Size<uint>(label_h, label_h);
}

void load( std::string file_name ); // load parameters from a config file
};

END_NAMESPACE_DISTRHO

#endif // __UICONFIG_H_
