#pragma once

struct HDLBgImageParameter {
	void resize(int size) {
		m_images.resize(size);
		m_imagesMax = float(size - 1);
		setIndex(0.f);
	}

	void setBounds(Rectangle<int> bounds) {
		for(auto& i: m_images)
			i = Image(Image::PixelFormat::ARGB, bounds.getWidth(), bounds.getHeight(), true);
	}

	void setIndex(float idx) {
		m_idxParam = idx;
		m_idx = int(m_idxParam * m_imagesMax);
	}

	void draw(Graphics& g) { g.drawImage(m_images[m_idx], m_images[m_idx].getBounds().toFloat()); }
	void draw(Graphics& g, Rectangle<float> bounds, int rp = RectanglePlacement::stretchToFit, bool fillAlphaWithCurrentBrush = false) {
		g.drawImage(m_images[m_idx], bounds, rp, fillAlphaWithCurrentBrush);
	}

	void makeHugeImages() {
		Random random;
		auto widthF = float(m_images[0].getWidth());
		auto heightF = float(m_images[0].getHeight());
		auto m_imagesMaxInv = 1.f / m_imagesMax;
		auto planetMaxSizeInv = 1.f / m_planetMaxSize;
		std::vector<Rectangle<float>> planets, craters;
		planets.resize(m_planetsAmount);
		craters.resize(m_cratersAmount);

		for (auto& p : planets) {
			auto x = random.nextFloat() * (widthF * 2.f - widthF);
			auto y = random.nextFloat() * (heightF * 2.f - heightF);
			auto size = std::pow(random.nextFloat(), 3) * m_planetMaxSize;

			p = Rectangle<float>(x, y, size, size);
		}

		for (auto i = 0; i < m_images.size(); ++i) {
			auto iF = float(i);
			auto iRel = iF * m_imagesMaxInv;

			Graphics g{ m_images[i] };

			auto moonColourI = Colours::white.interpolatedWith(m_moonColour, iRel);
			g.setColour(moonColourI);
			for (auto& p : planets) {
				auto relSize = p.getWidth() * planetMaxSizeInv;
				auto distanceFactor = std::pow(relSize, 2) * MathConstants<float>::twoPi;
				auto planetRotation = AffineTransform::rotation(iRel * distanceFactor, widthF * .65f, heightF * .6f);
				auto p2 = p.transformed(planetRotation);

				if (p2.getX() > -1.f && p2.getX() < widthF &&
					p2.getY() > -1.f && p2.getY() < heightF)
					g.fillEllipse(p2);
			}

			HDLLight moon;
			moon.setBounds(m_images[i].getBounds());
			moon.setImage(1.3f, .65f, .6f, moonColourI, .1f, .97f);
			struct moonPostProcessing : public HDLImagePPHelper {
				void addFX() override { alphaGate(.1f); }
			};
			moonPostProcessing pp;
			pp.process(moon.image);
			moon.draw(g);

			if (i == 0) {
				auto craterIdx = 0;
				while (craterIdx < m_cratersAmount) {
					auto lineSize = random.nextFloat() * moon.lightRadius;
					Line<float> line(0.f, 0.f, 0.f, lineSize);
					line.applyTransform(
						AffineTransform::rotation(MathConstants<float>::twoPi * random.nextFloat()));
					line.applyTransform(AffineTransform::translation(
						moon.centreX,
						moon.centreY));

					auto craterSize = moon.lightRadius - lineSize + 1.f;
					auto craterRadius = craterSize * .5f;
					auto craterX = line.getEndX() - craterRadius;
					auto craterY = line.getEndY() - craterRadius;
					auto curCrater = Rectangle<float>(craterX, craterY, craterSize, craterSize);

					bool craterIntersects = false;
					for (auto c = 0; c < craterIdx; ++c)
						if (craters[c].intersects(curCrater))
							craterIntersects = true;
					if (!craterIntersects) {
						craters[craterIdx] = curCrater;
						++craterIdx;
						HDLLight crater;
						crater.setBounds(curCrater.toNearestInt());
						crater.setImage(1.f, .5f, .5f, Colours::black.withAlpha(iRel * .1f + .1f), .95f, .99f);
						crater.draw(g, curCrater);
					}
				}
			}
			else {
				for (auto c : craters) {
					HDLLight crater;
					crater.setBounds(c.toNearestInt());
					crater.setImage(1.f, .5f, .5f, Colours::black.withAlpha(iRel * .1f + .1f), .95f, .99f);
					crater.draw(g, c);
				}
			}
		}
	}

	void setImage(int planetsAmount = 128, float planetMaxSize = 3.f, int cratersAmount = 7,
		Colour moonColour = Colours::greenyellow) {
		m_planetsAmount = planetsAmount;
		m_planetMaxSize = planetMaxSize;
		m_cratersAmount = cratersAmount;
		m_moonColour = moonColour;

		makeHugeImages();
	}

	Image& getImage() { return m_images[m_idx]; }

	int m_idx;
	float m_idxParam, m_imagesMax;
	std::vector<Image> m_images;

	int m_planetsAmount, m_cratersAmount;
	float m_planetMaxSize;
	Colour m_moonColour;
};