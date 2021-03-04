#include "MenuWidget.hpp"

#include <iostream>
#include "DebugPrint.hpp"

START_NAMESPACE_DISTRHO

MenuWidget::MenuWidget( NanoWidget *widget ) noexcept
	: WolfWidget(widget),
	  hover_i(-1),
	  selected_i(-1),
	  margin(Margin(7,15,7,13)),
	  font_item_size(UIConfig::menu_font_item_size),
	  font_section_size(UIConfig::menu_font_section_size),
	  callback(nullptr)
{
}

void MenuWidget::show(const Point<int>& click_pos,
	const Rectangle<int>& parent_widget_bounds)
{
	this->parent_widget_bounds = parent_widget_bounds;
	adaptSize();

	// move the menuwidget so that it always appears within the parent widget
	// bounds
	const auto menu_br = Point<int>(
		click_pos.getX() + getWidth(),
		click_pos.getY() + getHeight()
	);
	const auto parent_br = graphdyn::getBottomRight<int>(parent_widget_bounds);
	auto show_pos = click_pos;
	if (menu_br.getX() > parent_br.getX()) show_pos.moveBy(-getWidth(), 0);
	if (menu_br.getY() > parent_br.getY()) show_pos.moveBy(0, -getHeight());

	// set the cursor style to the default until the user hovers over an item
	getParentWindow().setCursorStyle(Window::CursorStyle::Default);

	NanoWidget::setAbsolutePos(show_pos);
	NanoWidget::show();
}

void MenuWidget::hide()
{
	NanoWidget::hide();
}

void MenuWidget::clear()
{
	items.clear();
	max_item_w_px = 0;
	hover_i = -1;
	selected_i = -1;
}

auto MenuWidget::findItemIndex(const std::string& name) -> int
{
	for (size_t i = 0; i < items.size(); ++i) {
		if (items[i].name == name) return i;
	}
	return -1;
}

auto MenuWidget::findItemIndex(const int id) -> int
{
	for (size_t i = 0; i < items.size(); ++i) {
		if (items[i].id == id) return i;
	}
	return -1;
}

void MenuWidget::setAllItemsEnabled(const bool enabled)
{
	for (auto &item : items) item.enabled = enabled;
}

void MenuWidget::setItemEnabled(const uint index, const bool enabled)
{
	items[index].enabled = enabled;
}

void MenuWidget::setItemEnabled(const std::string &name, const bool enabled)
{
	const int index = findItemIndex(name);
	if (index > 0) setItemEnabled(index, enabled);
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

	fillColor(UIConfig::menu_background_color);

	strokeColor(UIConfig::menu_border_color);
	strokeWidth(3.0f);

	rect(0, 0, w, h);
	fill();
	stroke();

	closePath();

	float vertical_offset = 0;
	bool cur_section_enabled = true;

	translate(margin.left, margin.top);

	for (size_t i = 0; i < items.size(); ++i) {
		Item& item = items[i];

		// black out items if section is disabled
		const bool is_section = (item.id < 0);
		if (is_section) cur_section_enabled = item.enabled;
		const bool is_enabled = (cur_section_enabled && item.enabled);
		const bool is_hovered = (static_cast<int>(i) == hover_i);

		// draw highlight background for hovered item
		if (is_hovered
			&& !is_section
			&& is_enabled) {
			beginPath();
			fillColor(UIConfig::menu_background_hover_color);
			rect(0, vertical_offset, w - margin.right, font_item_size);
			fill();
			closePath();
		}

		beginPath();

		int left_offset=0;
		if (is_section) {
			fontSize(font_section_size);
			fillColor(UIConfig::menu_font_section_color);
		} else {
			left_offset = font_section_size;
			fontSize(font_item_size);
			if (is_enabled) {
				if (is_hovered) fillColor(UIConfig::menu_font_item_hover_color);
				else fillColor(UIConfig::menu_font_item_color);
			} else {
				fillColor(UIConfig::menu_font_section_color);
			}
		}

		text(left_offset, vertical_offset, item.name.c_str(), NULL);

		if (static_cast<int>(i) == selected_i) {
			text(0, vertical_offset, "✓", NULL);
		}

		// render description if an item has one
		if (item.description.size() > 0) {
			fontSize(font_section_size);
			fillColor(UIConfig::menu_font_section_color);

			text(item.name.size()*font_item_size + font_section_size,
				 vertical_offset, item.description.c_str(), NULL);
		}

		vertical_offset += text_h;

		closePath();
	}
}

auto MenuWidget::onMouse(const MouseEvent& ev) -> bool
{
	const auto bounds = getBounds<float>();
	const auto mouse_pos = Point<float>(
		static_cast<float>(ev.pos.getX()),
		static_cast<float>(ev.pos.getY())
	);

	callback->propagateMouseEvent(ev);

	if (ev.press == true) {
		if (!bounds.contains(mouse_pos)) {
			NanoWidget::hide();
			return true;
		}

		bool cur_section_enabled = true;
		for (size_t i = 0; i < items.size(); ++i) {
			bool is_section = (items[i].id < 0);
			if (is_section) cur_section_enabled = items[i].enabled;
			bool is_enabled = cur_section_enabled && items[i].enabled;

			if (!is_section && is_enabled) {
				Rectangle<float> bounds = getItemBoundsPx(i);
				bounds.setWidth(Widget::getWidth() - margin.right);

				if (bounds.contains(mouse_pos)) {
					callback->menuItemSelected(items[i].id);
					selected_i = i;
					NanoWidget::hide();
					return true;
				}
			}
		}
	}
	return true;
}

auto MenuWidget::onMotion(const MotionEvent& ev) -> bool
{
	// check if mouse has moved outside the bounds of the parent widget
	const auto mouse_pos_absolute = Point<int>(
		ev.pos.getX() + getAbsoluteX(),
		ev.pos.getY() + getAbsoluteY()
	);
	if (!parent_widget_bounds.contains(mouse_pos_absolute)) {
		hide();
	}

	// check if mouse is outside menu and change cursor style
	const auto menu_bounds = getBounds<int>();
	if (menu_bounds.contains(ev.pos)) {
		// update hover_i
		for (size_t i = 0; i < items.size(); ++i) {
			Rectangle<float> bounds = getItemBoundsPx(i);
			bounds.setWidth(Widget::getWidth() - margin.right);
			const Point<float> mouse_pos = Point<float>(
				static_cast<float>(ev.pos.getX()),
				static_cast<float>(ev.pos.getY())
			);

			if (static_cast<int>(i) != selected_i
				&& items[i].id >= 0 // item must not be a section header
				&& bounds.contains(mouse_pos)) {
				getParentWindow().setCursorStyle(Window::CursorStyle::Grab);
				hover_i = i;
				return true;
			}
		}
	}
	getParentWindow().setCursorStyle(Window::CursorStyle::Default);
	hover_i = -1;
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
