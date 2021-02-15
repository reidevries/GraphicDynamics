/* RCFilter.hpp by rei de vries created 2020-12-30
 * simulates an RC filter, but with programmable linearity and frequency
 */ 

 
#ifndef RCFILTER_HPP
#define RCFILTER_HPP

#include "src/DistrhoDefines.h"
#include "extra/LeakDetector.hpp"

START_NAMESPACE_DISTRHO

class RCFilter
{
public:
	RCFilter(const double rate_hz, const double f_hz, const float init_v);
	RCFilter() : RCFilter( 44100, 20000, 0.0f ) {} // default values

	float linearity = 0.0f;

	void setParamsHz(const double rate_hz, const double f_hz);
	void setParamsMs(const double rate_hz, const double t_ms);
	void setRateHz(const double rate_hz) { setParamsHz(rate_hz, this->f_hz); }
	void setFHz(const double f_hz) { setParamsHz(this->rate_hz, f_hz); }
	void setTMs(const double t_ms) { setParamsMs(this->rate_hz, t_ms); }

	float chargeCap(const float in_v);
	float getCapV() const { return cap_v; }
	void dischargeCap(const float v) { cap_v = v; }

private:
	float cap_v;
	float rate_hz;
	float f_hz;
	double coeff;
	void recalculateCoeff();

	DISTRHO_LEAK_DETECTOR(RCFilter)
};

END_NAMESPACE_DISTRHO
#endif
