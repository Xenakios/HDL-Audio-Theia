#pragma once

/* HDLPaintings.h
This is where all beautiful hdl paintings are stored :3

toDo:
	features:
		HDLTree(s)
			musts work more consistently
				all trees more or less same size
				correct statics
		Spirograph
			add Nonlinear-Change to arm length
	bugs:
		HDLMoon
			can break because images can't have 0 size
*/

struct HDLPaintings {
	HDLPaintings() {};

	void setBounds(int width) {
		image = Image(Image::PixelFormat::ARGB, width, width, true);
	}

	void setBounds(Rectangle<int> bounds) {
		image = Image(Image::PixelFormat::ARGB, bounds.getWidth(), bounds.getHeight(), true);
	}

	void draw(Graphics& g){
		g.drawImage(image, image.getBounds().toFloat());
	}

	void draw(Graphics& g, Rectangle<float> bounds, int rp = RectanglePlacement::stretchToFit, bool fillAlphaWithCurrentBrush = false) {
		g.drawImage(image, bounds, rp, fillAlphaWithCurrentBrush);
	}

	Image image;
	Random random;
};

class HDLPastelForms : public HDLPaintings{
public:
	void setImage(float blobs = 70.f, float blobSizeMax = 60.f, float blobColourVariation = .3f) {
		Graphics g{ image };
		Colour bgColour = getRandomPastell();
		Colour bgDarker = bgColour.darker(1.f);
		Point<float> gradientPoints[] = {
			Point<float>(float(image.getWidth()), float(image.getHeight())),
			Point<float>(0,0)};
		ColourGradient bgGradient(bgDarker, gradientPoints[0], bgColour, gradientPoints[1], false);
		g.setGradientFill(bgGradient);
		g.fillAll();

		for (auto b = 0.f; b < blobs; ++b) {
			auto blobRelative = b / blobs;
			auto blobSize = std::pow(blobRelative, 2) * blobSizeMax;
			auto blobRadius = blobSize * .5f;
			auto blobX = random.nextFloat() * image.getWidth() - blobRadius;
			auto blobY = random.nextFloat() * image.getHeight() - blobRadius;
			auto blobColourRand = random.nextFloat();

			Rectangle<float> blobArea(blobX, blobY, blobSize, blobSize);
			g.setColour(bgColour.withRotatedHue(blobColourRand * blobColourVariation + .5f * (1.f - blobColourVariation)));
			g.fillEllipse(blobArea);
		}
	}
private:
	Colour getRandomPastell(float alpha = 1.f){ return Colour(random.nextInt()).withSaturation(.2f).withBrightness(.9f).withAlpha(alpha); }
};

class HDLLucasLights : public HDLPaintings {
public:
	void setImage(int lights = 24, float maxSize = 420.f) {
		Graphics g{ image };

		Colour transparency = Colour(0x00000000);
		for (auto l = 0; l < lights; ++l) {
			auto lightSize = maxSize * random.nextFloat();
			auto lightRadius = lightSize * .5f;
			Rectangle<float> lightArea(
				image.getWidth() * random.nextFloat() - lightRadius,
				image.getHeight() * random.nextFloat() - lightRadius,
				lightSize, lightSize);
			Point<float> lightCenter(
				lightArea.getX() + lightRadius,
				lightArea.getY() + lightRadius);
			Colour randHsvColour(
				random.nextFloat(),
				.5f, .7f, random.nextFloat());
			Point<float> lightBorder(
				lightCenter.getX(),
				lightCenter.getY() + lightRadius);
			ColourGradient gradient(randHsvColour, lightCenter, transparency, lightBorder, true);
			g.setGradientFill(gradient);
			g.fillEllipse(lightArea);
		}
	}
};

class HDLLight : public HDLPaintings {
public:
	HDLLight() {}

