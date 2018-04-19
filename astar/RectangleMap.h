#pragma once
#include <iostream>
#include <stdlib.h>

class RectangleMap
{
public:
	RectangleMap(int x, int y, int width, int height);
	void setHeight(int height);
	void setWidth(int width);
	int perimeter();		//piiri
	float area();
	void show();
	int sameArea(RectangleMap);

	int left();
	int right();
	int top();
	int bottom();
	~RectangleMap();
private:
	int rX, rY, rWidth, rHeight;
};
