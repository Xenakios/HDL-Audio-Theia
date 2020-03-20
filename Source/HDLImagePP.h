#pragma once

// class for post processing images to enhance their raw awesomeness even more
/*
todo:
	features:
		more mandala!
	bugs:
		setRelativeSaturation und setRelativeLuminance funzen nicht ordnungsgemäß
		getRelativeLuminance und getRelativeSaturation müssen auch geprüft werden
		rasterizeWithMeanColour macht nur kot
		kp macht keinen sinn
*/

class HDLImagePP {
public:
	HDLImagePP() {};

	static void kp(Image& image) {
		auto width = float(image.getWidth());
		auto height = float(image.getHeight());

		auto phase = 0.f;
		auto inc = 1.f / width;

		for (auto x = 0.f; x < width; x++)
			for (auto y = 0.f; y < height; y++) {
				auto wave = sin(phase * MathConstants<float>::pi);

				image.setPixelAt(int(x), int(y), image.getPixelAt(int(wave * x), int(wave * y)));

				phase += inc;
			}
	}

	static void blur(Image& i, float depth = 1.f, float opacity = 1.f) {
		Image newImage = Image(Image::ARGB, i.getWidth(), i.getHeight(), true);

		depth *= 6.f;

		for (auto x = 0; x < i.getWidth(); ++x)
			for (auto y = 0; y < i.getHeight(); ++y) {
				Array<Colour> pixel;
				for (auto pX = 0; pX < int(depth); ++pX)
					for (auto pY = 0; pY < int(depth); ++pY) {
						auto newX = x + pX;
						auto newY = y + pY;
						if (newX < i.getWidth() && newY < i.getHeight())
							pixel.add(i.getPixelAt(newX, newY));
						newX = x - pX;
						newY = y - pY;
						if (!(newX < 0 || newY < 0))
							pixel.add(i.getPixelAt(newX, newY));
					}
				auto pixelSizeDiv = 1.f / float(pixel.size());
				Colour newPixel = pixel[0];
				for (auto nP = 2; nP < pixel.size(); ++nP)
					newPixel = newPixel.interpolatedWith(pixel[nP], pixelSizeDiv);
				newImage.setPixelAt(x, y, pixel[0].interpolatedWith(newPixel, opacity));
			}

		i = newImage;
	}

	static void rasterizeWithMeanColour(Image& image, float amount = .05f) {
		auto width = float(image.getWidth());
		auto height = float(image.getHeight());

		amount *= jmax(width, height);
		auto amountPowDiv = 1.f / std::pow(amount, 2);

		for (auto x = 0.f; x < width; x += amount)
			for (auto y = 0.f; y < height; y += amount) {
				auto colourValue = 0.f;
				for(auto cX = 0.f; cX < amount; cX++)
					for (auto cY = 0.f; cY < amount; cY++)
						colourValue += image.getPixelAt(int(x + cX), int(y + cY)).getARGB();
				colourValue *= amountPowDiv;
				Colour meanColour = Colour(uint32(colourValue));
				for (auto cX = 0.f; cX < amount; cX++)
					for (auto cY = 0.f; cY < amount; cY++)
						image.setPixelAt(int(x + cX), int(y + cY), meanColour);
			}
	}

	static void rasterize(Image& image, float amount = .05f, float opacity = .5f) {
		auto width = float(image.getWidth());
		auto height = float(image.getHeight());

		amount *= jmax(width, height);

		for (auto x = 0.f; x < width; x++)
			for (auto y = 0.f; y < height; y++)
				image.setPixelAt(int(x), int(y),
					image.getPixelAt(int(x), int(y)).interpolatedWith(
						image.getPixelAt(int(int(x / amount) * amount), int(int(y / amount) * amount)), opacity));
	}

	static void repeatBlock(Image& image, float blocksX = 3.f, float blocksY = 6.f) {
		auto width = float(image.getWidth());
		auto height = float(image.getHeight());

		auto blockX = width / blocksX;
		auto blockY = height / blocksY;

		for (auto x = 0.f; x < width; x++)
			for (auto y = 0.f; y < height; y++)
				image.setPixelAt(int(x), int(y), image.getPixelAt( int(std::fmod(x, blockX)), int(std::fmod(y, blockY))));
	}

