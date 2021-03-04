#include "Geometry.hpp"
#include "Graph.hpp"
#include "NanoVG.hpp"
#include "Window.hpp"
#include "GraphWidget.hpp"
#include "GraphNode.hpp"
#include "Mathf.hpp"
#include "UIConfig.hpp"

START_NAMESPACE_DISTRHO

GraphNode::GraphNode(GraphWidgetInner *parent)
	: parent(parent),
	  grabbed(false)
{
}

bool GraphNode::onMotion(const Widget::MotionEvent &) { return false; }
bool GraphNode::onMouse(const Widget::MouseEvent &) { return false; }
void GraphNode::render() {}
void GraphNode::idleCallback() {}

auto GraphNode::getAbsoluteX() const -> int
{
    return getX() + parent->getAbsoluteX();
}

auto GraphNode::getAbsoluteY() const -> int
{
    return parent->getHeight() - getY() + parent->getAbsoluteY();
}

auto GraphNode::getParentWindow() -> Window&
{
    return parent->getParentWindow();
}

auto GraphNode::getLineEditor() const -> graphdyn::Graph*
{
    return &parent->lineEditor;
}


GraphTensionHandle::GraphTensionHandle( GraphWidgetInner *parent,
	GraphVertex *vertex )
	: GraphNode(parent),
	  vertex(vertex)
{
}

auto GraphTensionHandle::getX() const -> float
{
    GraphVertex *leftVertex = vertex;
    GraphVertex *rightVertex = leftVertex->getVertexAtRight();

    return (leftVertex->getX() + rightVertex->getX()) / 2.0f;
}

void GraphTensionHandle::reset()
{
    graphdyn::Graph *lineEditor = getLineEditor();
    lineEditor->getVertexAtIndex(vertex->getIndex())->setTension(0);

    parent->ui->setState("graph", lineEditor->serialize());
}

auto GraphTensionHandle::onMotion(const Widget::MotionEvent &ev) -> bool
{
    if (!grabbed)
    {
        parent->getParentWindow().setCursorStyle(Window::CursorStyle::Grab);
        return true;
    }

    const float resistance = 4.0f;

    Point<int> pos = wolf::flipY(ev.pos, parent->getHeight());

    const GraphVertex *leftVertex = vertex;
    const GraphVertex *rightVertex = vertex->getVertexAtRight();

    float tension = vertex->getTension();
    float difference = mouseDownPosition.getY() - pos.getY();

    if (leftVertex->getY() > rightVertex->getY())
        difference = -difference;

    Window &window = getParentWindow();

    //FIXME: this is a bit confusing... mouseDownPosition is flipped, but setCursorPos expects the real y value
    if (ev.pos.getY() <= 2)
    {
        window.setCursorPos(getAbsoluteX(), parent->getAbsoluteY() + parent->getHeight() - 2);
        mouseDownPosition.setY(2);
    }
    else if (ev.pos.getY() >= (int)parent->getHeight() + 2)
    {
        window.setCursorPos(getAbsoluteX(), parent->getAbsoluteY() + 2);
        mouseDownPosition.setY(parent->getHeight() - 2);
    }
    else
    {
        mouseDownPosition = pos;
    }

    tension = wolf::clamp(tension + difference / resistance, -100.0f, 100.0f);

    graphdyn::Graph *lineEditor = getLineEditor();
    lineEditor->getVertexAtIndex(vertex->getIndex())->setTension(tension);

    parent->ui->setState("graph", lineEditor->serialize());

    parent->repaint();

    return true;
}

auto GraphTensionHandle::contains(Point<int> pos) -> bool
{
	//last vertex doesn't have a tension handle
    if (vertex->getType() == GraphVertexType::Right) return false;

    Circle<int> surface(getX(), getY(), 8.0f);

    return wolf::pointInCircle(surface, pos);
}

