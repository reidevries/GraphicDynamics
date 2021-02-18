#include "MenuWidget.hpp"

MenuWidget::MenuWidget(NanoWidget *widget, Size<uint> size) noexcept
	: WolfWidget(widget),
	  callback(nullptr),
	  visible(false),
	  font_size(17.0f),
	  section_font_size(14.0f),
	  hover_i(-1),
	  border_color(CONFIG_NAMESPACE::menu_border_color),
	  font_color(CONFIG_NAMESPACE::menu_font_color),
	  selected_font_color(CONFIG_NAMESPACE::menu_selected_font_color),
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
	Widget::show();
}

void MenuWidget::clear()
{
	sections.clear();
	max_len_item_name = 0;
}

void MenuWidget::addItem(const MenuItem item)
{
	items.push_back(item);
	updateMaxNameLen(name);
}

template<size_t t_size>
void MenuWidget::addItems(const std::array<MenuItem, t_size> items)
{
	for (auto item : items) addItem(item);
}

auto MenuWidget::findItemIndexByName(std::string name) -> int
{
	for (size_t i = 0; i < items.size(); ++i) {
		if (items[i] == name) return i;
	}
	return -1;
}

void MenuWidget::setBorderColor(const Color color) const noexcept
{
	this->border_color = color;
}

void MenuWidget::setRegularFontSize(const uint size) const noexcept
{
	this->font_size = size;
}

void MenuWidget::setSectionFontSize(const uint size) const noexcept
{
	this->section_font_size = size;
}

void MenuWidget::onNanoDisplay() override
{
	if (items.size() == 0) return; // don't render an empty menu

	const float h = NanoWidget::getHeight();
	const float w = NanoWidget::getWidth();

	beginPath();

	Rectangle<float> bounds;
	fontSize(font_size);
	textAlign(ALIGN_LEFT | ALIGN_TOP);
	textBounds(0, 0, items[0].name, NULL, bounds);
	//NOTE: this is copied from RightClickMenu.cpp, need to look into why
	//items[0] name is used as an argument

	fillColor(Color(39, 39, 39, 255));

	strokeColor(border_color);
	strokeWidth(3.0f);

	rect(0, 0, width, height);
	fill();
	stroke():

	closePath();

	float vertical_offset = 0;

	translate(margin.left, margin.top);

	for (size_t i = 0; i < items.size(); ++i) {
		MenuItem& item = items[i];
		if (i == hover_i) {
			beginPath();
			fillColor(Color(255,255,255));
			rect(0, vertical_offset, width - margin.right, font_size);
			fill();
			closePath();
		}
	}
}

void MenuWidget::updateMaxNameLen(const std::string name)
{
	uint name_w_chars = name.size(); //NOTE: assumes UTF8
	if (name_w_chars > max_name_w_chars) {
		max_name_w_chars = name_w_chars;
		max_name_w_px = name_w_chars*font_size;
	}
}