	static void rgbSplitRadialRandom(Image& image, Random random, float randomization = 1.f, float redX = 10.f, float redY = 10.f,
		float greenX = 5.f, float greenY = 5.f, float blueX = 0.f, float blueY = 0.f, float opacity = 1.f) {
		Image newImage = Image(Image::PixelFormat::RGB, image.getWidth(), image.getHeight(), true);
		auto width = float(image.getWidth());
		auto height = float(image.getHeight());
		float offX, offY;

		for (auto x = 0.f; x < width; x++)
			for (auto y = 0.f; y < height; y++) {
				auto stretchX = -2.f * (x / width) + 1.f;
				auto stretchY = -2.f * (y / height) + 1.f;
				offX = x + stretchX * redX + stretchX * redX * random.nextFloat() * randomization;
				offY = y + stretchY * redY + stretchX * redY * random.nextFloat() * randomization;
				auto red = image.getPixelAt(int(offX), int(offY)).getRed();
				offX = x + stretchX * greenX + stretchX * greenX * random.nextFloat() * randomization;
				offY = y + stretchY * greenY + stretchX * greenY * random.nextFloat() * randomization;
				auto green = image.getPixelAt(int(offX), int(offY)).getGreen();
				offX = x + stretchX * blueX + stretchX * blueX * random.nextFloat() * randomization;
				offY = y + stretchY * blueY + stretchX * blueY * random.nextFloat() * randomization;
				auto blue = image.getPixelAt(int(offX), int(offY)).getBlue();

				newImage.setPixelAt(int(x), int(y), image.getPixelAt(int(x), int(y)).interpolatedWith(Colour(red, green, blue), opacity));
			}

		image = newImage;
	}

	static void rgbSplitRadial(Image& image, float redX = 10.f, float redY = 10.f,
		float greenX = 5.f, float greenY = 5.f, float blueX = 0.f, float blueY = 0.f, float opacity = 1.f) {
		Rectangle<int> bounds = image.getBounds();
		Image newImage = Image(Image::PixelFormat::RGB, bounds.getWidth(), bounds.getHeight(), true);
		auto width = float(image.getWidth());
		auto height = float(image.getHeight());
		float offX, offY;

		for (auto x = 0.f; x < width; x++)
			for (auto y = 0.f; y < height; y++) {
				auto stretchX = -2.f * (x / width) + 1.f;
				auto stretchY = -2.f * (y / height) + 1.f;
				offX = x + stretchX * redX;
				offY = y + stretchY * redY;
				auto red = image.getPixelAt(int(offX), int(offY)).getRed();
				offX = x + stretchX * greenX;
				offY = y + stretchY * greenY;
				auto green = image.getPixelAt(int(offX), int(offY)).getGreen();
				offX = x + stretchX * blueX;
				offY = y + stretchY * blueY;
				auto blue = image.getPixelAt(int(offX), int(offY)).getBlue();

				newImage.setPixelAt(int(x), int(y), image.getPixelAt(int(x), int(y)).interpolatedWith(Colour(red, green, blue), opacity));
			}

		image = newImage;
	}

	static void posterize(Image& i, float amount = .1f, float opacity = .5f) {
		amount *= 255.f;
		for(auto x = 0; x < i.getWidth(); x++)
			for (auto y = 0; y < i.getHeight(); y++) {
				auto r = i.getPixelAt(x, y).getRed();
				auto g = i.getPixelAt(x, y).getGreen();
				auto b = i.getPixelAt(x, y).getBlue();
				r -= uint8(fmodf(r, amount));
				g -= uint8(fmodf(g, amount));
				b -= uint8(fmodf(b, amount));
				i.setPixelAt(x, y, i.getPixelAt(x, y).interpolatedWith(Colour(r, g, b), opacity));
			}
	}

	static void addNoise(Image& i, bool isColoured = true, float opacity = .05f) {
		Random random;
		for (auto x = 0; x < i.getWidth(); x++)
			for (auto y = 0; y < i.getHeight(); y++) {
				uint8 r, g, b;
				if (isColoured) {
					r = uint8(random.nextInt({ 0,255 }));
					g = uint8(random.nextInt({ 0,255 }));
					b = uint8(random.nextInt({ 0,255 }));
				} else
					r = b = g = uint8(random.nextInt({ 0,255 }));
				
				i.setPixelAt(x, y, i.getPixelAt(x, y).interpolatedWith(Colour(r, g, b), opacity));
			}
	}
	
