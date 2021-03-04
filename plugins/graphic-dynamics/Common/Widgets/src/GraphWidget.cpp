#include "Widget.hpp"
#include "Window.hpp"
#include "Graph.hpp"
#include "ObjectPool.hpp"
#include "GraphWidget.hpp"
#include "GraphNode.hpp"
#include "Mathf.hpp"

#include "Fonts/chivo_italic.hpp"

#include <chrono>
#include <cstdlib>
#include <cmath>
#include <iostream>

START_NAMESPACE_DISTRHO

const char *graphDefaultState
	= "0x0p+0,0x0p+0,0x0p+0,0;0x1p+0,0x1p+0,0x0p+0,0;";

const std::array<MenuWidget::Item, 7> GraphWidgetInner::menu_layout
	= std::array<MenuWidget::Item, 7>{
		MenuWidget::Item( "Vertex Options" ),
		MenuWidget::Item( VertexMenuItem::Delete, "Delete", "(double l-click)"),
		MenuWidget::Item( "Curve Mode" ),
		MenuWidget::Item( VertexMenuItem::Single, "Single Power", ""),
		MenuWidget::Item( VertexMenuItem::Double, "Double Power", ""),
		MenuWidget::Item( VertexMenuItem::Stairs, "Stairs", ""),
		MenuWidget::Item( VertexMenuItem::Wave, "Wave", "")
};

GraphWidgetInner::GraphWidgetInner( UI *ui, Size<uint> size )
	: NanoWidget((NanoWidget *)ui),
	  ui(ui),
	  graphVerticesPool(graphdyn::max_vertices, this, GraphVertexType::Middle),
	  focusedElement(nullptr),
	  mouseLeftDown(false),
	  mouseRightDown(false),
	  graphGradientMode(GraphGradientMode::None),
	  mustHideVertices(false),
	  hovered(false),
	  maxInput(0.0f),
	  fInput(0.0f),
	  fLastCurveTypeSelected(graphdyn::Curve::Single)
{
	setSize(size);

	initializeDefaultVertices();

	getParentWindow().addIdleCallback(this);

	click_r_menu = std::make_unique<MenuWidget>(ui);
	click_r_menu->addItems(menu_layout);
	click_r_menu->setCallback(this);
	click_r_menu->hide();

	using namespace WOLF_FONTS;
	createFontFromMemory("chivo_italic", (const uchar *)chivo_italic, chivo_italic_size, 0);
}

GraphWidgetInner::~GraphWidgetInner()
{
	for (int i = 0; i < lineEditor.getNumVertices(); ++i) {
		delete graphVertices[i];
	}
}

void GraphWidgetInner::onResize(const ResizeEvent &ev)
{
	if (ev.oldSize.isNull())
		return;

	positionGraphNodes();
}

void GraphWidgetInner::positionGraphNodes()
{
	for (int i = 0; i < lineEditor.getNumVertices(); ++i)
	{
		GraphVertex *vertexWidget = graphVertices[i];
		graphdyn::Vertex *logicalVertex = lineEditor.getVertexAtIndex(i);

		vertexWidget->setPos(
			logicalVertex->getX() * getWidth(),
			logicalVertex->getY() * getHeight() );
	}

	repaint();
}

void GraphWidgetInner::initializeDefaultVertices()
{
	//Left vertex
	GraphVertex *vertex = graphVerticesPool.getObject();

	vertex->setPos(0, 0);
	vertex->index = 0;
	vertex->type = GraphVertexType::Left;

	graphVertices[0] = vertex;

	//Right vertex
	vertex = graphVerticesPool.getObject();

	vertex->setPos(getWidth(), getHeight());
	vertex->index = 1;
	vertex->type = GraphVertexType::Right;

	graphVertices[1] = vertex;
}

void GraphWidgetInner::reset()
{
	resetVerticesPool();

	initializeDefaultVertices();

	ui->setState("graph", graphDefaultState);
	lineEditor.deserialize(graphDefaultState);
}

