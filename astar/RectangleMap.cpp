#include "RectangleMap.h"

RectangleMap::RectangleMap(int x, int y, int w, int h)
{
	setWidth(w);
	setHeight(h);
	rX = x;
	rY = y;
}

RectangleMap::~RectangleMap()
{
}

void RectangleMap::setHeight(int h)
{
	rHeight = h;
}
void RectangleMap::setWidth(int w)
{
	rWidth = w;
}
int RectangleMap::perimeter()
{
	return (2 * rHeight + 2 * rWidth);
}//piiri
float RectangleMap::area()
{
	return rHeight * rWidth;
}
void RectangleMap::show()
{
	//cout << "Length: " << length << " Width: " << width;
}
int RectangleMap::sameArea(RectangleMap other)
{
	int areaf = rHeight * rWidth;
	int areas = other.rHeight * other.rWidth;
	if (areaf == areas)
		return 1;
	return 0;
}

int RectangleMap::left()
{
	return rX - (rWidth / 2);
}
int RectangleMap::right()
{
	return rX + (rWidth / 2);
}
int RectangleMap::top()
{
	return rY - (rHeight / 2);
}
int RectangleMap::bottom()
{
	return rY + (rHeight / 2);
}