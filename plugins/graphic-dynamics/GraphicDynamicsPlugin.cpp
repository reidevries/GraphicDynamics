#include "DistrhoPlugin.hpp"
#include "extra/Mutex.hpp"
#include "extra/ScopedPointer.hpp"

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "GraphicDynamicsParameters.hpp"
#include "Graph.hpp"
#include "Mathf.hpp"
#include "RCFilter.hpp"

#include "DspFilters/Dsp.h"

START_NAMESPACE_DISTRHO

class GraphicDynamics : public Plugin
{

	const static int NUM_CHANNELS = 2;

public:
	GraphicDynamics() : Plugin(p_c, 0, 1)
	{
		for ( uint8_t c = 0; c < NUM_CHANNELS; ++c ) {
			dc_filter[c].setFHz(8);
		}
	}

protected:

	auto getLabel() const noexcept -> const char* override 
	{
		return "Graphic Dynamics";
	}

	auto getDescription() const noexcept -> const char* override
	{
		return "Like a compressor or gate,"
			"but define an arbitrary gain reduction curve!";
	}

	auto getMaker() const noexcept -> const char* override
	{
		return "Rei de Vries";
	}

	auto getHomePage() const noexcept -> const char* override
	{
		return "https://github.com/reidevries";
	}

	auto getLicense() const noexcept -> const char* override
	{
		return "GPL v3+";
	}

	auto getVersion() const noexcept -> uint32_t override
	{
		return d_version(0, 1, 0);
	}

	auto getUniqueId() const noexcept -> int64_t override
	{
		return d_cconst('s', 'W', 's', 'p');
	}

	void activate() override
	{
		rate_hz = getSampleRate();
		smoother.reset();
		smoother.setup( 8, rate_hz, 16384 );
	}

	void initParameter(uint32_t index, Parameter &parameter) override
	{
		if (index >= p_c)
			return;

		switch (index) {
		case p_pre_gain:
			parameter.name = "Pre Gain";
			parameter.symbol = "pregain";
			parameter.ranges.min = 0.0f;
			parameter.ranges.max = 2.0f;
			parameter.ranges.def = 1.0f;
			parameter.hints = kParameterIsAutomable
				| kParameterIsLogarithmic;
			break;
		case p_wet:
			parameter.name = "Wet";
			parameter.symbol = "wet";
			parameter.ranges.min = 0.0f;
			parameter.ranges.max = 1.0f;
			parameter.ranges.def = 1.0f;
			parameter.hints = kParameterIsAutomable;
			break;
		case p_post_gain:
			parameter.name = "Post Gain";
			parameter.symbol = "postgain";
			parameter.ranges.min = 0.0f;
			parameter.ranges.max = 1.0f;
			parameter.ranges.def = 1.0f;
			parameter.hints = kParameterIsAutomable
				| kParameterIsLogarithmic;
			break;
		case p_remove_dc:
			parameter.name = "Remove DC Offset";
			parameter.symbol = "removedc";
			parameter.ranges.min = 0.0f;
			parameter.ranges.max = 1.0f;
			parameter.ranges.def = 1.0f;
			parameter.hints = kParameterIsAutomable
				| kParameterIsBoolean
				| kParameterIsInteger;
			break;
		case p_smooth:
			//16384  8192 4096 2048 1024 512 256 128 64 32 16
			parameter.name = "Envelope Smoothing";
			parameter.symbol = "smooth";
			parameter.ranges.min = 16.0f;
			parameter.ranges.max = 16384.0f;
			parameter.ranges.def = 16384.0f;
			parameter.hints = kParameterIsAutomable
				| kParameterIsInteger
				| kParameterIsLogarithmic;
			break;
		case p_hor_warp_mode:
			//None, Bend +, Bend -, Bend +/-, Skew +, Skew -, Skew +/-
			parameter.name = "H Warp Mode";
			parameter.symbol = "hwarpmode";
			parameter.ranges.min = 0.0f;
			parameter.ranges.max = 6.0f;
			parameter.ranges.def = 0.0f;
			parameter.hints = kParameterIsAutomable
				| kParameterIsInteger;
			break;
		case p_hor_warp_amt:
			parameter.name = "H Warp Amount";
			parameter.symbol = "hwarpamt";
			parameter.ranges.min = 0.0f;
			parameter.ranges.max = 1.0f;
			parameter.ranges.def = 0.0f;
			parameter.hints = kParameterIsAutomable;
			break;
		case p_ver_warp_mode:
			//None, Bend +, Bend -, Bend +/-, Skew +, Skew -, Skew +/-
			parameter.name = "V Warp Mode";
			parameter.symbol = "vwarpmode";
			parameter.ranges.min = 0.0f;
			parameter.ranges.max = 6.0f;
			parameter.ranges.def = 0.0f;
			parameter.hints = kParameterIsAutomable
				| kParameterIsInteger;
			break;
		case p_ver_warp_amt:
			parameter.name = "V Warp Amount";
			parameter.symbol = "vwarpamt";
			parameter.ranges.min = 0.0f;
			parameter.ranges.max = 1.0f;
			parameter.ranges.def = 0.0f;
			parameter.hints = kParameterIsAutomable;
			break;
		case p_atk_ms:
			parameter.name = "Envelope Attack Time (ms)";
			parameter.symbol = "atkms";
			parameter.ranges.min = 0.1f;
			parameter.ranges.max = 10000.0f;
			parameter.ranges.def = 20.0f;
			parameter.hints = kParameterIsAutomable
				| kParameterIsLogarithmic;
			break;
		case p_rls_ms:
			parameter.name = "Envelope Attack Time (ms)";
			parameter.symbol = "rlsms";
			parameter.ranges.min = 0.1f;
			parameter.ranges.max = 10000.0f;
			parameter.ranges.def = 100.0f;
			parameter.hints = kParameterIsAutomable
				| kParameterIsLogarithmic;
			break;
		case p_linearity:
			parameter.name = "Envelope Linearity";
			parameter.symbol = "linearity";
			parameter.ranges.min = 0.0f;
			parameter.ranges.max = 1.0f;
			parameter.ranges.def = 0.0f;
			parameter.hints = kParameterIsAutomable;
			break;
		case p_out:
			parameter.name = "Out";
			parameter.symbol = "out";
			parameter.hints = kParameterIsOutput;
			parameter.ranges.def = 0.0f;
			break;
		}

		params[index] = parameter.ranges.def;
		params_smooth[index] = RCFilter(
			getSampleRate(),
			20,
			parameter.ranges.def
		);
	}

