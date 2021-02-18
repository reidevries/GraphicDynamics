#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#include "src/DistrhoDefines.h"

#if defined(DISTRHO_OS_WINDOWS)
#include <windows.h>
#include <shlobj.h>
#else
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#endif

#include "Color.hpp"
#include "StringManipulation.hpp"

#include "Config.hpp"

START_NAMESPACE_DISTRHO

namespace CONFIG_NAMESPACE
{

enum ColorType
{
    kColorTypeRGB = 0,
    kColorTypeRGBA,
    kColorTypeHSL,
    kColorTypeHSLA
};

bool isLoaded = false;

Color grid_foreground = Color(103, 98, 102, 255);
Color grid_background = Color(25, 24, 26, 255);
Color sub_grid = Color(27, 27, 27, 255);
Color graph_background = Color(40, 40, 47, 255);
Color grid_middle_line_horizontal = Color(103, 98, 102, 255);
Color grid_middle_line_vertical = Color(103, 98, 102, 255);
Color in_out_labels = Color(255, 255, 255, 125);

Color alignment_lines = Color(200, 200, 200, 180);
Color input_volume_indicator = Color(255, 255, 255, 180);

Color playhead_circle_fill = Color(255, 255, 255, 180);
Color playhead_circle_stroke = Color(255, 255, 255, 180);

float graph_edges_stroke_width = 2.0f;
Color graph_edges_background_normal = Color(169, 29, 239, 100);
Color graph_edges_background_focused = Color(255, 221, 76, 100);

Color graph_edges_foreground_normal = Color(239, 61, 227, 255);
Color graph_edges_foreground_focused = Color(255, 150, 255, 255);

Color graph_gradient_icol = Color(60, 60, 60, 20);
Color graph_gradient_ocol = Color(60, 60, 60, 60);

Color vertex_fill_normal = Color(255, 255, 255, 255);
Color vertex_fill_focused = Color(255, 255, 255, 255);

Color vertex_halo = Color(0, 0, 0, 255);

Color vertex_stroke_normal = Color(0, 0, 0, 255);
Color vertex_stroke_focused = Color(0,0,0, 255);

float vertex_radius = 7.0f;
float vertex_stroke_width = 2.0f;

Color tension_handle_normal = Color(228, 104, 181, 255);
Color tension_handle_focused = Color(255, 150, 255, 255);

float tension_handle_radius = 4.5f;
float tension_handle_stroke_width = 2.0f;

Color plugin_background = Color(42, 44, 47, 255);
Color graph_margin = Color(33, 32, 39, 255);
Color top_border = Color(0, 0, 0, 255);
Color side_borders = Color(100, 100, 100, 255);

Color menu_border_color = Color(10, 10, 10);
Color menu_background_color = Color(39, 39, 39);
Color menu_background_selected_color = Color(255, 255, 255);
Color menu_font_item_color = Color(255, 255, 255);
Color menu_font_section_color = Color(100, 100, 100);

} // namespace CONFIG_NAMESPACE

END_NAMESPACE_DISTRHO
