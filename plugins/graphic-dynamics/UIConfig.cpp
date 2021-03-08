#include "UIConfig.hpp"

START_NAMESPACE_DISTRHO

namespace UIConfig
{
// Window minimum width and height
uint  min_w = 1280;
uint  min_h = 420;

// graph positioning parameters
uint  graph_bar_m = 6;
uint  graph_bar_h = 42;

// label parameters
uint  label_m = 12;
uint  label_w = 96;
uint  label_h = 24;
uint  label_font_size = 16;
Color label_font_color = Color(255,255,255);

// control bar parameters
uint  x_grid = 100;
uint  bar_m = 16;

// knob radii
float knob_r = 27;
float knob_mini_r = 20;

// graph tension handle parameters
float tension_handle_radius = 4.5f;
float tension_handle_stroke_w = 2.0f;
Color tension_handle_normal = Color(228, 104, 181, 255);
Color tension_handle_focus = Color(255, 150, 255, 255);

// graph vertex parameters
float vertex_radius = 7.0f;
float vertex_stroke_w = 2.0f;
Color vertex_fill_normal = Color(255, 255, 255, 255);
Color vertex_fill_focus = Color(255, 255, 255, 255);
Color vertex_halo = Color(0, 0, 0, 255);
Color vertex_stroke_normal = Color(0, 0, 0, 255);
Color vertex_stroke_focus = Color(0,0,0, 255);


// colors representing various abstract concepts (used for knob colours)
Color time_color = Color(136, 228, 255);
Color gain_color = Color(250, 250, 250);
Color wet_color  = Color(255, 255, 169);
Color warp_color = Color(143, 255, 147);

// plugin colours
Color plugin_background = Color(42, 44, 47, 255);
Color top_border = Color(0, 0, 0, 255);

// graph area parameters
Color graph_edges_background_normal = Color(169, 29, 239, 100);
Color graph_edges_background_focus = Color(255, 221, 76, 100);

// GraphWidget parameters
uint  graph_margin = 16;
Color graph_m_fill = Color(33, 32, 39, 255);
Color graph_m_stroke = Color(100, 100, 100, 255);
float graph_m_stroke_w = 1.0f;
Color graph_top_stroke = Color(0, 0, 0);
float graph_top_w = 2.0f;
float graph_edge_w = 2.0f;
Color graph_edge_fg_normal = Color(239, 61, 227, 255);
Color graph_edge_fg_focus = Color(255, 150, 255, 255);

// GraphWidgetInner parameters
Color grid_stroke_fg = Color(103, 98, 102, 255);
Color grid_stroke_bg = Color(25, 24, 26, 255);
Color grid_stroke_sub = Color(27, 27, 27, 255);
Color grid_stroke_middle = Color(153, 148, 152, 255);
Color grid_stroke_alignment = Color(200, 200, 200, 180);
Color grid_fill_bg = Color(40, 40, 47, 255);
Color grid_gradient_i = Color(60, 60, 60, 20);
Color grid_gradient_o = Color(60, 60, 60, 60);
Color in_out_labels = Color(255, 255, 255, 125);

Color volume_indicator_stroke = Color(255, 255, 255, 180);
Color playhead_fill = Color(255, 255, 255, 180);
Color playhead_stroke = Color(255, 255, 255, 180);

// MenuWidget parameters
float menu_font_item_size = 17.0f;
float menu_font_section_size = 14.0f;
Color menu_border_color = Color(10, 10, 10);
Color menu_background_color = Color(39, 39, 39);
Color menu_background_hover_color = Color(255, 255, 255);
Color menu_font_item_color = Color(255, 255, 255);
Color menu_font_item_hover_color = Color(39, 39, 39);
Color menu_font_section_color = Color(100, 100, 100);

// OversampleWheel parameters
float wheel_text_m     = 3.f;
float wheel_outline_w  = 2.f;
Color wheel_gradient_i = Color(54, 52, 88);
Color wheel_gradient_o = Color(38, 37, 51);
Color wheel_outline_fill = Color(27, 27, 27);
Color wheel_top_line_stroke = Color(72, 137, 208, 150);
//Color wheel_font_color = Color(193, 219, 240);

// get label size as a Size object
auto labelSize() -> Size<uint>
{
	return Size<uint>(label_w, label_h);
}

// get lr button size as a Size object
auto lrButtonSize() -> Size<uint>
{
	return Size<uint>(label_h, label_h);
}

// get a smaller variation on label font size
auto labelFontSizeSmall() -> uint
{
	return label_font_size*7/8;
}

} // end namespace UIConfig

END_NAMESPACE_DISTRHO