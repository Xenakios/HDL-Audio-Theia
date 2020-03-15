#pragma once

class HDLGrid {
public:
	void setGrid(float x, float y, Rectangle<int>& localBounds) {
		bounds = localBounds.toFloat();
		blocksX = x;
		blocksY = y;
		blockWidth = bounds.getWidth() / blocksX;
		blockHeight = bounds.getHeight() / blocksY;
	}

	float getBlocksX() { return blocksX; }
	float getBlocksY() { return blocksY; }

	void draw(Graphics& g) {
		for (auto i = 1; i < blocksX; ++i) {
			auto curX = i * blockWidth;
			g.drawLine(Line<float>(curX, 0, curX, bounds.getHeight()));
		}
		for (auto i = 1; i < blocksY; ++i) {
			auto curY = i * blockHeight;
			g.drawLine(Line<float>(0, curY, bounds.getWidth(), curY));
		}
	}

	void draw(Graphics& g, Colour c) {
		g.setColour(c);
		for (auto i = 1; i < blocksX; ++i) {
			auto curX = i * blockWidth;
			g.drawLine(Line<float>(curX, 0, curX, bounds.getHeight()));
		}
		for (auto i = 1; i < blocksY; ++i) {
			auto curY = i * blockHeight;
			g.drawLine(Line<float>(0, curY, bounds.getWidth(), curY));
		}
	}

	template<class Type>
	Path getPath(std::vector<Type> way) {
		Path p;
		p.startNewSubPath(getPoint(way[0].getX(), way[0].getY()));
		for (int i = 1; i < way.size(); ++i)
			p.lineTo(getPoint(way[i].getX(), way[i].getY()));
		p.closeSubPath();
		return p;
	}

	template<class Type>
	Rectangle<Type> getRect(Type x, Type y, Type xB, Type yB){
		return Rectangle<Type>((Type)(x * blockWidth), (Type)(y * blockHeight),
			(Type)(xB * blockWidth), (Type)(yB * blockHeight));
	}

	template<class Type>
	Point<Type> getPoint(Type x, Type y) {
		return Point<Type>((Type)(x * blockWidth), (Type)(y * blockHeight));
	}

	template<class Type>
	Point<Type> getLine(Type x, Type y, Type xB, Type yB) {
		return Line<Type>((Type)(x * blockWidth), (Type)(y * blockHeight),
			(Type)(xB * blockWidth), (Type)(yB * blockHeight));
	}

private:
	float blocksX, blocksY, blockWidth, blockHeight;
	Rectangle<float> bounds;
};