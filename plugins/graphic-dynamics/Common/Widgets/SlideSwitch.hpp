#ifndef __SLIDE_SWITCH_HPP_INCLUDED
#define __SLIDE_SWITCH_HPP_INCLUDED

#include "NanoSwitch.hpp"

START_NAMESPACE_DISTRHO

class SlideSwitch : public NanoSwitch
{
public:
    explicit SlideSwitch(NanoWidget *widget, Size<uint> size) noexcept;

protected:
    void drawHandle();
    void drawSocket();

    void draw() override;
    
private:
    DISTRHO_LEAK_DETECTOR(SlideSwitch)
};

END_NAMESPACE_DISTRHO

#endif