void GraphWidgetInner::resetVerticesPool()
{
    for (int i = 0; i < lineEditor.getNumVertices(); ++i)
    {
        graphVerticesPool.freeObject(graphVertices[i]);
    }
}

void GraphWidgetInner::rebuildFromString(const char *serializedGraph)
{
    resetVerticesPool();

    lineEditor.deserialize(serializedGraph);

    //position ui vertices to match the new graph
    for (int i = 0; i < lineEditor.getNumVertices(); ++i)
    {
        GraphVertex *vertex = graphVerticesPool.getObject();

        vertex->index = i;

        if (i == 0)
            vertex->type = GraphVertexType::Left;
        else if (i == lineEditor.getNumVertices() - 1)
            vertex->type = GraphVertexType::Right;
        else
            vertex->type = GraphVertexType::Middle;

        graphVertices[i] = vertex;
    }

    positionGraphNodes();
}

void GraphWidgetInner::updateAnimations()
{
}

void GraphWidgetInner::flipYAxis()
{
    transform(1.0f, 0.0f, 0.0f, -1.0f, 0.0f, getHeight());
}

void GraphWidgetInner::drawSubGrid()
{
}

void GraphWidgetInner::drawGrid()
{
    const float width = getWidth();
    const float height = getHeight();

    const float lineWidth = 1.0f;

    const int squaresPerRow = 8.0f;
    const int gridMiddleLineIndex = squaresPerRow / 2.0f;

    const float verticalStep = width / squaresPerRow;
    const float horizontalStep = height / squaresPerRow;

    //vertical
    for (int i = 0; i < squaresPerRow + 1; ++i)
    {
        const float posX = std::round(i * verticalStep);

        //subgrid
        beginPath();
        strokeWidth(lineWidth);
        strokeColor(UIConfig::grid_stroke_sub);

        moveTo(std::round(posX + verticalStep / 2.0f), 0.0f);
        lineTo(std::round(posX + verticalStep / 2.0f), height);

        stroke();
        closePath();

        //background
        beginPath();
        strokeWidth(lineWidth);
        strokeColor(UIConfig::grid_stroke_bg);

        moveTo(posX + lineWidth, 0.0f);
        lineTo(posX + lineWidth, height);

        stroke();
        closePath();

        //foreground
        beginPath();
        strokeWidth(lineWidth);
		auto stroke_fg = UIConfig::grid_stroke_fg;
		if (i == gridMiddleLineIndex) stroke_fg = UIConfig::grid_stroke_middle;
        strokeColor(stroke_fg);

        moveTo(posX, 0.0f);
        lineTo(posX, height);

        stroke();
        closePath();
    }

    //horizontal
    for (int i = 0; i < squaresPerRow + 1; ++i)
    {
        const float posY = std::round(i * horizontalStep);

        //subgrid
        beginPath();
        strokeWidth(lineWidth);
        strokeColor(UIConfig::grid_stroke_sub);

        moveTo(0.0f, std::round(posY + horizontalStep / 2.0f));
        lineTo(width, std::round(posY + horizontalStep / 2.0f));

        stroke();
        closePath();

        //background
        beginPath();
        strokeWidth(lineWidth);

        moveTo(0.0f, posY + lineWidth);
        lineTo(width, posY + lineWidth);

        strokeColor(UIConfig::grid_stroke_bg);

        stroke();
        closePath();

        //foreground
        beginPath();
        strokeWidth(lineWidth);
		auto stroke_fg = UIConfig::grid_stroke_fg;
		if (i == gridMiddleLineIndex) stroke_fg = UIConfig::grid_stroke_middle;
        strokeColor(stroke_fg);

        moveTo(0.0f, posY);
        lineTo(width, posY);

        stroke();
        closePath();
    }
}