auto GraphTensionHandle::getY() const -> float
{
	GraphVertex *leftVertex = vertex;
	GraphVertex *rightVertex = leftVertex->getVertexAtRight();

	float tension = vertex->getTension();

	//calculate value for generic curve
	float p1x = 0.0f;
	float p1y = leftVertex->getY() / parent->getHeight();
	float p2x = 1.0f;
	float p2y = rightVertex->getY() / parent->getHeight();

	const int index = leftVertex->getIndex();

	graphdyn::Curve curve
		= getLineEditor()->getVertexAtIndex(index)->getCurve();

	return graphdyn::Graph::getOutValue( 0.5f, tension,
		p1x, p1y, p2x, p2y, curve ) * parent->getHeight();
}

auto GraphTensionHandle::onMouse(const Widget::MouseEvent &ev) -> bool
{
    grabbed = ev.press;
    Window &window = getParentWindow();

    if (grabbed)
    {
        mouseDownPosition = wolf::flipY(ev.pos, parent->getHeight());

        window.hideCursor();
        window.clipCursor(Rectangle<int>(getAbsoluteX(), 0, 0, (int)window.getHeight()));
    } else {
        window.unclipCursor();
        window.setCursorPos(getAbsoluteX(), getAbsoluteY());
        window.showCursor();

        window.setCursorStyle(Window::CursorStyle::Grab);
    }

    parent->repaint();

    return true;
}

void GraphTensionHandle::render()
{
    if (vertex->getType() == GraphVertexType::Right) //last vertex doesn't have a tension handle
        return;

    parent->beginPath();

    parent->strokeWidth(UIConfig::tension_handle_stroke_w);

    if (parent->edgeMustBeEmphasized(vertex->getIndex())) //TODO: make that a method on the vertex
        parent->strokeColor(UIConfig::tension_handle_focus);
    else
        parent->strokeColor(UIConfig::tension_handle_normal);

    parent->circle(getX(), getY(), UIConfig::tension_handle_radius);

    parent->stroke();

    parent->closePath();
}



GraphVertex::GraphVertex( GraphWidgetInner *parent, GraphVertexType type )
	: GraphNode(parent),
	  tensionHandle(parent, this),
	  surface(Circle<int>(0, 0, 8.0f)),
	  type(type),
	  lastClickButton(0)
{
    switch (type)
    {
    case GraphVertexType::Left:
    case GraphVertexType::Middle:
        surface = Circle<int>(0, 0, UIConfig::tension_handle_radius);
        break;
    case GraphVertexType::Right:
        surface = Circle<int>(
			parent->getWidth(),
			parent->getHeight(),
			UIConfig::tension_handle_radius
		);
        break;
    }
}

void GraphVertex::reset()
{
    surface = Circle<int>(0, 0, UIConfig::tension_handle_radius);
    type = GraphVertexType::Middle;
    grabbed = false;
}

auto GraphVertex::isLockedX() const -> bool
{
    return type != GraphVertexType::Middle;
}

void GraphVertex::render()
{
    const bool focused = parent->focusedElement == this;

    parent->beginPath();

    parent->strokeWidth(UIConfig::tension_handle_stroke_w);


    if (focused)
    {
        parent->strokeColor(UIConfig::vertex_stroke_focus);
        parent->fillColor(UIConfig::vertex_fill_focus);
    }
    else
    {
        parent->strokeColor(UIConfig::vertex_stroke_normal);
        parent->fillColor(UIConfig::vertex_fill_normal);
    }

    parent->circle(getX(), getY(), getSize());

    parent->fill();
    parent->stroke();

    parent->closePath();
}

GraphVertexType GraphVertex::getType()
{
    return type;
}

auto GraphVertex::contains(Point<int> pos) -> bool
{
    Circle<int> surface(getX(), getY(), 8.0f);

    return wolf::pointInCircle(surface, pos);
}

void GraphVertex::setPos(int x, int y)
{
    surface.setPos(x, y);
}

void GraphVertex::setPos(Point<int> pos)
{
    surface.setPos(pos);
}

auto GraphVertex::getX() const -> float
{
    return surface.getX();
}

auto GraphVertex::getY() const -> float
{
    return surface.getY();
}

auto GraphVertex::getSize() const -> float
{
    return surface.getSize();
}

auto GraphVertex::getTension() -> float
{
    return getLineEditor()->getVertexAtIndex(index)->getTension();
}

