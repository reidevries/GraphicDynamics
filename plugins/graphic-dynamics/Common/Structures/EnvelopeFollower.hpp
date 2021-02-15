/*
	created by rei de vries 2020-12-28
	class implements an envelope follower, tracks the peak of the signal.
	has some more parameters than a regular envelope follower:
	atk_ms, rls_ms:
		attack and release, same as every other envelope follower
	linearity:
		fades between a capacitor-style exponential response to a fully linear
		response
	smoothing:
		a regular lowpass filter is applied to the envelope to remove high
		frequency components when using very fast attack/release settings
*/

#ifndef ENVELOPE_FOLLOWER_HPP
#define ENVELOPE_FOLLOWER_HPP

#include "DspFilters/Dsp.h"
#include "src/DistrhoDefines.h"
#include "extra/LeakDetector.hpp"

START_NAMESPACE_DISTRHO

class EnvelopeFollower
{

public:
	EnvelopeFollower(double rate_hz, uint32_t num_samples);
	EnvelopeFollower() : EnvelopeFollower( 44100, 512 ) {}
	~EnvelopeFollower();

	float linearity = 0.0f; // 1.0 is linear, 0.0 is exponential
	
	float atk_ms = 20.0f;
	float rls_ms = 100.0f;

	void setRate( double rate_hz );
	void setSmoothingHz( uint f_hz );
	uint getSmoothingHz() { return smoothing_hz; }

	float **getEnvelope( uint32_t num_samples,
		const float * const * audio );
	float **getEnvelope( uint32_t num_samples, double rate_hz,
		const float * const * audio );
	float **getEnvelope( const float * const * audio );

private:
	static const uint8_t NUM_CHANNELS = 2;

	float **buf = new float*[NUM_CHANNELS];
	uint32_t buf_c;
	void deleteBuf();
	void allocBuf(uint32_t num_samples);

	double rate_hz = 44100.0f;
	double dt_ms = 1/44100.0f;

	float cap_v[NUM_CHANNELS];
	void chargeCapacitor(const float in_v, const int channel);
	uint smoothing_hz = 10000;
	Dsp::SimpleFilter<
		Dsp::Butterworth::LowPass<8>, NUM_CHANNELS
	> smoothing_filter;

	DISTRHO_LEAK_DETECTOR(EnvelopeFollower)
};

END_NAMESPACE_DISTRHO
#endif