void GraphWidgetInner::drawBackground()
{
    const float width = getWidth();
    const float height = getHeight();

    //const float centerX = width / 2.0f;
    //const float centerY = height / 2.0f;

    beginPath();

    rect(0.f, 0.f, width, height);
    //Paint gradient = radialGradient(centerX, centerY, 1.0f, centerX, Color(42, 42, 42, 255), Color(33, 32, 39, 255));
    //fillPaint(gradient);
    fillColor(UIConfig::grid_fill_bg);
    fill();

    closePath();
}

bool GraphWidgetInner::edgeMustBeEmphasized(int vertexIndex)
{
    if (focusedElement == nullptr)
        return false;

    GraphVertex *vertex = graphVertices[vertexIndex];

    if (dynamic_cast<GraphTensionHandle *>(focusedElement))
        return focusedElement == vertex->getTensionHandle();

    return false; //might not be a good idea to make it emphasized otherwise

    /*
    if (type == GraphVertexType::Right)
        return false; //there is no edge at the right of the last vertex

    return focusedElement == vertex || focusedElement == vertex->getVertexAtRight();*/
}

void GraphWidgetInner::drawGraphEdge(int vertexIndex, float lineWidth, Color color)
{
    DISTRHO_SAFE_ASSERT(vertexIndex < lineEditor.getNumVertices() - 1);

    const float width = getWidth();
    const float height = getHeight();

    graphdyn::Vertex *leftVertex = lineEditor.getVertexAtIndex(vertexIndex);
    graphdyn::Vertex *rightVertex = lineEditor.getVertexAtIndex(vertexIndex + 1);

    beginPath();

    strokeColor(color);
    strokeWidth(lineWidth);
    lineJoin(ROUND);

    moveTo(leftVertex->getX() * width, leftVertex->getY() * height);

    const float edgeLength = (rightVertex->getX() - leftVertex->getX()) * width;

    for (int i = 0; i <= edgeLength; ++i)
    {
        const float normalizedX = leftVertex->getX() + i / width;

        lineTo(normalizedX * width, lineEditor.getValueAt(normalizedX) * height);
    }

    lineTo(rightVertex->getX() * width, rightVertex->getY() * height);

    stroke();

    closePath();
}

void GraphWidgetInner::drawGraphLine(float lineWidth, Color normalColor, Color emphasizedColor)
{
    for (int i = 0; i < lineEditor.getNumVertices() - 1; ++i)
    {
        const Color color = edgeMustBeEmphasized(i) ? emphasizedColor : normalColor;

        drawGraphEdge(i, lineWidth, color);
    }
}

void GraphWidgetInner::drawAlignmentLines()
{
    const int x = focusedElement->getX();
    const int y = focusedElement->getY();
    const int w = getWidth();
    const int h = getHeight();

    translate(0.5f, 0.5f);

    beginPath();

    strokeWidth(1.0f);
    strokeColor(UIConfig::grid_stroke_alignment);

    moveTo(x, 0);
    lineTo(x, h);

    moveTo(0, y);
    lineTo(w, y);

    stroke();

    closePath();

    translate(-0.5f, -0.5f);
}

void GraphWidgetInner::drawGradient()
{
	if (graphGradientMode == GraphGradientMode::None)
		return;

	const float width = getWidth();
	const float height = getHeight();

	float peak = 0.0f;

	beginPath();

	moveTo(0, lineEditor.getVertexAtIndex(0)->getY() * height);

	for (int vertexIndex = 0; vertexIndex < lineEditor.getNumVertices() - 1; ++vertexIndex) {
		graphdyn::Vertex *leftVertex = lineEditor.getVertexAtIndex(vertexIndex);
		graphdyn::Vertex *rightVertex = lineEditor.getVertexAtIndex(vertexIndex + 1);

		const float edgeLength = (rightVertex->getX() - leftVertex->getX()) * width;

		for (int i = 0; i <= edgeLength; ++i)
		{
			const float normalizedX = leftVertex->getX() + i / width;
			const float x = normalizedX * width;
			const float y = lineEditor.getValueAt(normalizedX) * height;

			lineTo(x, y);

			peak = std::max(peak, y);
		}
	}

	lineTo(width, 0);
	lineTo(0, 0);

	fillPaint( linearGradient(
		width / 2.0f, 0,    // start x and y
		width / 2.0f, peak, // end x and y
		UIConfig::grid_gradient_i,
		UIConfig::grid_gradient_o
	) );
	fill();

	closePath();
}

