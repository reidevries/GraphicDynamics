/* created by rei de vries 2021-02-17
 * attempting to recreate menu as a widget to avoid opening a new window
 */

#ifndef GRAPHDYN_MENUWIDGET_H_
#define GRAPHDYN_MENUWIDGET_H_

#include "Widget.hpp"
#include "NanoVG.hpp"
#include "WolfWidget.hpp"
#include "Margin.hpp"
#include "Config.hpp"
#include "Mathf.hpp"
#include <vector>
#include <array>
#include <string>
#include <utility>

START_NAMESPACE_DISTRHO

class MenuWidget : public WolfWidget
{
public:
	struct Item
	{
		const int id; // item is considered a section if id < 0
		const std::string name;
		const std::string description;
		bool enabled = true;
		Item(const int _id,
			const std::string& _name,
			const std::string& _description)
			: id(_id),
			  name(_name),
			  description(_description),
			  enabled(true)
		{
		}

		// constructor for sections
		Item(const std::string& _name)
			: id(-1),
			  name(_name),
			  description(""),
			  enabled(true)
		{
		}
	};

	class Callback
	{
	public:
		virtual ~Callback() {}
		virtual void menuItemSelected(const int id) = 0;
		// this is necessary to maintain the stored state of mouse buttons in
		// the parent class
		virtual void propagateMouseEvent(const MouseEvent& ev) = 0;
	};

	explicit MenuWidget(NanoWidget *widget) noexcept;

	// shows and hides the widget without affecting the elements
	void show(const Point<int>& click_pos,
		const Rectangle<int>& parent_widget_bounds);
	void hide();

	// clear all sections and items
	void clear();

	// add items in sections from array for convenience
	template<size_t t_size>
	void addItems(std::array<Item, t_size> new_items)
	{
		for (auto item : new_items) {
			this->items.push_back(item);
			updateMaxItemWidth(item);
		}
	}

	// find the index of the first item with a matching name/id
	// obviously this won't work if two items have the same name/id
	auto findItemIndex(const std::string& name) -> int;
	auto findItemIndex(const int id) -> int;
	// if you disable an item that's a section header, all the items
	// until the next section header will be disabled too
	void setAllItemsEnabled(const bool enabled);
	void setItemEnabled(const uint index, const bool enabled);
	void setItemEnabled(const std::string& name, const bool enabled);

	void setRegularFontSize(const uint size) noexcept;
	void setSectionFontSize(const uint size) noexcept;

	template<typename T>
	auto getBounds() const -> Rectangle<T>
	{
		return Rectangle<T>(0, 0,
			static_cast<T>(Widget::getWidth()),
			static_cast<T>(Widget::getHeight())
		);
	}

	void setCallback(Callback *callback) noexcept {this->callback = callback;}

protected:
	void onNanoDisplay() override;
	auto onMouse( const MouseEvent &)  -> bool override;
	auto onMotion(const MotionEvent &) -> bool override;

	DGL_NAMESPACE::Rectangle<float> getBoundsOfItem(const int i);

private:
	std::vector<Item> items;

	// if the mouse moves outside these bounds, the menu will close.
	// updated on show. assumes the bounds won't change while the menu is open.
	// this is a safe assumption as long as the bounds don't include a resize
	// handle.
	Rectangle<int> parent_widget_bounds;

	float max_item_w_px;

	int hover_i;
	int selected_i;

	Margin margin;
	float font_item_size;
	float font_section_size;

	const Color border_color;
	const Color background_color;
	const Color background_hover_color;
	const Color font_item_color;
	const Color font_item_hover_color;
	const Color font_section_color;

	Callback *callback;

	void updateMaxItemWidth(const Item& item);
	void adaptSize();
	auto getItemWidthPx(const Item& item) const -> float;
	auto getItemBoundsPx(const int index) -> Rectangle<float>;
};

END_NAMESPACE_DISTRHO

#endif // GRAPHDYN_MENUWIDGET_H_
