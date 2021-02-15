#include "Graph.hpp"
#include "Mathf.hpp"

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>

START_NAMESPACE_DISTRHO

namespace graphdyn
{

Vertex::Vertex() :
	x(0), y(0),
	x_dirty(true), y_dirty(true),
	tension(0),
	h_warp(0.0f), v_warp(0.0f),
	h_warp_graph(0.0f), v_warp_graph(0.0f),
	h_warp_mode(None), v_warp_mode(None),
	curve(Single),
	graph_ptr(nullptr)
{
}

Vertex::Vertex( float pos_x, float pos_y,
	float tension, Curve curve, Graph *graph_ptr) :
	x(pos_x), y(pos_y),
	x_dirty(true), y_dirty(true),
	tension(tension),
	h_warp(0.0f), v_warp(0.0f),
	h_warp_graph(0.0f), v_warp_graph(0.0f),
	h_warp_mode(None), v_warp_mode(None),
	curve(curve),
	graph_ptr(graph_ptr)
{
}

static float powerScale( float input, float tension, float max_expo,
	float p1x, float p1y, float p2x, float p2y, bool inverse)
{
	DISTRHO_SAFE_ASSERT_RETURN(max_expo >= 1, input);

	const int in_sign = input >= 0 ? 1 : -1;
	const bool tension_is_pos = tension >= 0;

	tension = std::abs(tension);

	const float d_x = p2x - p1x;
	const float d_y = p2y - p1y;

	input = std::abs(input);

	float exponent = 1 + tension * (max_expo - 1);

	if (inverse) {
		exponent = 1.0f / exponent;
	}

	float result;

	if (tension_is_pos) {
		result = d_y * std::pow((input - p1x) / d_x, exponent) + p1y;
	} else {
		result = -d_y * std::pow(1 - (input - p1x) / d_x, exponent) + p2y;
	}

	return in_sign * result;
}

static float skewPlus(float x, float warp_amt)
{
	return 1 - std::pow(1 - x, warp_amt * 2 + 1);
}

static float invSkewPlus(float x, float warp_amt)
{
	return 1 - std::pow(1 - x, 1.0f / (warp_amt * 2 + 1));
}

static float skewMinus(float x, float warp_amt)
{
	return std::pow(x, warp_amt * 2 + 1);
}

static float invSkewMinus(float x, float warp_amt)
{
	return std::pow(x, 1.0f / (warp_amt * 2 + 1));
}

static float bendPlus(float x, float warp_amt, bool inverse)
{
	if (x < 0.5f) {
		return powerScale(x, -warp_amt, 3, 0.0f, 0.0f, 0.5f, 0.5f, inverse);
	} else if (x > 0.5f) {
		return powerScale(x, warp_amt, 3, 0.5f, 0.5f, 1.0f, 1.0f, inverse);
	} else {
		return x;
	}
}

static float bendMinus(float x, float warpAmount, bool inverse)
{
	if (x < 0.5f) {
		return powerScale(x, warpAmount, 3, 0.0f, 0.0f, 0.5f, 0.5f, inverse);
	} else if (x > 0.5f) {
		return powerScale(x, -warpAmount, 3, 0.5f, 0.5f, 1.0f, 1.0f, inverse);
	} else {
		return x;
	}
}

float Vertex::warpCoordinate( const float coordinate, const float warpAmount,
	const WarpMode warp_mode) const
{
	switch (warp_mode) {
	case None:
		return coordinate;
	case BendPlus:
		return bendPlus(coordinate, warpAmount, false);
	case BendMinus:
		return bendMinus(coordinate, warpAmount, false);
	case Bend:
	{
		if (warpAmount < 0.5f) {
			return bendPlus(coordinate, (0.5f - warpAmount) * 2, false);
		} else if (warpAmount > 0.5f) {
			return bendMinus(coordinate, (warpAmount - 0.5f) * 2, false);
		} else {
			return coordinate;
		}
	}
	case SkewPlus:
		return skewPlus(coordinate, warpAmount);
	case SkewMinus:
		return skewMinus(coordinate, warpAmount);
	case Skew:
	{
		if (warpAmount < 0.5f) {
			return skewPlus(coordinate, (0.5f - warpAmount) * 2);
		} else if (warpAmount > 0.5f) {
			return skewMinus(coordinate, (warpAmount - 0.5f) * 2);
		} else {
			return coordinate;
		}
	}
	default:
		return coordinate;
	}
}

float Vertex::getX()
{
	if ( x_dirty
		|| h_warp_graph != graph_ptr->getHorWarpAmt() 
		|| h_warp_mode != graph_ptr->getHorWarpMode() ) {
		h_warp_graph = graph_ptr->getHorWarpAmt();
		h_warp_mode = graph_ptr->getHorWarpMode();
		h_warp = warpCoordinate(x, h_warp_graph, h_warp_mode);
		x_dirty = false;
	}

	return h_warp;
}

float Vertex::getY()
{
	if ( y_dirty
		|| v_warp_graph != graph_ptr->getVerWarpAmt() 
		|| v_warp_mode != graph_ptr->getVerWarpMode() ) {
		v_warp_graph = graph_ptr->getVerWarpAmt();
		v_warp_mode = graph_ptr->getVerWarpMode();
		v_warp = warpCoordinate(y, v_warp_graph, v_warp_mode);
		y_dirty = false;
	}

	return v_warp;
}

float Vertex::unwarpCoordinate( float coordinate, const float warp_amt,
	const WarpMode warp_mode) const
{
	//we revert the effects of warp to set the correct value
	switch (warp_mode) {
	case None:
		return coordinate;
	case BendPlus:
		return bendPlus(coordinate, warp_amt, true);
	case BendMinus:
		return bendMinus(coordinate, warp_amt, true);
	case Bend:
		if (warp_amt < 0.5f) {
			return bendPlus(coordinate, (0.5f - warp_amt) * 2, true);
		} else if (warp_amt > 0.5f) {
			return bendMinus(coordinate, (warp_amt - 0.5f) * 2, true);
		} else {
			return coordinate;
		}
	case SkewPlus:
		return invSkewPlus(coordinate, warp_amt);
	case SkewMinus:
		return invSkewMinus(coordinate, warp_amt);
	case Skew:
	{
		if (warp_amt < 0.5f) {
			return invSkewPlus(coordinate, (0.5f - warp_amt) * 2);
		} else if (warp_amt > 0.5f) {
			return invSkewMinus(coordinate, (warp_amt - 0.5f) * 2);
		} else {
			return coordinate;
		}
	}
	default:
		return coordinate;
	}
}

void Vertex::setX(float x)
{
	this->x = unwarpCoordinate( x,
		graph_ptr->getHorWarpAmt(), graph_ptr->getHorWarpMode());
	x_dirty = true;
}

void Vertex::setY(float y)
{
	this->y = unwarpCoordinate(y,
		graph_ptr->getVerWarpAmt(), graph_ptr->getVerWarpMode());
	y_dirty = true;
}

Graph::Graph() : num_vertices(0),
	hor_warp_amt(0.0f),
	ver_warp_amt(0.0f),
	hor_warp_mode(None),
	ver_warp_mode(None)
{
	insertVertex(0.0f, 0.0f);
	insertVertex(1.0f, 1.0f);
}

float Graph::getOutValue( float input, float tension,
	float p1x, float p1y, float p2x, float p2y, Curve curve )
{
	const float in_sign = input >= 0 ? 1 : -1;

	if (p1x == p2x) {
		return in_sign * p2y;
	}

	//should probably be stored as a normalized value instead
	tension /= 100.0f;

	const bool tension_is_pve = tension >= 0.0f;

	//make the curve bend more slowly when the tension is near 0
	if (tension_is_pve) {
		tension = std::pow(tension, 1.2f);
	} else {
		tension = -std::pow(-tension, 1.2f);
	}

	const float d_x = p2x - p1x;
	const float d_y = p2y - p1y;

	switch (curve) {
	case Single:
	{
		return powerScale(input, tension, 15.0f, p1x, p1y, p2x, p2y, false);
	}
	case Double:
	{
		const float mid_x = p1x + d_x / 2.0f;
		const float mid_y = p1y + d_y / 2.0f;

		if (std::abs(input) > mid_x) {
			return powerScale( input, -tension, 15.0f,
				mid_x, mid_y, p2x, p2y, false );
		} else {
			return powerScale( input, tension, 15.0f,
				p1x, p1y, mid_x, mid_y, false );
		}
		break;
	}
	case Stairs:
	{
		if (tension == 0.0f) { //linear
			return powerScale( input, tension, 15.0f,
				p1x, p1y, p2x, p2y, false);
		}

		input = std::abs(input);

		int num_steps = std::floor(2.0f / std::pow(tension, 2.0f));

		const float step_x = d_x
			/ (tension_is_pve ? num_steps : num_steps - 1);
		const float step_y = d_y
			/ (tension_is_pve ? num_steps - 1 : num_steps);

		float result;

		if (tension_is_pve) {
			result = std::floor((input - p1x) / step_x) * step_y + p1y;
		} else {
			result = std::floor((input - p1x) / step_x + 1) * step_y + p1y;
		}

		//clamped to avoid some overshoot, might not be necessary
		const float min_y = std::min(p1y, p2y);
		const float max_y = std::max(p1y, p2y);

		return in_sign * wolf::clamp(result, min_y, max_y);
	}
	case Wave:
	{
		tension = std::floor(tension * 100.f);
		input = std::abs(input);

		const float frequency = (0.5f + tension) / d_x;
		const float phase = p1x * frequency * 2.0f * M_PI;

		float wave = -std::cos(frequency * M_PI * 2.0f * input - phase)
			/ 2.0f + 0.5f;

		if (!tension_is_pve) {
			wave = M_2_PI * std::asin(wave);
		}

		return in_sign * (wave * d_y + p1y);
	}
	default:
		return input; //¯\_(ツ)_/¯
	}
}

float Graph::getValueAt(float x)
{
	const float abs_x = std::abs(x);

	DISTRHO_SAFE_ASSERT_RETURN(abs_x <= 1.0f, x);

	//binary search
	int left = 0;
	int right = num_vertices - 1;
	int mid = 0;

	while (left <= right)
	{
		mid = left + (right - left) / 2;

		if (vertices[mid].getX() < abs_x) {
			left = mid + 1;
		} else if (vertices[mid].getX() > abs_x) {
			right = mid - 1;
		} else {
			return x >= 0 ? vertices[mid].getY() : -vertices[mid].getY();
		}
	}

	const float p1x = vertices[left - 1].getX();
	const float p1y = vertices[left - 1].getY();

	const float p2x = vertices[left].getX();
	const float p2y = vertices[left].getY();

	return getOutValue(x, vertices[left - 1].getTension(),
		p1x, p1y, p2x, p2y, vertices[left - 1].getCurve());
}

void Graph::insertVertex(float x, float y, float tension, Curve curve)
{
	if (num_vertices == max_vertices) return;

	int i = num_vertices;

	while ( (i > 0) && (x < vertices[i - 1].getX()) ) {
		vertices[i] = vertices[i - 1];
		--i;
	}

	Vertex vertex = Vertex(x, y, tension, curve, this);
	vertex.setPosition(x, y);

	vertices[i] = vertex;

	++num_vertices;
}

void Graph::removeVertex(int index)
{
	--num_vertices;

	for (int i = index; i < num_vertices; ++i) {
		vertices[i] = vertices[i + 1];
	}
}

void Graph::setTensionAtIndex(int index, float tension)
{
	vertices[index].setTension(tension);
}

Vertex *Graph::getVertexAtIndex(int index)
{
	DISTRHO_SAFE_ASSERT(index < num_vertices);

	return &vertices[index];
}

const char *Graph::serialize()
{
	Vertex vertex;

	int length = 0;

	for (int i = 0; i < num_vertices; ++i)
	{
		vertex = vertices[i];

		length += wolf::toHexFloat(serial_buf + length, vertex.x);
		length += std::sprintf(serial_buf + length, ",");
		length += wolf::toHexFloat(serial_buf + length, vertex.y);
		length += std::sprintf(serial_buf + length, ",");
		length += wolf::toHexFloat(serial_buf + length, vertex.tension);
		length += std::sprintf(serial_buf + length, ",%d;", vertex.curve);
	}

	return serial_buf;
}

void Graph::clear()
{
    num_vertices = 0;
}

void Graph::deserialize(const char *serialized)
{
	char *rest = (char *)serialized;

	int i = 0;

	do {
		const float x = wolf::parseHexFloat(rest, &rest);
		const float y = wolf::parseHexFloat(++rest, &rest);
		const float tension = wolf::parseHexFloat(++rest, &rest);
		const Curve type = static_cast<Curve>(std::strtol(++rest, &rest, 10));

		Vertex vertex = Vertex(x, y, tension, type, this);

		vertices[i++] = vertex;

	} while (strcmp(++rest, "\0") != 0);

	num_vertices = i;
}

} // namespace graphdyn

END_NAMESPACE_DISTRHO