	void setImage(float size = 1.f, float x = .5f, float y = .5f,
		Colour lightColour = Colours::limegreen, float pupilSize = 0.f,
		float skewFactor = .5f) {
		Graphics g{ image };

		iWidth = float(image.getWidth());
		iHeight = float(image.getHeight());
		lightSize = size * jmin(iWidth, iHeight);
		lightX = x * (iWidth + lightSize) - lightSize;
		lightY = y * (iHeight + lightSize) - lightSize;
		lightRadius = lightSize * .5f;
		centreX = lightX + lightRadius;
		centreY = lightY + lightRadius;

		Rectangle<float> lightArea(
			lightX, lightY, lightSize, lightSize);

		Colour midColour = Colours::transparentBlack.interpolatedWith(lightColour, pupilSize * .5f + .5f);
		ColourGradient gradient;
		gradient.addColour(0., lightColour);
		gradient.addColour(skewFactor, midColour);
		gradient.addColour(1., Colours::transparentBlack);
		gradient.isRadial = true;
		gradient.point1 = Point<float>(centreX, centreY);
		gradient.point2 = Point<float>(centreX, centreY + lightRadius);

		g.setGradientFill(gradient);
		g.fillEllipse(lightArea);
	}

	float iWidth, iHeight, lightSize, lightX, lightY, lightRadius, centreX, centreY;
};

class HDLCloudySky : public HDLPaintings {
public:
	HDLCloudySky() {};

	void setImage(float cloudComplexity = .5f, float cloudOpacity = .4f) {
		Graphics g{ image };

		Colour skyColour(0xff0044ff);
		g.fillAll(skyColour);

		auto whiteOpacity = mix(cloudOpacity * .3f, cloudOpacity * .075f, cloudComplexity);
		Colour white = Colour(0xffffffff).withAlpha(whiteOpacity);
		Colour transparentWhite(0x00ffffff);
		
		auto minDimension = float(jmin(image.getWidth(), image.getHeight()));
		auto cloudLoops = int(cloudComplexity * 2056.f);
		for (int i = 0; i < cloudLoops; i++) {
			auto cloudSize = .5f * minDimension * random.nextFloat();
			auto cloudSizeHalf = cloudSize * .5f;
			auto cloudX = (image.getWidth() + cloudSizeHalf) * random.nextFloat() - cloudSizeHalf;
			auto cloudY = (image.getHeight() + cloudSizeHalf) * random.nextFloat() - cloudSizeHalf;
			Rectangle<float> cloudEllipse(cloudX, cloudY, cloudSize, cloudSize);
			Point<float>cloudTop(cloudX + cloudSizeHalf, cloudY);
			Point<float>cloudMid(cloudTop.getX(), cloudY + cloudSizeHalf);
			ColourGradient gradient(white, cloudTop, transparentWhite, cloudMid, false);
			g.setGradientFill(gradient);
			g.fillEllipse(cloudEllipse);
		}
	}

private:
	float mix(float a, float b, float p) { return a * (1.f - p) + b * p; }
};

class HDLTree : public HDLPaintings {
public:
	HDLTree() {
		woodColour = Colour(0xff5B0C0C);
		leafColour = Colour(0xff84BD13);
	}

