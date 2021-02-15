#ifndef GRAPHIC_DYNAMICS_PARAMETERS_HPP
#define GRAPHIC_DYNAMICS_PARAMETERS_HPP

#include "src/DistrhoDefines.h"

START_NAMESPACE_DISTRHO

enum Parameters
{
    p_pre_gain = 0,
    p_wet,
    p_post_gain,
    p_remove_dc,
    p_smooth,
    p_hor_warp_mode,
    p_hor_warp_amt,
    p_ver_warp_mode, 
    p_ver_warp_amt,
	p_atk_ms,
	p_rls_ms,
	p_linearity,
    p_out,
    p_c
};

END_NAMESPACE_DISTRHO

#endif
