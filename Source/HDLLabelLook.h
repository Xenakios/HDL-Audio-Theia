#pragma once

class HDLLunaLabel : public LookAndFeel_V4 {
public:
    HDLLunaLabel() {
        luna = getCustomFont();
    }

    virtual void drawLabel(Graphics& g, Label& label) override {
        auto widthF = float(label.getWidth());
        auto heightF = float(label.getHeight());
        auto labelArea = Rectangle<float>(0.f, 0.f, widthF, heightF);
        auto textHeight = label.getWidth() / label.getText().length() * 2.f;

        luna.setHeight(textHeight * .95f);
        g.setColour(findColour(420).withAlpha(.5f));
        g.setFont(luna);
        g.drawText(label.getText(), labelArea, Justification::centred, true);

        luna.setHeight(textHeight);
        g.setColour(Colours::white);
        g.setFont(luna);
        g.drawText(label.getText(), labelArea, Justification::centred, true);
    }

    virtual Font getLabelFont(Label& label) override { return luna; }

    /*
    virtual BorderSize<int> getLabelBorderSize(Label& label) {
    
    }*/

    static const Font& getCustomFont() {
        static Font custFont(Font(Typeface::createSystemTypefaceFor(BinaryData::Luna_ttf,
            BinaryData::Luna_ttfSize)));
        return custFont;
    }
private:
    Font luna;
};

