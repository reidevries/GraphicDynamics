#include <map>
#include <vector>
#include <cmath>
#include <stdlib.h>     /* atof */

#include "gnuplot-iostream/gnuplot-iostream.h"
#include "../RCFilter.hpp"

float genSquare( const double x )
{
	return sin(x) > 0 ? +1.f : -1.f ;
}

float genSaw( const double x )
{
	return (50 - static_cast<int>(x*100) % 100)/50.f;
}

float genShapedSine( const double x )
{
	return (6.f/2.f) * std::abs( 2.f/3.f - std::abs( sin(x) + 1.f/3.f ) )-1;
}

int main(int argc, char** argv) {
	float f_hz = 1;
	float linearity = 0;
	float time_s = 1;
	if (argc > 1) {
		f_hz = std::atof(argv[1]);
		if (argc > 2) linearity = std::atof(argv[2]);
		if (argc > 3) time_s = std::atof(argv[3]);
	} else {
		std::cout << "usage: TestRCFilter frequency_hz linearity time_s";
	}

	std::cout << "Using frequency = " << f_hz << "hz, "
		<< " linearity = " << linearity << ", "
		<< " graph time = " << time_s << " seconds"
		<< std::endl;
		
	double rate_hz = 44100;
	uint32_t num_samples = rate_hz * time_s;
	RCFilter rc;
	rc.setParamsHz( rate_hz, f_hz );
	rc.linearity = linearity;

	Gnuplot gp;

	std::vector<std::pair<double, double> > xy_pts_A;
	std::vector<std::pair<double, double> > xy_pts_B;

	for (uint32_t i = 0; i < num_samples; ++i) {
		double x = i/rate_hz;
		double y = genSquare(100*x*x);
		if (i % static_cast<int>(rate_hz) < rate_hz/3) {
			y = genSaw(100*x*x);
		} else if ( i % static_cast<int>(rate_hz) < 2*rate_hz/3) {
			y = genShapedSine(60*x*x);
		}
		xy_pts_A.push_back(std::make_pair(x, y));
		double y_filtered = rc.chargeCap( y );
		xy_pts_B.push_back(std::make_pair(x, y_filtered));
	}

	gp << "set xrange [0:1]\nset yrange [-1.2:1.2]\n";
	// Data will be sent via a temporary file.  These are erased when you call
	// gp.clearTmpfiles() or when gp goes out of scope.  If you pass a filename
	// (e.g. "gp.file1d(pts, 'mydata.dat')"), then the named file will be created
	// and won't be deleted (this is useful when creating a script).
	gp << "plot" << gp.file1d(xy_pts_A) << "with lines title 'input',"
		<< gp.file1d(xy_pts_B) << "with lines title 'RC'" << std::endl;

#ifdef _WIN32
	// For Windows, prompt for a keystroke before the Gnuplot object goes out of scope so that
	// the gnuplot window doesn't get closed.
	std::cout << "Press enter to exit." << std::endl;
	std::cin.get();
#endif
}
