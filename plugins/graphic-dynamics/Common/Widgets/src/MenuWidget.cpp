#include "MenuWidget.hpp"

START_NAMESPACE_DISTRHO

MenuWidget::MenuWidget(NanoWidget *widget) noexcept
	: WolfWidget(widget),
	  font_item_size(17.0f),
	  font_section_size(14.0f),
	  hover_i(-1),
	  selected_i(-1),
	  border_color(CONFIG_NAMESPACE::menu_border_color),
	  background_color(CONFIG_NAMESPACE::menu_background_color),
	  background_selected_color(
		  CONFIG_NAMESPACE::menu_background_selected_color),
	  font_item_color(CONFIG_NAMESPACE::menu_font_item_color),
	  font_selected_color(CONFIG_NAMESPACE::menu_font_selected_color),
	  font_section_color(CONFIG_NAMESPACE::menu_font_section_color),
	  margin(Margin(7,15,7,13)),
	  callback(nullptr)
{
}

void MenuWidget::show(Point<int> pos)
{
	// from pdesaulniers's wolf shaper:
	// "we don't want the mouse to intersect with the popup straight away,
	// so we add a bit of margin"
	pos += Point<int>(2,2);
	adaptSize();
	Widget::setAbsolutePos(pos);
	Widget::show();
}

void MenuWidget::hide()
{
	Widget::hide();
}

void MenuWidget::clear()
{
	items.clear();
	max_item_w_px = 0;
	hover_i = -1;
	selected_i = -1;
}

void MenuWidget::addItem(const Item item)
{
	items.push_back(item);
	updateMaxItemWidth(item);
}

void MenuWidget::setItemEnabled(const int id, const bool enabled)
{
	const int index = findItemIndexByID(id);
	items[index].enabled = enabled;
}

void MenuWidget::setSectionEnabled(const std::string name, const bool enabled)
{
	// at the moment, this will also work when given the name of an item
	// it just started at the index and sets each element from there until the
	// next section to 'enabled'
	const int index = findItemIndexByName(name);
	if (index < 0) return; // this means name didn't exist

	items[index].enabled = enabled;

	for (size_t i = index+1; i < items.size(); ++i) {
		if (items[i].id < 0) break; // stop if another section is reached
		items[i].enabled = enabled;
	}
}

auto MenuWidget::findItemIndexByName(const std::string name) -> int
{
	for (size_t i = 0; i < items.size(); ++i) {
		if (items[i].name == name) return i;
	}
	return -1;
}

auto MenuWidget::findItemIndexByID(const int id) -> int
{
	for (size_t i = 0; i < items.size(); ++i) {
		if (items[i].id == id) return i;
	}
	return -1;
}

void MenuWidget::setRegularFontSize(const uint size) noexcept
{
	this->font_item_size = size;
}

void MenuWidget::setSectionFontSize(const uint size) noexcept
{
	this->font_section_size = size;
}

void MenuWidget::onNanoDisplay()
{
	if (items.size() == 0) return; // don't render an empty menu

	const float h = NanoWidget::getHeight();
	const float w = NanoWidget::getWidth();

	beginPath();

	textAlign(ALIGN_LEFT | ALIGN_TOP);

	// these four lines are used to calculate the height of a line of text
	Rectangle<float> bounds;
	fontSize(font_item_size);
	textBounds(0, 0, items[0].name.c_str(), NULL, bounds);
	const float text_h = bounds.getHeight();

	fillColor(background_selected_color);

	strokeColor(border_color);
	strokeWidth(3.0f);

	rect(0, 0, w, h);
	fill();
	stroke();

	closePath();

	float vertical_offset = 0;

	translate(margin.left, margin.top);

	for (size_t i = 0; i < items.size(); ++i) {
		Item& item = items[i];
		if (static_cast<int>(i) == hover_i) {
			beginPath();
			fillColor(background_selected_color);
			rect(0, vertical_offset, w - margin.right, font_item_size);
			fill();
			closePath();
		}

		beginPath();

		int left_offset=0;
		if (item.id < 0) {
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

			text(item.name.size()*font_item_size + font_section_size,
				 vertical_offset, item.description.c_str(), NULL);
		}

		if (static_cast<int>(i) == selected_i) {
			fontSize(font_item_size);
			fillColor(font_item_color);
			text(0, vertical_offset, "✓", NULL);
		}

		vertical_offset += text_h;

		closePath();
	}
}

