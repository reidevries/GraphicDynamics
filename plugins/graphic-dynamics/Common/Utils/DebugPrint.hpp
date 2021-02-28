#ifndef __DEBUGPRINT_HPP_
#define __DEBUGPRINT_HPP_

#include "Widget.hpp"
#include "Geometry.hpp"
#include <sstream>
#include <iostream>

START_NAMESPACE_DGL
template<typename T>
auto operator<<(std::ostream& output, const Point<T>& p) -> std::ostream&
{
	output << p.getX() << "×" << p.getY();
	return output;
}
END_NAMESPACE_DGL

namespace graphdebug
{
auto printMouseEvent(const Widget::MouseEvent& ev) -> std::stringstream;
}

#endif // __DEBUGPRINT_HPP_
