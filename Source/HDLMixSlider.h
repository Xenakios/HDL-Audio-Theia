#pragma once

class HDLSpirographSlider : public LookAndFeel_V4 {
public:
	HDLSpirographSlider() {}

	void drawRotarySlider(Graphics& g, int x, int y, int width, int height,
		float sliderPos, float startAngle,
		float endAngle, Slider& slider) override{

		g.setColour(Colours::red.withAlpha(.2f));
		g.fillRect(x,y,width,height);
	}
private:
	static inline float mix(float a, float b, float p) { return a * (1.f - p) + b * p; }
};