auto GraphVertex::getVertexAtLeft() const -> GraphVertex*
{
    if (index == 0)
        return nullptr;

    return parent->graphVertices[index - 1];
}

auto GraphVertex::getVertexAtRight() const -> GraphVertex*
{
    if (index == parent->lineEditor.getNumVertices() - 1)
        return nullptr;

    return parent->graphVertices[index + 1];
}

auto GraphVertex::getTensionHandle() -> GraphTensionHandle*
{
    return &tensionHandle;
}

auto GraphVertex::clampVertexPosition(const Point<int> point) const -> Point<int>
{
    const GraphVertex *leftVertex = getVertexAtLeft();
    const GraphVertex *rightVertex = getVertexAtRight();

    int x = this->getX();
    int y = point.getY();

    if (!isLockedX())
    {
        //clamp to neighbouring vertices
        x = wolf::clamp<int>(point.getX(), leftVertex->getX() + 1, rightVertex->getX() - 1);
    }

    //clamp to graph
    y = wolf::clamp<int>(y, 0, parent->getHeight());

    return Point<int>(x, y);
}

void GraphVertex::updateGraph()
{
    const float width = parent->getWidth();
    const float height = parent->getHeight();

    const float normalizedX = wolf::normalize(surface.getX(), width);
    const float normalizedY = wolf::normalize(surface.getY(), height);

    graphdyn::Graph *lineEditor = &parent->lineEditor;

    lineEditor->getVertexAtIndex(index)->setPosition(normalizedX, normalizedY);

    parent->ui->setState("graph", lineEditor->serialize());
}

auto GraphVertex::onMotion(const Widget::MotionEvent &ev) -> bool
{
    if (!grabbed)
    {
        getParentWindow().setCursorStyle(Window::CursorStyle::Grab);
        return true;
    }

    Point<int> pos = wolf::flipY(ev.pos, parent->getHeight());

    Point<int> clampedPosition = clampVertexPosition(pos);
    surface.setPos(clampedPosition);

    updateGraph();

    parent->repaint();

    //Cancel out double clicks
    lastClickButton = 0;

    return true;
}

auto GraphVertex::getIndex() -> int
{
    return index;
}


void GraphVertex::removeFromGraph()
{
    parent->removeVertex(index);
}

auto GraphVertex::leftDoubleClick(const Widget::MouseEvent &) -> bool
{
    removeFromGraph();
    getParentWindow().setCursorStyle(Window::CursorStyle::Default);

    return true;
}

void GraphVertex::clipCursorToNeighbouringVertices()
{
    GraphVertex *leftVertex = getVertexAtLeft();
    GraphVertex *rightVertex = getVertexAtRight();

    //properties of the clip rectangle
    const int left = leftVertex ? leftVertex->getAbsoluteX() : this->getAbsoluteX();
    const int top = parent->getAbsoluteY();
    const int right = rightVertex ? rightVertex->getAbsoluteX() : this->getAbsoluteX();

    getParentWindow().clipCursor(Rectangle<int>(left, top, right - left, parent->getHeight()));
}

auto GraphVertex::onMouse(const Widget::MouseEvent &ev) -> bool
{
    using namespace std::chrono;

    steady_clock::time_point now = steady_clock::now();

    bool doubleClick = ev.press && lastClickButton == ev.button && duration_cast<duration<double>>(now - lastClickTimePoint).count() < 0.250;

    if (ev.press)
    {
        lastClickTimePoint = now;
        lastClickButton = ev.button;
    }

    if (doubleClick)
    {
        lastClickButton = -1;

        if (this->type == GraphVertexType::Middle) //vertices on the sides don't receive double click, cause they can't get removed
            return leftDoubleClick(ev);
    }

    grabbed = ev.press;
    Window &window = getParentWindow();

    if (grabbed)
    {
        window.hideCursor();

        clipCursorToNeighbouringVertices();
    }
    else
    {
        window.setCursorPos(getAbsoluteX(), getAbsoluteY());
        window.unclipCursor();

        window.showCursor();
        window.setCursorStyle(Window::CursorStyle::Grab);
    }

    parent->repaint();

    return true;
}

END_NAMESPACE_DISTRHO