void GraphWidgetInner::updateInput(const float input)
{
    fInput = input;
}

void GraphWidgetInner::setGraphGradientMode(GraphGradientMode graphGradientMode)
{
    this->graphGradientMode = graphGradientMode;
    repaint();
}

void GraphWidgetInner::setHorWarpAmt(const float amt)
{
    lineEditor.setHorWarpAmt(amt);
    positionGraphNodes();
}

void GraphWidgetInner::setHorWarpMode(const graphdyn::WarpMode mode)
{
    lineEditor.setHorWarpMode(mode);
    positionGraphNodes();
}

void GraphWidgetInner::setVerWarpAmt(const float amt)
{
    lineEditor.setVerWarpAmt(amt);
    positionGraphNodes();
}

void GraphWidgetInner::setVerWarpMode(const graphdyn::WarpMode mode)
{
    lineEditor.setVerWarpMode(mode);
    positionGraphNodes();
}

void GraphWidgetInner::setMustHideVertices(const bool hide)
{
    mustHideVertices = hide;
    repaint();
}

void GraphWidgetInner::drawInputIndicator()
{
    const float width = getWidth();
    const float height = getHeight();

    if (fInput <= 0.0f)
        return;

    const float inputIndicatorX = std::round(fInput * width);

    translate(0.5f, 0.5f);

    beginPath();

    strokeColor(UIConfig::volume_indicator_stroke);
    strokeWidth(1.0f);

    moveTo(inputIndicatorX, 0);
    lineTo(inputIndicatorX, height);

    stroke();

    closePath();

    translate(-0.5f, -0.5f);

    //circle at indicator intersection with the graph
    const float circleY = lineEditor.getValueAt(fInput) * height;

    beginPath();

    fillColor(UIConfig::playhead_fill);
    strokeColor(UIConfig::playhead_stroke);

    circle(inputIndicatorX, circleY, 3.5f);
    fill();
    stroke();
    closePath();
}

void GraphWidgetInner::idleCallback()
{
    repaint();
}

void GraphWidgetInner::drawInOutLabels()
{
	fontFace("chivo_italic");
	fontSize(36.f);
	fillColor(255, 255, 255, 125);

	textAlign(ALIGN_BOTTOM | ALIGN_RIGHT);
	text(getWidth() - 5, getHeight(), "In", NULL);

	textAlign(ALIGN_TOP | ALIGN_LEFT);
	text(5, 0, "Out", NULL);
}

void GraphWidgetInner::drawVertices()
{
	const int vertexCount = lineEditor.getNumVertices();

	for (int i = 0; i < vertexCount; ++i)
	{
		GraphVertex *vertex = graphVertices[i];

		vertex->getTensionHandle()->render();
		vertex->render();
	}
}

void GraphWidgetInner::onNanoDisplay()
{
}

bool GraphWidgetInner::onScroll(const ScrollEvent &ev)
{
	const Point<int> point = wolf::flipY(ev.pos, getHeight());

	//Testing for mouse hover on tension handles
	for (int i = 0; i < lineEditor.getNumVertices() - 1; ++i) {
		GraphTensionHandle *tensionHandle = graphVertices[i]->getTensionHandle();

		if (tensionHandle->contains(point)) {
			float delta = ev.delta.getY();
			if ( graphVertices[i]->getY() < graphVertices[i + 1]->getY() ) {
				delta = -delta;
			}

			const float oldTension = lineEditor.getVertexAtIndex(i)
				->getTension();

			lineEditor.setTensionAtIndex(i,
				wolf::clamp(oldTension + 1.5f * delta, -100.0f, 100.0f));

			ui->setState("graph", lineEditor.serialize());
			repaint();

			getParentWindow().setCursorPos(
				tensionHandle->getAbsoluteX(), tensionHandle->getAbsoluteY() );

			return true;
		}
	}

	return false;
}

