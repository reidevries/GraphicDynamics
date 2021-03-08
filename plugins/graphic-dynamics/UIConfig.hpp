#ifndef __UICONFIG_H_
#define __UICONFIG_H_

#include "DistrhoUI.hpp"
#include "Margin.hpp"
#include <string>

START_NAMESPACE_DISTRHO


// config for graphic dynamics UI Visual parameters
// m always means margin, w always means width, h always means height
// fg = foreground, bg = background
namespace UIConfig
{

// Window minimum width and height
extern uint min_w, min_h;

// graph positioning parameters
extern uint graph_bar_m;
extern uint graph_bar_h;

// label parameters
extern uint  label_m;
extern uint  label_w, label_h;
extern uint  label_font_size;
extern Color label_font_color;

// control bar parameters
extern uint control_bar_h;
extern uint x_grid;
extern uint bar_m;

// knob parameters
extern float knob_r, knob_mini_r, knob_gauge_w;
extern Color knob_gradient_stroke_i, knob_gradient_stroke_o;
extern Color knob_gradient_fill_i, knob_gradient_fill_o;
extern Color knob_gauge_stroke;

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
extern Color time_color;
extern Color gain_color;
extern Color wet_color;
extern Color warp_color;

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

// OversampleWheel parameters
extern float wheel_text_m;
extern float wheel_outline_w;
extern Color wheel_gradient_i, wheel_gradient_o;
extern Color wheel_outline_fill;
extern Color wheel_top_line_stroke;
//extern Color wheel_font_color;

// get label size as a Size object
auto labelSize() -> Size<uint>;
// get lr button size as a Size object
auto lrButtonSize() -> Size<uint>;
// get a smaller variation on label font size
auto labelFontSizeSmall() -> uint;

void load( std::string file_name ); // load parameters from a config file
};

END_NAMESPACE_DISTRHO

#endif // __UICONFIG_H_
