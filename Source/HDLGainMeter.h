#pragma once

class HDLGainMeter {
public:
	HDLGainMeter() : m_magnitude(0.f), m_gain(0.f), m_release(.2f), m_fpsInv(0.f), m_samplesCount(1.f), m_imageIdx(0),
		m_imageIdxCache(0), m_fps(-1), m_sampleIdx(0), m_sampleRate(1.), m_imagesMax(0), m_imagesMaxInv(0.f), m_isReady(0) {}

	// set amount of images.
	void setImagesCount(int imagesCount) {
		m_imagesMax = imagesCount - 1;
		m_imagesMaxInv = 1.f / float(m_imagesMax);
		m_images.resize(imagesCount);
	}

	// set width of each image.
	void setImageBounds(int imageWidth) {
		for (auto& i : m_images)
			i = Image(Image::ARGB, imageWidth, imageWidth, true);
	}

	// set release of envelope follower. 0-1
	void setRelease(float release) { m_release = release; }

	// set frames per secound
	void setFPS(int fps) {
		if (m_fps != fps) {
			m_fps = fps;
			m_fpsInv = 1.f / float(m_fps);
			calculateSamplesCount();
		}
	}

	// set sampleRate
	void setSampleRate(double sampleRate) {
		if (m_sampleRate != sampleRate) {
			m_sampleRate = sampleRate;
			calculateSamplesCount();
		}
	}

	// implement the method void setImages(), set m_isReady = true and add images to m_image!

	// this is called in the sample/channel-loop in processBlock. *sample = buffer.getReadPointer();
	void setSample(float sample) {
		if (++m_sampleIdx < m_samplesCount) {
			auto absSample = std::abs(sample);
			if (m_magnitude < absSample)
				m_magnitude = absSample;
		}
		else {
			if (m_gain < m_magnitude)
				m_gain = m_magnitude;
			else
				m_gain = m_gain + m_release * (m_magnitude - m_gain);
			if (m_gain > 1.f)
				m_gain = 1.f;

			m_sampleIdx = 0;
			m_magnitude = 0.f;
			m_imageIdx.store(int(m_gain * m_imagesMax));
		}
	}

	// called in timerCallback to check if repaint is needed
	bool shouldRepaint() {
		const auto curIdx = m_imageIdx.load();

		if (!m_isReady || m_imageIdxCache == curIdx)
			return false;

		m_imageIdxCache = curIdx;
		m_imageIdx.store(curIdx);
		return true;
	}

	// draw in paint().
	void draw(Graphics& g, Rectangle<float> bounds) { g.drawImage(m_images[m_imageIdx], bounds); }

private:
	// calculates how many samples to calc before updating m_imageIdx
	void calculateSamplesCount() {
		m_samplesCount = float(m_sampleRate) * m_fpsInv;
		m_sampleIdx = int(m_samplesCount + 1.f);
	}

	float m_magnitude, m_gain, m_release, m_fpsInv, m_samplesCount;
	std::atomic<int> m_imageIdx;
	int m_imageIdxCache, m_fps, m_sampleIdx;
	double m_sampleRate;
protected:
	int m_imagesMax;
	float m_imagesMaxInv;
	bool m_isReady;
	std::vector<Image> m_images;
};

class HDLGainMeterExample : public HDLGainMeter {
public:
	void setImages(ColourGradient gradient) {
		for (auto i = 0; i < m_images.size(); ++i) {
			auto iF = float(i);
			auto iRel = iF * m_imagesMaxInv;
			
			auto c = Colours::black.interpolatedWith(Colours::white, iRel);

			for (auto y = 0; y < m_images[i].getHeight(); ++y)
				for (auto x = 0; x < m_images[i].getWidth(); ++x)
					m_images[i].setPixelAt(x, y, c);
		}
		m_isReady = true;
	}
};