	void setImage(float trunkX = .5f, float maxBranches = 8.f, float maxAngle = 1.f,
		float maxThicness = 6.f, float trunkLength = .2f, float trunkAngle = .2f,
		float angleLimit = 1.5f) {
		Graphics g{ image };
		m_maxBranches = maxBranches;
		m_maxAngle = maxAngle;
		m_maxAngleHalf = m_maxAngle * .5f;
		m_maxThicness = maxThicness;
		m_maxLength = trunkLength * image.getHeight();
		m_angleLimit = angleLimit * MathConstants<float>::halfPi;
		auto angle = MathConstants<float>::halfPi * (random.nextFloat() * trunkAngle - trunkAngle * .5f);
		auto branches = random.nextFloat() * m_maxBranches;
		
		drawBranch(g, float(image.getWidth()) * trunkX, float(image.getHeight()), m_maxLength, angle, branches);
	}
private:
	bool drawBranch(Graphics& g, float x, float y, float length, float angle, float branches) {
		if (y > 0 && length > 1.f) {
			for (auto b = 0; b < branches; ++b) {
				Line<float> branch(0.f, 0.f, 0.f, -length);
				branch.applyTransform(
					AffineTransform::rotation(angle).followedBy(
						AffineTransform::translation(x, y)));
				auto relativeLength = (length / m_maxLength);
				auto relLengthCurve = std::tanh(4.f * relativeLength);
				Colour branchColour = leafColour.interpolatedWith(woodColour, relLengthCurve);
				g.setColour(branchColour);
				auto branchThicness = m_maxThicness * relativeLength + 1.f;
				g.drawLine(branch, branchThicness);
				x = branch.getEndX();
				y = branch.getEndY();
				length *= std::tanh(2.f * random.nextFloat());
				angle += MathConstants<float>::halfPi * (random.nextFloat() * m_maxAngle - m_maxAngleHalf);
				angle = jlimit(-m_angleLimit, m_angleLimit, angle);
				branches = random.nextFloat() * m_maxBranches;

				drawBranch(g, x, y, length, angle, branches);
			}
		}
		
		return 0;
	}

	Colour woodColour, leafColour;
	float m_maxLength, m_maxBranches, m_maxAngle, m_maxAngleHalf, m_maxThicness, m_angleLimit;
};

class HDLTree2 : public HDLPaintings {
public:
	HDLTree2() {
		woodColour = Colour(0xff5B0C0C);
		leafColour = Colour(0xff84BD13).withAlpha(.15f);
	}
	void setImage(float branchLength = .15f, float branchThicness = 1.2f, int branchesPerNode = 2) {
		Graphics g{ image };
		m_width = float(image.getWidth());
		m_height = float(image.getHeight());
		m_branchLength = m_height * branchLength;
		m_branchThicness = branchThicness;
		m_branchesPerNode = float(branchesPerNode);
		m_quarterPi = MathConstants<float>::halfPi * .5f;
		auto centreX = m_width * .5f;
		for (auto b = 0; b < m_branchesPerNode; ++b) {
			auto rotation = getRotation(1.f);
			drawBranch(g, centreX, m_height, m_branchLength, rotation);
		}
	}
private:
	bool drawBranch(Graphics& g, int x, int y, float branchLength, float rotation) {
		if (y > 0 && x > 0 && y <= m_height && x < m_width && branchLength > 1.f) {
			Line<float> branch(0.f, 0.f, 0.f, -branchLength);
			auto rotate = AffineTransform::rotation(rotation);
			branch.applyTransform(rotate);
			auto translate = AffineTransform::translation(x, y);
			branch.applyTransform(translate);
			auto relativeLength = branchLength / m_branchLength;
			auto branchColour = leafColour.interpolatedWith(woodColour, relativeLength);
			g.setColour(branchColour);
			g.drawLine(branch, m_branchThicness);
			x = int(branch.getEndX());
			y = int(branch.getEndY());
			branchLength *= std::tanh(2.f * random.nextFloat());
			auto relativeHeight = y / m_height;
			for (auto b = 0; b < getBranchesPerNode(relativeHeight); ++b) {
				rotation = getRotation(relativeHeight);
				drawBranch(g, x, y, branchLength, rotation);
			}
		}
		return false;
	}

	float getBranchesPerNode(float relHeight) {
		float rNum = random.nextFloat();
		return mix(
			rNum * m_branchesPerNode,
			m_branchesPerNode,
			std::tanh(8.f * relHeight)
		);
	}

	float getRotation(float relHeight) {
		float rNum = random.nextFloat();
		return mix(
			rNum * MathConstants<float>::twoPi - MathConstants<float>::pi,
			rNum - .5f,
			relHeight);
	}

	inline float mix(float a, float b, float p) { return b * p + a * (1.f - p); }

