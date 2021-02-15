#include "EnvelopeFollower.hpp"
#include "Mathf.hpp"
#include <cmath>
#define PI 3.1415926535897932384626

START_NAMESPACE_DISTRHO

EnvelopeFollower::EnvelopeFollower( double rate_hz, uint32_t num_samples )
{
	setRate(rate_hz);
	setSmoothingHz(16384);

	allocBuf( num_samples );
	for ( uint8_t chan = 0; chan < NUM_CHANNELS; ++chan ) {
		cap_v[chan] = 0.0f; //set initial capacitor voltage to zero
	}
};

void EnvelopeFollower::setRate( double rate_hz ) 
{
	this->rate_hz = rate_hz;
	this->dt_ms = 1000/rate_hz;
}

void EnvelopeFollower::setSmoothingHz( uint f_hz )
{
	this->smoothing_hz = f_hz;
	smoothing_filter.reset();
	smoothing_filter.setup( 8, rate_hz, f_hz);
}

auto EnvelopeFollower::getEnvelope( const float * const * audio ) -> float**
{
	for ( uint32_t i = 0; i < buf_c; ++i ) {
		for ( uint8_t chan = 0; chan < NUM_CHANNELS; ++chan ) {
			const float rect_v = std::abs(audio[chan][i]);
			chargeCapacitor( rect_v, chan );
			buf[chan][i] = cap_v[chan];
		}
	}

	smoothing_filter.process( buf_c, buf );
	return buf;
}

auto EnvelopeFollower::getEnvelope( uint32_t num_samples,
	const float * const * audio ) -> float**
{
	if ( num_samples != buf_c ) {
		deleteBuf();
		allocBuf( num_samples );
	}
	
	return getEnvelope( audio );
}

auto EnvelopeFollower::getEnvelope( uint32_t num_samples, double rate_hz,
	const float * const * audio ) -> float**
{
	if ( rate_hz != this->rate_hz ) setRate(rate_hz);
	return getEnvelope( num_samples, audio );
}

void EnvelopeFollower::deleteBuf( )
{
	for ( uint8_t chan = 0; chan < NUM_CHANNELS; ++chan ) {
		delete[] buf[chan];
	}
	delete[] buf;
	
}

void EnvelopeFollower::allocBuf( uint32_t num_samples )
{
	buf_c = num_samples;
	buf = new float*[NUM_CHANNELS];

	for ( uint8_t chan = 0; chan < NUM_CHANNELS; ++chan ) {
		buf[chan] = new float[buf_c];
	}
}

void EnvelopeFollower::chargeCapacitor( const float in_v, const int channel )
{
	const float delta_v = in_v-cap_v[channel];

	float speed_ms;
	float lin_change;
	if ( delta_v > 0 ) {
		speed_ms = atk_ms;
		lin_change = std::min<float>( dt_ms/atk_ms, delta_v);
	} else {
		speed_ms = rls_ms;
		lin_change = std::max<float>(-dt_ms/rls_ms, delta_v);
	}
	const float exp_change = std::exp( -( (2*PI*dt_ms)/speed_ms ) );
	
	const float exp_new_v = in_v*(1-exp_change) + cap_v[channel]*exp_change;
	const float lin_new_v = cap_v[channel]  + lin_change;
	
	cap_v[channel] = exp_new_v*(1-linearity) + lin_new_v*linearity;
}

EnvelopeFollower::~EnvelopeFollower()
{
	deleteBuf();
}

END_NAMESPACE_DISTRHO
