#include "MenuWidget.hpp"

MenuWidget::MenuWidget(NanoWidget *widget, Size<uint> size) noexcept
	: WolfWidget(widget),
	  callback(nullptr),
	  visible(false),
	  font_size(17.0f),
	  section_font_size(14.0f),
	  hover_i(-1),
	  border_color(CONFIG_NAMESPACE::right_click_menu_border_color),
	  margin(Margin(7,15,7,13))
{
	setSize(size);
}

void MenuWidget::show(Point<int> pos)
{
	// from pdesaulniers's wolf shaper:
	// "we don't want the mouse to intersect with the popup straight away,
	// so we add a bit of margin"
	pos += Point<int>(2,2);
	Widget::setAbsolutePos(pos);
	visible = true;
}

void MenuWidget::hide()
{
	visible = false;
}

void MenuWidget::clear()
{
	sections.clear();
	max_len_item_name = 0;
}

void MenuWidget::addItem(uint section, const char * name,
	const char * description)
{
	DISTRHO_SAFE_ASSERT(section < sections.size());
	sections[section].emplace_back(name, description);
	updateMaxNameLen(name);
}

void MenuWidget::addSection(const char * name, const char * description)
{
	sections.emplace_back(name, description);
	updateMaxNameLen(name);
}

auto MenuWidget::findSectionIndex(const char * name) -> int
{
	for (size_t i = 0; i < sections.size(); ++i) {
		if (strcmp(sections[i], name) == 0) return i;
	}
	return -1;
}

void MenuWidget::updateMaxNameLen(const char * name)
{
	uint name_len = strlen(name);
	if (name_len > max_name_len) max_name_len = name_len;
}
