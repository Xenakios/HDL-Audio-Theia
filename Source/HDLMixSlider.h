#pragma once

class HDLSpirographSlider : public LookAndFeel_V4 {
public:
	void drawRotarySlider(Graphics& g, int x, int y, int width, int height,
		float sliderPos, float startAngle,
		float endAngle, Slider& slider) override{ }
};