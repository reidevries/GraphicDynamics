#include "RCFilter.hpp"
#include <cmath>

START_NAMESPACE_DISTRHO

RCFilter::RCFilter(const double rate_hz, const double f_hz, const float init_v)
	: cap_v(init_v)
{
	setParamsHz( rate_hz, f_hz );
}

void RCFilter::setParamsHz(const double rate_hz, const double f_hz)
{
	if ( this->rate_hz != rate_hz || this->f_hz != f_hz ) {
		this->rate_hz = std::abs(rate_hz);
		this->f_hz = std::abs(f_hz);
		recalculateCoeff();
	}
}

void RCFilter::setParamsMs(const double rate_hz, const double t_ms)
{
	const double t_ms_hz = 1000.0f/t_ms;
	setParamsHz( rate_hz, t_ms_hz );
}

auto RCFilter::chargeCap(const float in_v) -> float
{
	const float diff_v = in_v-cap_v;
	const float lin_delta_v = std::copysign(
		std::fmin(f_hz/rate_hz, std::abs(diff_v)), diff_v );

	const float exp_new_v = in_v * (1-coeff) + cap_v * coeff;
	const float lin_new_v = cap_v + lin_delta_v;
	cap_v = exp_new_v * (1-linearity) + lin_new_v * linearity;

	return cap_v;
}

void RCFilter::recalculateCoeff()
{
	coeff = std::exp( -2.0f * M_PI * f_hz / rate_hz );
}

END_NAMESPACE_DISTRHO