auto MenuWidget::onMouse(const MouseEvent& ev) -> bool
{
	const Rectangle<float> bounds = Rectangle<float>(
		static_cast<float>(Widget::getAbsoluteX()),
		static_cast<float>(Widget::getAbsoluteY()),
		static_cast<float>(Widget::getWidth()),
		static_cast<float>(Widget::getHeight())
	);
	const Point<float> mouse_pos = Point<float>(
		static_cast<float>(ev.pos.getX()),
		static_cast<float>(ev.pos.getY())
	);

	if (ev.press == true) {
		if (!bounds.contains(mouse_pos)) {
			NanoWidget::hide();
			return true;
		}

		for (size_t i = 0; i < items.size(); ++i) {
			Rectangle<float> bounds = getItemBoundsPx(i);
			bounds.setWidth(Widget::getWidth() - margin.right);

			if (items[i].id >= 0 && bounds.contains(mouse_pos)) {
				callback->menuItemSelected(items[i]);
				selected_i = i;
				NanoWidget::hide();
				return true;
			}
		}
	}
	return true;
}

auto MenuWidget::onMotion(const MotionEvent& ev) -> bool
{
	for (size_t i = 0; i < items.size(); ++i) {
		Rectangle<float> bounds = getItemBoundsPx(i);
		bounds.setWidth(Widget::getWidth() - margin.right);
		const Point<float> mouse_pos = Point<float>(
			static_cast<float>(ev.pos.getX()),
			static_cast<float>(ev.pos.getY())
		);

		if (static_cast<int>(i) != selected_i && bounds.contains(mouse_pos)) {
			hover_i = i;
			return true;
		}
	}
	hover_i = -1;
	return true;
}

auto MenuWidget::onScroll(const ScrollEvent& ev) -> bool
{
	const float scroll_y = ev.delta.getY();
	if (std::abs(scroll_y) < 0.5f) return true;
	if (items.size() == 0) return true;

	const Rectangle<float> bounds = Rectangle<float>(
		static_cast<float>(Widget::getAbsoluteX()),
		static_cast<float>(Widget::getAbsoluteY()),
		static_cast<float>(Widget::getWidth()),
		static_cast<float>(Widget::getHeight())
	);
	const Point<float> mouse_pos = Point<float>(
		static_cast<float>(ev.pos.getX()),
		static_cast<float>(ev.pos.getY())
	);

	if (bounds.contains(mouse_pos)) {
		hover_i += std::round(scroll_y);
		while (hover_i > items.size()) hover_i -= items.size();
		while (hover_i < 0) hover_i += items.size();
	}
	return true;
}

void MenuWidget::updateMaxItemWidth(const Item& item)
{
	max_item_w_px = std::max(max_item_w_px, getItemWidthPx(item));
}

void MenuWidget::adaptSize()
{
	NanoWidget::setSize(Size<uint>(
		max_item_w_px + margin.left + margin.right + font_section_size,
		items.size()*font_item_size + margin.top + margin.bottom
	));
}

auto MenuWidget::getItemWidthPx(const Item& item) const -> float
{
	//NOTE: assumes name, description are UTF8
	if (item.id < 0) {
		return (item.name.size() + item.description.size()) * font_section_size;
	} else {
		return item.name.size()*font_item_size
			+ item.description.size()*font_section_size;
	}
}

auto MenuWidget::getItemBoundsPx(const int index) -> Rectangle<float>
{
	fontSize(font_item_size);
	textAlign(ALIGN_LEFT | ALIGN_TOP);
	Rectangle<float> bounds;
	textBounds(margin.left, index*font_item_size + margin.top,
		items[index].name.c_str(), NULL, bounds);
	return bounds;
}

END_NAMESPACE_DISTRHO
