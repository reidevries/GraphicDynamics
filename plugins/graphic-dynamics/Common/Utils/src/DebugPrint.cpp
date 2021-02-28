#include "DebugPrint.hpp"

namespace graphdebug {
auto printMouseEvent(const Widget::MouseEvent& ev) -> std::stringstream {
	std::stringstream ss;
	switch (ev.button) {
	case 1:
		ss << "left   button";
		break;
	case 2:
		ss << "middle button";
		break;
	case 3:
		ss << "right  button";
		break;
	}
	if (ev.press) ss << " pressed";
	else          ss << " released";
	ss << " pos: " << ev.pos;
	return ss;
}
}