	float m_width, m_height, m_branchLength, m_branchThicness, m_branchesPerNode, m_branchLengthDecrement, m_quarterPi;
	Colour woodColour, leafColour;
};

class HDLSpirograph : public HDLPaintings {
public:
	HDLSpirograph(int width) { setBounds(width); }

	void setImage(int arms = 5, int edges = 2056, float spirograph = 3.f, float scale = .95f,
		float spirographThicness = 3.f, float loopLength = 1.f,
		Colour spirographColour = Colours::yellow.withAlpha(.5f), Colour armsColour = Colours::limegreen.withAlpha(.2f)) {
		Graphics g{ image };
		g.setColour(armsColour);

		auto centreX = float(image.getWidth()) * .5f;
		auto centreY = float(image.getHeight()) * .5f;
		auto minDimension = float(jmin(image.getWidth(), image.getHeight()));
		auto radius = minDimension * .5f;
		auto armLength = radius / float(arms) * scale;
		std::vector<Line<float>> armVec;
		armVec.resize(arms);
		Path path;
		auto pathAlreadyStarted = false;
		auto points = float(edges);
		auto angle = 0.f;
		auto angleInc = MathConstants<float>::twoPi / points;
		points *= loopLength;
		for (auto p = 0; p < points; ++p) {
			armVec[0] = Line<float>(0.f, 0.f, 0.f, armLength);
			auto rotation = AffineTransform::rotation(angle);
			armVec[0].applyTransform(rotation);
			auto translate = AffineTransform::translation(centreX, centreY);
			armVec[0].applyTransform(translate);
			for (auto a = 1; a < arms; ++a) {
				armVec[a] = Line<float>(0.f, 0.f, 0.f, armLength);
				rotation = AffineTransform::rotation(float(a) * spirograph * angle + angle);
				armVec[a].applyTransform(rotation);
				auto lastIdx = a - 1;
				auto endX = armVec[lastIdx].getEndX();
				auto endY = armVec[lastIdx].getEndY();
				translate = AffineTransform::translation(endX, endY);
				armVec[a].applyTransform(translate);
				g.drawLine(armVec[a], 1.f);
			}
			if (pathAlreadyStarted)
				path.lineTo(armVec[armVec.size() - 1].getEnd());
			else {
				path.startNewSubPath(armVec[armVec.size() - 1].getEnd());
				pathAlreadyStarted = true;
			}
			angle += angleInc;
		}
		if (std::fmodf(loopLength, 1.f) == 0.f && std::fmodf(spirograph, 1.f) == 0)
			path.closeSubPath();
		g.setColour(spirographColour);
		g.strokePath(path, PathStrokeType(spirographThicness));
	}
};

class HDLMoon : public HDLPaintings {
public:
	void setImage(int planets = 64, int crater = 7) {
		Graphics g{ image };
		
		auto widthF = float(image.getWidth());
		auto heightF = float(image.getHeight());

		g.setColour(Colours::white);
		for (auto p = 0; p < planets; ++p) {
			auto x = random.nextFloat() * widthF;
			auto y = random.nextFloat() * heightF;
			auto size = std::pow(random.nextFloat(), 2) * 3.f;

			g.fillEllipse(x, y, size, size);
		}

		HDLLight moon;
		moon.setBounds(image.getBounds());
		moon.setImage(1.3f, .65f, .6f, Colours::white, .1f, .97f);
		struct moonPostProcessing : public HDLImagePPHelper {
			void addFX() override{ alphaGate(.1f); }
		};
		moonPostProcessing pp;
		pp.process(moon.image);
		moon.draw(g);

		std::vector<Rectangle<float>> craters;
		craters.resize(crater);
		auto craterIdx = 0;
		while (craterIdx < crater) {
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
				crater.setImage(1.f, .5f, .5f, Colours::black.withAlpha(.1f), .95f, .99f);
				crater.draw(g, curCrater);
			}
		}
	}
};