	static void setRelativeSaturation(Colour& c, float amount = 1.f) {
		Array<float> rgb(3), rgbSort(3);
		rgb.set(0, float(c.getRed()));
		rgb.set(1, float(c.getGreen()));
		rgb.set(2, float(c.getBlue()));
		rgbSort = rgb;
		rgbSort.sort();
		rgb.set(rgb.indexOf(rgbSort[2]), amount * 255.f + (1.f - amount) * rgbSort[1]);

		c = Colour(uint8(rgb[0]), uint8(rgb[1]), uint8(rgb[2]));
	}

	static void setRelativeLuminance(Colour& c, float amount = 1.f) {
		Array<float> rgb(3), rgbSort(3);
		Array<int> rgbIdx(3);
		rgb.set(0, float(c.getRed()));
		rgb.set(1, float(c.getGreen()));
		rgb.set(2, float(c.getBlue()));
		rgbSort = rgb;
		rgbSort.sort();
		rgb.set(rgb.indexOf(rgbSort[2]), 255.f * amount);
		auto relativeRGB = float(rgb.indexOf(rgbSort[1])) / float(rgb.indexOf(rgbSort[2]));
		rgb.set(rgb.indexOf(rgbSort[1]), relativeRGB * 255.f * amount);
		relativeRGB = float(rgb.indexOf(rgbSort[0])) / float(rgb.indexOf(rgbSort[2]));
		rgb.set(rgb.indexOf(rgbSort[0]), relativeRGB * 255.f * amount);

		c = Colour(uint8(rgb[0]), uint8(rgb[1]), uint8(rgb[2]));
	}

	static Colour getMeanColour(Image& i) {
		int r, g, b;
		r = i.getPixelAt(0, 0).getRed();
		g = i.getPixelAt(0, 0).getGreen();
		b = i.getPixelAt(0, 0).getBlue();
		for (auto x = 1; x < i.getWidth(); x++)
			for (auto y = 0; y < i.getHeight(); y++) {
				r += i.getPixelAt(x, y).getRed();
				g += i.getPixelAt(x, y).getGreen();
				b += i.getPixelAt(x, y).getBlue();
			}
		float amountOfPixels = float(i.getWidth() * i.getHeight());
		r = int(float(r) / amountOfPixels);
		g = int(float(g) / amountOfPixels);
		b = int(float(b) / amountOfPixels);
		return Colour(uint8(r), uint8(g), uint8(b));
	}

	static float getRelativeLuminance(Colour c) {
		auto r = c.getRed();
		auto g = c.getGreen();
		auto b = c.getBlue();

		float luminance = jmax(jmax(r, g), b) / 255.f;

		return luminance;
	}

	static float getRelativeSaturation(Colour c) {
		Array<float> rgb(3);
		rgb.set(0, float(c.getRed()));
		rgb.set(1, float(c.getGreen()));
		rgb.set(2, float(c.getBlue()));
		auto biggest = 0.f;

		for (auto i = 0; i < 3; i++)
			if (biggest < rgb[i])
				biggest = rgb[i];

		auto smallest = biggest;

		for (auto i = 0; i < 3; i++)
			if (smallest > rgb[i])
				smallest = rgb[i];

		float normalizationFactor = biggest / 255.f;

		smallest /= normalizationFactor;
		biggest = 255.f;

		return abs(biggest - smallest) / 255.f;
	}
private:

};

///////////////////////////////////////// test area for new HDLImagePPHelpfer-class:


class HDLImagePPHelper {
public:
	enum VignetteModes {
		jminMode,
		addMode,
		multiplyMode
	};

	void rgbSplitRadial(float redX = 10.f, float redY = 10.f, float greenX = 5.f, float greenY = 5.f,
		float blueX = 0.f, float blueY = 0.f, float opacity = 1.f) {

		float offX, offY;
		auto stretchX = -2.f * (x / widthF) + 1.f;
		auto stretchY = -2.f * (y / heightF) + 1.f;
		offX = xF + stretchX * redX;
		offY = yF + stretchY * redY;
		auto red = image.getPixelAt(int(offX), int(offY)).getRed();
		offX = xF + stretchX * greenX;
		offY = yF + stretchY * greenY;
		auto green = image.getPixelAt(int(offX), int(offY)).getGreen();
		offX = xF + stretchX * blueX;
		offY = yF + stretchY * blueY;
		auto blue = image.getPixelAt(int(offX), int(offY)).getBlue();

		newImage.setPixelAt(x, y, image.getPixelAt(x, y)
			.interpolatedWith(Colour(red, green, blue), opacity).withAlpha(alpha));
	}