void GraphWidgetInner::removeVertex(int index)
{
	//Make sure the vertex to remove is in the middle of the graph
	if (index <= 0)
		return;
	else if (index >= lineEditor.getNumVertices() - 1)
		return;

	//Get rid of the ui widget
	graphVerticesPool.freeObject(graphVertices[index]);

	const int vertexCount = lineEditor.getNumVertices() - 1;

	for (int i = index; i < vertexCount; ++i)
	{
		graphVertices[i] = graphVertices[i + 1];
		graphVertices[i]->index--;
	}

	//Get rid of the logical vertex and update dsp
	lineEditor.removeVertex(index);
	ui->setState("graph", lineEditor.serialize());

	focusedElement = nullptr;

	repaint();
}

GraphVertex *GraphWidgetInner::insertVertex(const Point<int> pos)
{
	int i = lineEditor.getNumVertices();

	if (i == graphdyn::max_vertices)
		return nullptr;

	while ((i > 0) && (pos.getX() < graphVertices[i - 1]->getX()))
	{
		graphVertices[i] = graphVertices[i - 1];
		graphVertices[i]->index++;

		--i;
	}

	GraphVertex *vertex = graphVerticesPool.getObject();

	vertex->index = i;

	graphVertices[i] = vertex;

	const float width = getWidth();
	const float height = getHeight();

	const float normalizedX = wolf::normalize(pos.getX(), width);
	const float normalizedY = wolf::normalize(pos.getY(), height);

	lineEditor.insertVertex(normalizedX, normalizedY, 0, fLastCurveTypeSelected);

	ui->setState("graph", lineEditor.serialize());

	positionGraphNodes();

	return vertex;
}

GraphNode *GraphWidgetInner::getHoveredNode(Point<int> cursorPos)
{
    //Testing for mouse hover on graph vertices
    for (int i = lineEditor.getNumVertices() - 1; i >= 0; --i)
    {
        if (graphVertices[i]->contains(cursorPos))
        {
            return graphVertices[i];
        }
    }

    //Testing for mouse hover on tension handles
    for (int i = lineEditor.getNumVertices() - 1; i >= 0; --i)
    {
        if (graphVertices[i]->tensionHandle.contains(cursorPos))
        {
            return &graphVertices[i]->tensionHandle;
        }
    }

    return nullptr;
}

bool GraphWidgetInner::leftClick(const MouseEvent &ev)
{
    const Point<int> point = wolf::flipY(ev.pos, getHeight());

    if (mouseRightDown)
        return true;

    mouseLeftDown = ev.press;

    if (!ev.press)
    {
        if (focusedElement != nullptr)
        {
            focusedElement->onMouse(ev);
            focusedElement = nullptr;
        }

        return true;
    }

    GraphNode *hoveredNode = getHoveredNode(point);

    if (hoveredNode != nullptr)
    {
        focusedElement = hoveredNode;

        return focusedElement->onMouse(ev);
    }

    //The cursor is not over any graph node
    return false;
}

bool GraphWidgetInner::middleClick(const MouseEvent &)
{
    return false;
}

