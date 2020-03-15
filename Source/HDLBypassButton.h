#pragma once

class HDLBypassButton : public LookAndFeel_V4 {
public:
	HDLBypassButton(){}

	void drawToggleButton(Graphics& g, ToggleButton& button,
		bool shouldDrawAsHighlighted, bool shouldDrawAsDown) override {

		auto widthF = float(button.getWidth());
		auto heightF = float(button.getHeight());
		auto buttonArea = Rectangle<float>(0.f, 0.f, widthF, heightF);

		if(shouldDrawAsHighlighted)
			g.setColour(findColour(420).withRotatedHue(.5f));
		else
			if(!button.getToggleState())
				g.setColour(findColour(420));
			else
				g.setColour(findColour(420).withAlpha(.25f));
		g.fillRoundedRectangle(buttonArea, 3.f);

		auto arcMargin = heightF / 8;
		auto arcArea = buttonArea.reduced(arcMargin);
		auto startAngle = MathConstants<float>::halfPi * .25f;
		auto endAngle = startAngle * 15.f;
		auto arcThicness = 3.f;

		Path arc;
		arc.addArc(arcArea.getX() , arcArea.getY(), arcArea.getWidth(), arcArea.getHeight(), startAngle, endAngle, true);

		g.setColour(Colours::black);
		g.strokePath(arc, PathStrokeType(arcThicness));

		auto x = widthF * .5f;
		auto y = arcMargin;
		auto lineHeight = heightF * .5f;

		Line<float> line(x, y, x, lineHeight);
		g.drawLine(line, arcThicness);
	}
};