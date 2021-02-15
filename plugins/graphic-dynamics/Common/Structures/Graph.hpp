#ifndef GRAPHDYN_GRAPH_DEFINED_H
#define GRAPHDYN_GRAPH_DEFINED_H

#include "src/DistrhoDefines.h"

START_NAMESPACE_DISTRHO

namespace graphdyn
{
/**
* The max number of vertices that can be in the graph at the same time.
*/
const int max_vertices = 99;

enum Curve
{
	Single = 0,
	Double,
	Stairs,
	Wave
};

enum WarpMode
{
	None = 0,
	BendPlus,
	BendMinus,
	Bend,
	SkewPlus,
	SkewMinus,
	Skew
};

class Graph;

class Vertex
{
public:
	friend class Graph;

	float getX();
	float getY();
	float getTension() const { return tension; }
	Curve getCurve() const { return curve; }

	void setX(float x);
	void setY(float y);
	void setPosition(float x, float y) { setX(x); setY(y); }
	void setTension(float tension) { this->tension = tension; }
	void setCurve(Curve curve) { this->curve = curve; }
	void setGraphPtr(Graph *graph_ptr) { this->graph_ptr = graph_ptr; }

protected:
	Vertex();
	Vertex(float posX, float posY, float tension, Curve curve,
		Graph *graph_ptr);

	float warpCoordinate( const float coordinate, const float warp_amt,
		const WarpMode warp_mode ) const;
	float unwarpCoordinate( float coordinate, const float warp_amt,
		const WarpMode warp_mode ) const;

private:
	float x;
	float y;
	bool x_dirty;
	bool y_dirty;
	float tension;
	float h_warp;
	float v_warp;
	float h_warp_graph;
	float v_warp_graph;
	WarpMode h_warp_mode;
	WarpMode v_warp_mode;
	Curve curve;

	Graph *graph_ptr;
};

class Graph
{
public:
	Graph();

	void insertVertex(float x, float y,
		float tension = 0.0f, Curve curve = Curve::Single);
	void removeVertex(int index);
	Vertex *getVertexAtIndex(int index);

	void setTensionAtIndex(int index, float tension);

	/**
	* Return the number of vertices contained in the graph.
	*/
	int getNumVertices() { return num_vertices; }

	static float getOutValue(float input, float tension,
		float p1x, float p1y, float p2x, float p2y, Curve curve);

	/**
	* Get the y value at x in the graph. 
	*/
	float getValueAt(float x);

	/**
	* Empty the graph.
	*/
	void clear();

	/**
	* Save the graph into a string.
	*/
	const char *serialize();

	/**
	* warp getter/setters
	*/
	//-------------------------------------------
	void setHorWarpAmt(float amt) { this->hor_warp_amt = amt; }
	float getHorWarpAmt() const { return this->hor_warp_amt; }

	void setVerWarpAmt(float amt) { this->ver_warp_amt = amt; }
	float getVerWarpAmt() const { return this->ver_warp_amt; }

	void setHorWarpMode(WarpMode mode) { this->hor_warp_mode = mode; }
	WarpMode getHorWarpMode() const { return this->hor_warp_mode; }

	void setVerWarpMode(WarpMode mode) { this->ver_warp_mode = mode; }
	WarpMode getVerWarpMode() const { return this->ver_warp_mode; }
	//-------------------------------------------

	/**
	* Rebuild the graph from a string.
	*/
	void deserialize(const char *serialized);

private:
	Vertex vertices[max_vertices];
	int num_vertices;

	float hor_warp_amt;
	float ver_warp_amt;

	WarpMode hor_warp_mode;
	WarpMode ver_warp_mode;

	//format: x,y,tension,type;
	char serial_buf[(sizeof(char) * 256 + 4) * max_vertices + 1];
};

} // namespace graphdyn

END_NAMESPACE_DISTRHO

#endif