bool GraphWidgetInner::rightClick(const MouseEvent &ev)
{
	const Point<int> point = wolf::flipY(ev.pos, getHeight());

	if (mouseLeftDown)
		return true;

	mouseRightDown = ev.press;

	if (focusedElement == nullptr)
	{
		GraphNode *node = getHoveredNode(point);

		if (node != nullptr)
		{
			//check if tension handle, if true, reset tension
			GraphTensionHandle *tensionHandle
				= dynamic_cast<GraphTensionHandle *>(node);

			if (tensionHandle != nullptr)
			{
				tensionHandle->reset();

				//if the handle is not on the cursor anymore, we reset the cursor style
				node = getHoveredNode(point);

				if (node == nullptr)
				{
					getParentWindow().setCursorStyle(Window::CursorStyle::Default);
				}

				repaint();
			}

			//else, show curve selection menu
			else
			{
				fNodeSelectedByRightClick = node;

				//disable certain items depending which vertex is selected
				click_r_menu->setAllItemsEnabled(true);
				const auto vertex_type
					= dynamic_cast<GraphVertex*>(node)->getType();
				if (vertex_type != GraphVertexType::Middle) {
					click_r_menu->setItemEnabled(section_index_delete, false);
					if (vertex_type == GraphVertexType::Right) {
						click_r_menu->setItemEnabled(section_index_curve,
							false);
					}
				}

				auto click_pos = Point<int>(
					getAbsoluteX() + ev.pos.getX(),
					getAbsoluteY() + ev.pos.getY()
				);
				auto widget_bounds = Rectangle<int>(
					getAbsoluteX(), getAbsoluteY(),
					getWidth(), getHeight()
				);
				click_r_menu->show(click_pos, widget_bounds);
			}

			return true;
		}

		if (ev.press && contains(ev.pos))
		{
			focusedElement = insertVertex(point);

			if (focusedElement != nullptr)
				return focusedElement->onMouse(ev);
		}
	}
	else
	{
		focusedElement->onMouse(ev);
		focusedElement = nullptr;

		return true;
	}

	return false;
}

bool GraphWidgetInner::onMouse(const MouseEvent &ev)
{
	/* debug stuff
	std::cout << "current mouse state:" << std::endl;
	if (mouseLeftDown)  std::cout << "\tleft  down" << std::endl;
	if (mouseRightDown) std::cout << "\tright down" << std::endl;
	std::cout << "\tfocused element:" << focusedElement << std::endl;
	*/

	if (mustHideVertices)
        return false;

    switch (ev.button)
    {
    case 1:
        return leftClick(ev);
    case 2:
        return middleClick(ev);
    case 3:
        return rightClick(ev);
    }

    return false;
}

void GraphWidgetInner::menuItemSelected(const int id)
{
	std::cout << "user selected menu item " << id << std::endl;
	GraphVertex *vertex = static_cast<GraphVertex*>(fNodeSelectedByRightClick);

	if (id == VertexMenuItem::Delete) {
		removeVertex(vertex->getIndex());
	} else {
		graphdyn::Curve curve = (graphdyn::Curve)(id - 1);

		lineEditor.getVertexAtIndex(vertex->getIndex())->setCurve(curve);
		fLastCurveTypeSelected = curve;

		ui->setState("graph", lineEditor.serialize());
		repaint();
	}
}

void GraphWidgetInner::propagateMouseEvent(const MouseEvent &ev)
{
	switch (ev.button) {
		case 1:
			mouseLeftDown = ev.press;
			break;
		case 3:
			mouseRightDown = ev.press;
			break;
	}
}

bool GraphWidgetInner::onMotion(const MotionEvent &ev)
{
    if (mustHideVertices)
        return false;

    const Point<int> point = wolf::flipY(ev.pos, getHeight());
    GraphNode *hoveredNode = getHoveredNode(point);

    if (contains(ev.pos) || hoveredNode != nullptr)
    {
        hovered = true;
    }
    else if (hovered && !contains(ev.pos) && focusedElement == nullptr)
    {
        onMouseLeave();
        hovered = false;

        return false;
    }
    else if (focusedElement == nullptr)
    {
        return false;
    }

    if (focusedElement != nullptr)
    {
        return focusedElement->onMotion(ev);
    }

    if (hoveredNode != nullptr)
    {
        return hoveredNode->onMotion(ev);
    }

    //The mouse pointer is not over any graph node
    getParentWindow().setCursorStyle(Window::CursorStyle::Default);

    return true;
}

