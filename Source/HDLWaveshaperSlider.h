#pragma once

class HDLWaveshaperSlider : public LookAndFeel_V4 {
public:
    HDLWaveshaperSlider() { setColour(Slider::textBoxTextColourId, Colours::transparentBlack); }

    void makeWaveshaperImages(Colour colour,
        int images = 36, int imageSize = 128, float waveformThicness = 3.5f) {
        struct Osc {
            std::vector<float> waveform;

            Osc(int size) {
                waveform.resize(size);
                auto phase = -1.f;
                auto inc = 2.f / float(size);
                for (auto i = 0; i < size; ++i) {
                    waveform[i] = phase;
                    phase += inc;
                }
            }

            void setSine() {
                for (auto& i : waveform)
                    i = std::sin(i * MathConstants<float>::pi);
            }

            void setFancySine() {
                for (auto& i : waveform) {
                    auto ampMod = 1.f - abs(i);
                    auto freqMod = ampMod * 4.f + 4.f;
                    i = ampMod * std::sin(freqMod * i * MathConstants<float>::pi);
                }
            }
        };
        Osc osc(imageSize);
        waveshaperImages.resize(images);
        auto parameter = 1.f;
        auto inc = 7.f / float(images);
        auto zeroCross = imageSize / 2;
        for (auto i = 0; i < images; ++i) {
            waveshaperImages[i] = Image(Image::ARGB, imageSize, imageSize, true);
            Graphics g{ waveshaperImages[i] };
            g.fillAll(Colours::transparentBlack);
            Path path;
            for (auto x = 0; x < imageSize; ++x) {
                auto waveshapedWaveform = std::tanh(parameter * osc.waveform[x]);
                auto scaledWaveform = (imageSize - 1) - (waveshapedWaveform * .5f + .5f) * (imageSize - 1);
                auto difWithZero = scaledWaveform - zeroCross;
                auto direction = difWithZero < 0 ? -1 : 1;
                for (auto y = 0; y < abs(difWithZero); ++y)
                    waveshaperImages[i].setPixelAt(x, zeroCross + y * direction, colour.withAlpha(.25f));
                if (x != 0)
                    path.lineTo(float(x), scaledWaveform);
                else
                    path.startNewSubPath(0, scaledWaveform);
            }
            auto imageSizeF = float(imageSize);
            path.lineTo(imageSizeF, path.getCurrentPosition().getY());
            g.setColour(colour);
            g.strokePath(path, PathStrokeType(waveformThicness));
            parameter += inc;
            auto outlineRect = Rectangle<float>(0, 0, imageSizeF, imageSizeF);
            g.drawRect(outlineRect, waveformThicness);

            struct ImagePP : HDLImagePPHelper {
                void addFX() {
                    alphaVignette(VignetteModes::multiplyMode, .75f);
                    addNoise(.25f);
                }
            };
            ImagePP pp;
            pp.process(waveshaperImages[i]);
        }
    }

    void makeWaveshaperImages2(int images = 36, int imageSize = 128, float waveformThicness = 3.5f,
        Colour bgColour = Colours::white.withAlpha(.9f),
        Colour waveformColour = Colours::blueviolet.withAlpha(.9f),
        Colour waveformOutlineColour = Colours::blueviolet.darker(2.f).withAlpha(.9f)) {
        struct Osc {
            std::vector<float> waveform;

            Osc(int size) {
                waveform.resize(size);
                auto phase = -1.f;
                auto inc = 2.f / float(size);
                for (auto i = 0; i < size; ++i) {
                    waveform[i] = phase;
                    phase += inc;
                }
            }

            void setSine() {
                for (auto& i : waveform)
                    i = std::sin(i * MathConstants<float>::pi);
            }

            void setFancySine() {
                for (auto& i : waveform) {
                    auto ampMod = 1.f - abs(i);
                    auto freqMod = ampMod * 4.f + 4.f;
                    i = ampMod * std::sin(freqMod * i * MathConstants<float>::pi);
                }
            }
        };
        Osc osc(imageSize);
        waveshaperImages.resize(images);
        auto parameter = 1.f;
        auto inc = 7.f / float(images);
        auto zeroCross = imageSize / 2;
        for (auto i = 0; i < images; ++i) {
            waveshaperImages[i] = Image(Image::ARGB, imageSize, imageSize, true);
            Graphics g{ waveshaperImages[i] };
            g.fillAll(bgColour);
            Path path;
            for (auto x = 0; x < imageSize; ++x) {
                auto waveshapedWaveform = std::tanh(parameter * osc.waveform[x]);
                auto scaledWaveform = (imageSize - 1) - (waveshapedWaveform * .5f + .5f) * (imageSize - 1);
                auto difWithZero = scaledWaveform - zeroCross;
                auto direction = difWithZero < 0 ? -1 : 1;
                for (auto y = 0; y < abs(difWithZero); ++y)
                    waveshaperImages[i].setPixelAt(x, zeroCross + y * direction, waveformColour);
                if(x != 0)
                    path.lineTo(float(x), scaledWaveform);
                else
                    path.startNewSubPath(0, scaledWaveform);
            }
            auto imageSizeF = float(imageSize);
            path.lineTo(imageSizeF, path.getCurrentPosition().getY());
            g.setColour(waveformOutlineColour);
            g.strokePath(path, PathStrokeType(waveformThicness));
            parameter += inc;
            auto outlineRect = Rectangle<float>(0, 0, imageSizeF, imageSizeF);
            g.drawRect(outlineRect, waveformThicness);
        }
    }

    void drawRotarySlider(Graphics& g, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, Slider& slider) override {
        auto sliderArea = Rectangle<float>(float(x), float(y), float(width), float(height));
        auto sizeF = float(waveshaperImages.size());
        auto waveshaperIdx = int(slider.getValue() * double(sizeF - 1.));
        g.drawImage(waveshaperImages[waveshaperIdx], sliderArea);
    }

private:
    std::vector<Image> waveshaperImages;
};