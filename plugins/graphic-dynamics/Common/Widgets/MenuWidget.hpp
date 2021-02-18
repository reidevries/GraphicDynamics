/* created by rei de vries 2021-02-17
 * attempting to recreate menu as a widget to avoid opening a new window
 */

#ifndef GRAPHDYN_MENUWIDGET_H_
#define GRAPHDYN_MENUWIDGET_H_

#include "Widget.hpp"
#include "NanoVG.hpp"
#include "WolfWidget.hpp"

#include <vector>

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
		const char * name;
		const char * description;
	};

	struct MenuSection
	{
		const char * name;
		const char * description;
		std::vector<MenuItem> items;
	};

	// shows and hides the widget without affecting the elements
	void show(Point<int> pos);
	void hide();

	// clear all sections and items
	void clear();

	// add sections, and add items to a section
	void addItem(uint section, const char * name, const char * description);
	void addSection(const char * name, const char * description);

	// find the index of the first section with a matching name
	// obviously this won't work if two sections have the same name
	auto findSectionIndex(const char * name) -> uint;

	void setBorderColor(const Color color) {this->border_color = color;}
	void setRegularFontSize(float size) {this->font_size = size;}
	void setSectionFontSize(float size) {this->section_font_size = size;}
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

	virtual void draw() = 0;
private:
	bool visible;

	std::vector<MenuSection> sections;

	void updateMaxNameLen(const char* name);
	uint max_name_w_chars;
	uint max_name_w_px;

	float font_size;
	float section_font_size;
	float hover_i;
	Color border_color;
	Margin margin;

	Callback *callback;
}

END_NAMESPACE_DISTRHO

#endif // GRAPHDYN_MENUWIDGET_H_
