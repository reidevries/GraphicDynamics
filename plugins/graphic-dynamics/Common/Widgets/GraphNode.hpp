#ifndef GRAPHDYN_GRAPH_NODES_HPP
#define GRAPHDYN_GRAPH_NODES_HP

#include <chrono>
#include "src/DistrhoDefines.h"

START_NAMESPACE_DISTRHO

class GraphWidgetInner;

enum GraphVertexType
{
	Left,
	Middle,
	Right
};

class GraphNode : public IdleCallback
{
public:
	friend class GraphWidgetInner;

	GraphNode(GraphWidgetInner *parent);
	~GraphNode(); 

	/**
	* Draw the node.
	*/
	virtual void render();

	/**
	* Check if this node contains the point @a pos.
	*/
	virtual bool contains(Point<int> pos) = 0;

	virtual float getX() const = 0;
	virtual float getY() const = 0;

	int getAbsoluteX() const;
	int getAbsoluteY() const;

	Window &getParentWindow();

protected:
	void idleCallback() override;

	virtual bool onMotion(const Widget::MotionEvent &ev);
	virtual bool onMouse(const Widget::MouseEvent &ev);

	Point<int> getCorrectedPos(Point<int> surface);
	graphdyn::Graph *getLineEditor() const;

	GraphWidgetInner *parent;
	Color color;

	bool grabbed;
};

class GraphTensionHandle : public GraphNode
{
public:
	friend class GraphWidgetInner;

	GraphTensionHandle(GraphWidgetInner *parent, GraphVertex *vertex);

	void reset();
	void render() override;
	bool contains(Point<int> pos) override;

	float getX() const override;
	float getY() const override;

protected:
	bool onMotion(const Widget::MotionEvent &ev) override;
	bool onMouse(const Widget::MouseEvent &ev) override;

private:
	GraphVertex *vertex;
	Point<int> mouseDownPosition;

	DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphTensionHandle)
};

class GraphVertex : public GraphNode
{
	friend class GraphWidgetInner;

public:
	GraphVertex(GraphWidgetInner *parent, GraphVertexType type);

	void render() override;

	/**
	* Return true if the vertex cannot be moved on the X axis.
	*/
	bool isLockedX() const;
	void removeFromGraph();

	bool contains(Point<int> pos) override;
	void setPos(int x, int y);
	void setPos(Point<int> pos);

	float getX() const override;
	float getY() const override;
	float getSize() const;

	float getTension();
	int getIndex();
	
	void reset();

	GraphVertex *getVertexAtLeft() const;
	GraphVertex *getVertexAtRight() const;
	GraphTensionHandle *getTensionHandle();

	GraphVertexType getType();

protected:
	bool leftDoubleClick(const Widget::MouseEvent &ev);

	GraphTensionHandle tensionHandle;

	bool onMotion(const Widget::MotionEvent &ev) override;
	bool onMouse(const Widget::MouseEvent &ev) override;

	void clipCursorToNeighbouringVertices();

	void updateGraph();

	Point<int> clampVertexPosition(const Point<int> point) const;

private:
	int index;
	Circle<int> surface;

	GraphVertexType type;

	std::chrono::steady_clock::time_point lastClickTimePoint;
	int lastClickButton;

	DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphVertex)
};

END_NAMESPACE_DISTRHO

#endif