class HDLGainMeterLight : public HDLGainMeter{
public:
	void setImages(ColourGradient gradient) {
		for (auto i = 0; i < m_images.size(); ++i) {
			auto iF = float(i);
			auto iRel = iF * m_imagesMaxInv;

			HDLLight light;
			light.setBounds(m_images[i].getBounds());
			light.setImage(1.f, .5f, .5f, gradient.getColourAtPosition(iRel), iRel, iRel * .9f);
			for (auto y = 0; y < m_images[i].getHeight(); ++y)
				for (auto x = 0; x < m_images[i].getWidth(); ++x)
					m_images[i].setPixelAt(x, y, light.image.getPixelAt(x, y));
		}
		m_isReady = true;
	}

	void setImages(float scaleStart, float scaleEnd, Colour c,
		float pupilStart, float pupilEnd, float skewStart, float skewEnd) {
		for (auto i = 0; i < m_images.size(); ++i) {
			auto iF = float(i);
			auto iRel = iF * m_imagesMaxInv;

			auto scale = scaleStart + iRel * (scaleEnd - scaleStart);
			auto pupil = pupilStart + iRel * (pupilEnd - pupilStart);
			auto skew = skewStart + iRel * (skewEnd - skewStart);

			HDLLight light;
			light.setBounds(m_images[i].getBounds());
			light.setImage(scale, .5f, .5f,
				Colours::transparentBlack.interpolatedWith(c, iRel),
				pupil, skew);

			m_images[i] = light.image.createCopy();
			/*
			for (auto y = 0; y < m_images[i].getHeight(); ++y)
				for (auto x = 0; x < m_images[i].getWidth(); ++x)
					m_images[i].setPixelAt(x, y, light.image.getPixelAt(x, y)); */
		}
		m_isReady = true;
	}
};

class HDLGainMeterSpirograph : public HDLGainMeter {
public:
	void setImages(int armsStart, int armsEnd, int edgesStart, int edgesEnd, float twistsStart, float twistsEnd,
		float scaleStart, float scaleEnd, float thicnessStart, float thicnessEnd,
		Colour spiroColourStart, Colour spiroColourEnd, Colour armsColourStart, Colour armsColourEnd) {
		for (auto i = 0; i < m_images.size(); ++i) {
			auto iF = float(i);
			m_iRel = iF * m_imagesMaxInv;

			auto arms = int(armsStart + m_iRel * float(armsEnd - armsStart));
			auto edges = int(edgesStart + m_iRel * float(edgesEnd - edgesStart));
			auto twists = twistsStart + m_iRel * (twistsEnd - twistsStart);
			auto scale = scaleStart + m_iRel * (scaleEnd - scaleStart);
			auto thicness = thicnessStart + m_iRel * (thicnessEnd - thicnessStart);
			auto spirographColour = spiroColourStart.interpolatedWith(spiroColourEnd, m_iRel);
			auto armsColour = armsColourStart.interpolatedWith(armsColourEnd, m_iRel);

			HDLSpirograph spirograph(m_images[i].getWidth());
			spirograph.setImage(arms, edges, twists, scale, thicness, 1.f, spirographColour, armsColour);
			for (auto y = 0; y < m_images[i].getHeight(); ++y)
				for (auto x = 0; x < m_images[i].getWidth(); ++x)
					m_images[i].setPixelAt(x, y, spirograph.image.getPixelAt(x, y));
		}
		m_isReady = true;
	}
private:
	float m_iRel;
};

class HDLGainMeterCircles : public HDLGainMeter {
public:
	// bug: it draws weird points from bottomRight to center

	void setImages(ColourGradient lineGradient, ColourGradient fillGradient) {
		for (auto i = 0; i < m_images.size(); ++i) {
			Graphics g{ m_images[i] };
			for (auto x = 0; x < i; ++x) {
				auto iF = float(x);
				auto iRel = iF * m_imagesMaxInv;
				auto iRelRev = 1.f - iRel;
				auto iRelRevWidth = iRelRev * float(m_images[i].getWidth());
				auto circleArea = m_images[i].getBounds().toFloat().reduced(iRelRevWidth);

				g.setColour(fillGradient.getColourAtPosition(iRel));
				g.fillEllipse(circleArea);
				g.setColour(lineGradient.getColourAtPosition(iRel));
				g.drawEllipse(circleArea, 2.f);
			}
		}
		m_isReady = true;
	}
};