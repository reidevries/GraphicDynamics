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

#include <vector>
#include <array>
#include <string>

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
	};

	class Callback
	{
	public:
		virtual ~Callback() {}
		virtual void menuItemSelected(Item& item) = 0;
	};

	explicit MenuWidget(NanoWidget *widget) noexcept;

	// shows and hides the widget without affecting the elements
	void show(Point<int> pos);
	void show(int pos_x, int pos_y) { show(Point<int>(pos_x,pos_y)); }
	void hide();

	// clear all sections and items
	void clear();

	// add items by pushing them to the end of the list
	void addItem(const Item item);

	// add item from array for convenience
	template<size_t t_size>
	void addItems(std::array<Item, t_size>)
	{
		for (auto item : items) addItem(item);
	}

	//TODO: Find a better data structure to handle sections so that they don't
	// have to be indexed by name
	void setItemEnabled(const int id, const bool enabled);
	void setSectionEnabled(const std::string name, const bool enabled);

	// find the index of the first section with a matching name
	// obviously this won't work if two sections have the same name
	auto findItemIndexByName(const std::string name) -> int;
	auto findItemIndexByID(const int id) -> int;

	void setRegularFontSize(const uint size) noexcept;
	void setSectionFontSize(const uint size) noexcept;
	void setCallback(Callback * callback) noexcept {this->callback = callback;}

protected:
	void onNanoDisplay() override;
	auto onMouse( const MouseEvent &)  -> bool override;
	auto onMotion(const MotionEvent &) -> bool override;
	//NOTE: this scroll animation is useless rn, but it's kinda fun
	auto onScroll(const ScrollEvent &) -> bool override;

	DGL_NAMESPACE::Rectangle<float> getBoundsOfItem(const int i);

private:
	std::vector<Item> items;

	float max_item_w_px;

	int hover_i;
	int selected_i;

	Margin margin;
	float font_item_size;
	float font_section_size;

	const Color border_color;
	const Color background_color;
	const Color background_selected_color;
	const Color font_item_color;
	const Color font_selected_color;
	const Color font_section_color;

	Callback *callback;

	void updateMaxItemWidth(const Item& item);
	void adaptSize();
	auto getItemWidthPx(const Item& item) const -> float;
	auto getItemBoundsPx(const int index) -> Rectangle<float>;
};

END_NAMESPACE_DISTRHO

#endif // GRAPHDYN_MENUWIDGET_H_