	auto getParameterValue(uint32_t index) const -> float override
	{
		return params[index];
	}

	void setParameterValue(uint32_t index, float value) override
	{
		params[index] = value;
	}

	void initState(uint32_t index, String &stateKey, String &defaultStateValue)
		override
	{
		switch (index)
		{
		case 0:
			stateKey = "graph";
			break;
		}

		// state for initializing the graph
		defaultStateValue =
			String("0x0p+0,0x0p+0,0x0p+0,0;0x1p+0,0x1p+0,0x0p+0,0;");
	}

	void setState(const char *key, const char *value) override
	{
		const MutexLocker cml(mutex);

		if (std::strcmp(key, "graph") == 0) {
			tempLineEditor.deserialize(value);
			must_copy_line_editor = true;
		}
	}

	void run(const float **inputs, float **outputs, uint32_t num_samples)
		override
	{
		// update the status of the graph line editor
		if (mutex.tryLock()) {
			if (must_copy_line_editor) {
				lineEditor = tempLineEditor;

				for (int i = 0; i < lineEditor.getNumVertices(); ++i)
				{
					lineEditor.getVertexAtIndex(i)->setGraphPtr(&lineEditor);
				}

				must_copy_line_editor = false;
			}
		}

		// find the envelope of the input signal
		float** env = followEnvelope( inputs, num_samples,
			params[p_atk_ms], params[p_rls_ms] );

		// get the smoothing frequency and update the filter if it has changed
		const uint32_t smoother_f_hz_new
			= static_cast<uint32_t>(params[p_smooth]);
		if ( smoother_f_hz_new != smoother_f_hz ) {
			smoother_f_hz = smoother_f_hz_new;
			smoother.reset();
			smoother.setup( 8, rate_hz, smoother_f_hz );
		}

		// apply the smoother to the envelope
		smoother.process( num_samples, env );

		// update the values of the warp modes and amounts
		graphdyn::WarpMode hor_warp_mode = (graphdyn::WarpMode) std::round( 
			params[p_hor_warp_mode] );
		lineEditor.setHorWarpMode(hor_warp_mode);
		graphdyn::WarpMode ver_warp_mode = (graphdyn::WarpMode) std::round(
			params[p_ver_warp_mode] );
		lineEditor.setVerWarpMode(ver_warp_mode);

		const bool must_remove_dc = params[p_remove_dc] > 0.5f;
		if (!must_remove_dc) {
			dc_filter[0].dischargeCap(0.0f);
			dc_filter[1].dischargeCap(0.0f);
		}

		// apply the gain modulation effect to each sample of the input
		float env_peak = 0.0f; // find the max value of env
		for ( uint32_t i = 0; i < num_samples; ++i ) {
			lineEditor.setHorWarpAmt( getSmoothParam(p_hor_warp_amt) );
			lineEditor.setVerWarpAmt( getSmoothParam(p_ver_warp_amt) );

			for ( uint8_t c = 0; c < NUM_CHANNELS; ++c ) {
				const float in_c = inputs[c][i];

				const float pre_gain = getSmoothParam( p_pre_gain );
				// add very small float to prevent division by zero
				const float env_c = env[c][i] * pre_gain + 0.000001f;
				env_peak = std::fmax( env_peak, env_c );
				const float gain_mod = pre_gain * getGraphValue(env_c)/env_c;

				const float wet = getSmoothParam( p_wet );
				const float dry = 1.0f - wet;
				outputs[c][i] = in_c * ( dry + wet*gain_mod );
				outputs[c][i] *= getSmoothParam( p_post_gain );

				if (must_remove_dc) {
					outputs[c][i] -= dc_filter[c].chargeCap(outputs[c][i]);
				}
			}
		}

		updateInputIndicatorPos(env_peak);
		setParameterValue(p_out, env_indicator_pos);

		mutex.unlock();
	}



private:
	float params[p_c];
	RCFilter params_smooth[p_c];
	RCFilter env_followers[NUM_CHANNELS];
	RCFilter dc_filter[NUM_CHANNELS];
	Dsp::SimpleFilter<
		Dsp::Butterworth::LowPass<8>, NUM_CHANNELS
	> smoother;
	uint smoother_f_hz = 16384;
	uint rate_hz = 44100;

