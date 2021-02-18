/* created by rei de vries 2021-02-17
 * attempting to recreate menu as a widget to avoid opening a new window
 */

#ifndef GRAPHDYN_MENUWIDGET_H_
#define GRAPHDYN_MENUWIDGET_H_

#include "Widget.hpp"
#include "NanoVG.hpp"
#include "WolfWidget.hpp"

#include <vector>
#include <string>

START_NAMESPACE_DISTRHO

class MenuWidget : public WolfWidget
{
public:
	class Callback
	{
	public:
		virtual ~Callback() {}
		virtual void menuItemSelected(
			MenuWidget *menu_widget, const int item) = 0;
	};

	explicit MenuWidget(NanoWidget *widget, Size<uint> size) noexcept;

	struct MenuItem
	{
		const std::string name;
		const std::string description;
		const bool is_section;
	};

	// shows and hides the widget without affecting the elements
	void show(Point<int> pos) override;

	// clear all sections and items
	void clear();

	// add items by pushing them to the end of the list
	void addItem(const MenuItem item);

	template<size_t t_size>
	void addItems(const std::array<MenuItem, t_size>);

	// find the index of the first section with a matching name
	// obviously this won't work if two sections have the same name
	auto findItemIndexByName(const std::string name) -> uint;

	void setBorderColor(const Color color) const noexcept;
	void setRegularFontSize(const uint size) const noexcept;
	void setSectionFontSize(const uint size) const noexcept;
	void setCallback(Callback * callback) noexcept {this->callback = callback;}

protected:
	void onNanoDisplay() override;
	auto onMouse( const MouseEvent &)  -> bool override;
	auto onMotion(const MotionEvent &) -> bool override;
	auto onScroll(const ScrollEvent &) -> bool override;

	virtual void onMouseHover();
	virtual void onMouseLeave();
	virtual void onMouseUp();
	virtual void onMouseDown();

	DGL_NAMESPACE::Rectangle<float> getBoundsOfItem(const int i);

private:
	std::vector<MenuItem> items;

	void updateMaxNameLen(const std::string name);
	uint max_name_w_chars;
	uint max_name_w_px;

	uint hover_i;

	Margin margin;
	float font_size;
	float section_font_size;

	Color border_color;
	Color background_color;
	Color selection_color;
	Color font_color;
	Color font_color_selected;


	Callback *callback;
}

END_NAMESPACE_DISTRHO

#endif // GRAPHDYN_MENUWIDGET_H_