void GraphWidgetInner::onMouseLeave()
{
    getParentWindow().setCursorStyle(Window::CursorStyle::Default);
}

GraphWidget::GraphWidget( UI *ui, Size<uint> size, const UIConfig& uiconf ) :
	NanoWidget((NanoWidget *)ui),
	margin(uiconf.graph_margin),
	m_fill(uiconf.graph_m_fill),
	m_stroke(uiconf.graph_m_stroke),
	m_stroke_w(uiconf.graph_m_stroke_w),
	top_stroke(uiconf.graph_top_stroke),
	top_w(uiconf.graph_top_w),
	edge_w(uiconf.graph_edge_w),
	edge_fg_normal(uiconf.graph_edge_fg_normal),
	edge_fg_focus(uiconf.graph_edge_fg_focus)
{
	setSize(size);
	inner = std::make_unique<GraphWidgetInner>(ui, calcInnerSize(), uiconf);
	inner->parent = this;
}

void GraphWidget::onNanoDisplay()
{
	const float w = getWidth();
	const float h = getHeight();

	beginPath();

	fillColor(m_fill);
	strokeColor(m_stroke);
	strokeWidth(m_stroke_w);

	rect(0.f, 0.f, w, h);

	fill();
	stroke();

	closePath();

	beginPath();

	strokeColor(UIConfig::graph_top_stroke);
	strokeWidth(UIConfig::graph_top_w);

	moveTo(0, 1);
	lineTo(w, 1);

	stroke();

	closePath();

	translate(UIConfig::graph_margin, UIConfig::graph_margin);
	inner->setAbsolutePos(
		getAbsoluteX() + UIConfig::graph_margin,
		getAbsoluteY() + UIConfig::graph_margin );

	inner->drawBackground();
	inner->drawGrid();
	inner->drawInOutLabels();

	inner->flipYAxis();

	if (inner->focusedElement != nullptr
		&& dynamic_cast<GraphVertex *>(inner->focusedElement)) {
		inner->drawAlignmentLines();
	}

	inner->drawGradient();
	inner->drawGraphLine( UIConfig::graph_edge_w,
		UIConfig::graph_edge_fg_normal,
		UIConfig::graph_edge_fg_focus );

	inner->drawInputIndicator();

	if (!inner->mustHideVertices)
		inner->drawVertices();
}

void GraphWidget::onResize(const ResizeEvent &ev)
{
	if (ev.oldSize.isNull())
		return;

	inner->setSize(calcInnerSize());
}

void GraphWidget::rebuildFromString(const char *serializedGraph)
{
	inner->rebuildFromString(serializedGraph);
}

void GraphWidget::reset()
{
	inner->reset();
}

void GraphWidget::updateInput(const float input)
{
	inner->updateInput(input);
}

void GraphWidget::setGraphGradientMode(GraphGradientMode graphGradientMode)
{
	inner->setGraphGradientMode(graphGradientMode);
}

void GraphWidget::setHorWarpAmt(const float amt)
{
	inner->setHorWarpAmt(amt);
}

void GraphWidget::setHorWarpMode(const graphdyn::WarpMode mode)
{
    inner->setHorWarpMode(mode);
}

void GraphWidget::setVerWarpAmt(const float amt)
{
    inner->setVerWarpAmt(amt);
}

void GraphWidget::setVerWarpMode(const graphdyn::WarpMode mode)
{
    inner->setVerWarpMode(mode);
}

void GraphWidget::setMustHideVertices(const bool hide)
{
    inner->setMustHideVertices(hide);
}

auto GraphWidget::calcInnerSize() -> Size<uint>
{
	return Size<uint>(
		getWidth()  - UIConfig::graph_margin*2,
		getHeight() - UIConfig::graph_margin*2
	);
}

END_NAMESPACE_DISTRHO
