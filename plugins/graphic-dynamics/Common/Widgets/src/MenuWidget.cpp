#include "MenuWidget.hpp"

MenuWidget::MenuWidget(NanoWidget *widget, Size<uint> size) noexcept
	: WolfWidget(widget),
	  callback(nullptr),
	  visible(false),
	  font_item_size(17.0f),
	  font_section_size(14.0f),
	  hover_i(-1),
	  border_color(CONFIG_NAMESPACE::menu_border_color),
	  background_color(CONFIG_NAMESPACE::menu_background_color),
	  background_selected_color(
		  CONFIG_NAMESPACE::menu_background_selected_color),
	  font_item_color(CONFIG_NAMESPACE::menu_font_item_color),
	  font_selected_color(CONFIG_NAMESPACE::menu_font_selected_color),
	  font_section_color(CONFIG_NAMESPACE::menu_font_section_color),
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
	this->font_item_size = size;
}

void MenuWidget::setSectionFontSize(const uint size) const noexcept
{
	this->font_section_size = size;
}

void MenuWidget::onNanoDisplay() override
{
	if (items.size() == 0) return; // don't render an empty menu

	const float h = NanoWidget::getHeight();
	const float w = NanoWidget::getWidth();

	beginPath();

	Rectangle<float> bounds;
	fontSize(font_item_size);
	textAlign(ALIGN_LEFT | ALIGN_TOP);
	textBounds(0, 0, items[0].name.c_str(), NULL, bounds);
	//NOTE: this is copied from RightClickMenu.cpp, need to look into why
	//items[0] name is used as an argument

	fillColor(background_selected_color);

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
			fillColor(background_selected_color);
			rect(0, vertical_offset, width - margin.right, font_item_size);
			fill();
			closePath();
		}

		beginPath();

		int left_offset=0;
		if (item.is_section) {
			fontSize(font_section_size);
			fillColor(font_section_color);
		} else {
			left_offset = font_section_size;
			fontSize(font_item_size);
			fillColor(font_item_color);
		}

		text(left_offset, vertical_offset, item.name.c_str(), NULL);

		if (item.description.size() > 0) {
			fontSize(font_section_size);
			fillColor(font_section_color);

			text(getTextWidthPx(item.name) + font_section_size, vertical_offset,
				 item.description.c_str(), NULL);
		}

		if (item.selected) {
			fontSize(font_item_size);
			fillColor(font_item_color);
			text(0, verticalOffset, "✓", NULL);
		}

		vertical_offset += bounds.getHeight(); //TODO: what's bounds?

		closePath();
	}
}

void MenuWidget::updateMaxNameLen(const std::string name)
{
	uint name_w_chars = name.size(); //NOTE: assumes UTF8
	if (name_w_chars > max_name_w_chars) {
		max_name_w_chars = name_w_chars;
		max_name_w_px = name_w_chars*font_item_size;
	}
}

auto MenuWidget::getMenuItemWidthPx(MenuItem& item) const -> float
{
	if (item.is_section) {
		return (item.name.size() + item.description.size()) * font_section_size;
	} else {
		return item.name.size()*font_item_size
			+ item.description.size()*font_section_size;
	}
}