	void posterize(float amount = .1f, float opacity = 1.f) {
		amount *= 255.f;
		auto r = float(image.getPixelAt(x, y).getRed());
		auto g = float(image.getPixelAt(x, y).getGreen());
		auto b = float(image.getPixelAt(x, y).getBlue());
		r -= fmodf(r, amount);
		g -= fmodf(g, amount);
		b -= fmodf(b, amount);
		newImage.setPixelAt(x, y, image.getPixelAt(x, y)
			.interpolatedWith(Colour(uint8(r), uint8(g), uint8(b)).withAlpha(alpha), opacity));
	}

	void addNoise(float amount = .1f, bool isColoured = true) {
		if(isColoured)
			newImage.setPixelAt(x, y, image.getPixelAt(x,y)
				.interpolatedWith(Colour(random.nextInt()).withAlpha(alpha), amount));
		else {
			auto rgb = uint8(random.nextFloat() * 255.f);
			newImage.setPixelAt(x, y, image.getPixelAt(x, y)
				.interpolatedWith(Colour(rgb, rgb, rgb).withAlpha(alpha), amount));
		}
	}

	void alphaGate(float alphaThreshold, Colour bgColour = Colours::black) {
		if (alpha < alphaThreshold)
			newImage.setPixelAt(x, y, image.getPixelAt(x, y));
		else
			newImage.setPixelAt(x, y,
				bgColour.interpolatedWith(image.getPixelAt(x, y).withAlpha(1.f), alpha));
		image.setPixelAt(x, y, newImage.getPixelAt(x, y));
	}

	void alphaVignette(int mode = multiplyMode, float shape = .75f) {
		auto relativeX = xF / (widthF - 1.f);
		auto relativeY = yF / (heightF - 1.f);
		auto relMidX = relativeX < .5f ?
			2.f * relativeX : 2.f - 2.f * relativeX;
		auto relMidY = relativeY < .5f ?
			2.f * relativeY : 2.f - 2.f * relativeY;
		float relMid;
		switch (mode) {
		case jminMode:
			relMid = jmin(relMidX, relMidY);
			break;
		case addMode:
			relMid = .5f * (relMidX + relMidY);
			break;
		case multiplyMode:
			relMid = relMidX * relMidY;
			break;
		}
		auto shapeMod = std::fmod(2.f * shape, 1.f);
		float skewRelMid;
		if (shape < .5f) {
			auto curved = std::pow(relMid, 4);
			skewRelMid = curved + shapeMod * (relMid - curved);
		}
		else {
			auto curved = std::tanh(4.f * relMid);
			skewRelMid = relMid + shapeMod * (curved - relMid);
		}
		
		alpha *= skewRelMid;
		image.setPixelAt(x, y, image.getPixelAt(x, y)
			.withAlpha(alpha));
		newImage.setPixelAt(x, y, image.getPixelAt(x, y));
	}

	virtual void addFX() = 0;

	void process(Image& i) {
		image = i;
		newImage = Image(Image::ARGB, image.getWidth(), image.getHeight(), true);
		widthF = float(image.getWidth());
		heightF = float(image.getHeight());
		auto uint8Divide = 1.f / 255.f;

		for (y = 0; y < image.getHeight(); ++y)
			for (x = 0; x < image.getWidth(); ++x){
				xF = float(x);
				yF = float(y);
				alpha = image.getPixelAt(x, y).getAlpha() * uint8Divide;
				addFX();
			}

		i = newImage;
	}

	int x, y;
	float xF, yF, alpha, widthF, heightF;
private:
	Image newImage, image;
	Random random;
};

// test:
class ImagePP : public HDLImagePPHelper {
public:
	void addFX() override {
		posterize(.05f,.5f);
		rgbSplitRadial(10.f, 10.f, -10.f, -10.f, 0.f, 0.f, .5f);
		addNoise(.5f);
	}
};