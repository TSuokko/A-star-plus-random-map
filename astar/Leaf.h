#pragma once
#include "RectangleMap.h"
#include <vector>
#include <list>

class Leaf
{
public:
	Leaf(int x, int y, int w, int h);

	void init(int x, int y, int width, int height);
	bool Split();

	void createRooms(std::list<Leaf>* leaf_edge_nodes, std::list<RectangleMap>* halls);
	RectangleMap* getRoom();


	int randIBetween(int a, int b);
	bool randTrue(int percentage);
	void createHall(std::list<RectangleMap>* halls, RectangleMap* l, RectangleMap* r);

	void generate(int maxLeafSize);

	int _x, _y, _width, _height;
	Leaf* rightChild;
	Leaf* leftChild;
	RectangleMap* room;
	std::list<RectangleMap*> halls;

private:
	const int MIN_LEAF_SIZE = 6;
	int MAX_LEAF_SIZE = 10;
	int randomInclusiveBetween(int a, int b);
	void DoGeneration();




};