	graphdyn::Graph lineEditor;
	graphdyn::Graph tempLineEditor;
	bool must_copy_line_editor = false;

	float env_indicator_pos;

	Mutex mutex;

	// use this function exactly once per sample, or else the cutoff frequency
	// will be incorrect
	auto getSmoothParam( const int index ) -> float
	{
		return params_smooth[index].chargeCap(params[index]);
	}

	auto followEnvelope( const float **in, uint32_t num_samples,
		const double atk_ms, const double rls_ms ) -> float**
	{
		float** env = new float*[NUM_CHANNELS];
		for ( int c = 0; c < NUM_CHANNELS; ++c ) {
		
			env[c] = new float[num_samples];
			for ( uint32_t i = 0; i < num_samples; ++i ) {
				const float in_v = in[c][i];
				
				//TODO: only change time when the cap_v crosses in_v
				// rather than every single frame
				if ( in_v > env_followers[c].getCapV() ) {
					env_followers[c].setTMs( atk_ms );
				} else {
					env_followers[c].setTMs( rls_ms );
				}
				
				env[c][i] = env_followers[c].chargeCap( in[c][i] );
			}
			
		}
		return env;
	}

	auto calculateValueOutsideGraph(const float value) -> float
	{
		return value * lineEditor.getVertexAtIndex(
			lineEditor.getNumVertices() - 1)->getY();
	}

	auto getGraphValue(float input) -> float
	{
		const float abs_in = std::abs(input);

		if (abs_in > 1.0f) {
			return calculateValueOutsideGraph(input);
		}

		return lineEditor.getValueAt(input);
	}

	void updateInputIndicatorPos(const float peak)
	{
		const float dead_zone = 0.001f;

		env_indicator_pos = wolf::clamp(peak, -dead_zone, 1.0f);
	}

	DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphicDynamics)
};

Plugin *createPlugin() { return new GraphicDynamics(); }


END_NAMESPACE_DISTRHO
