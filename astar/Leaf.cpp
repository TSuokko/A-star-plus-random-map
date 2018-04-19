#include "Leaf.h"
#include "Point.h"


Leaf::Leaf(int X, int Y, int Width, int Height) {
	init(X, Y, Width, Height);
}

void Leaf::init(int X, int Y, int Width, int Height) {
	_x = X;
	_y = Y;
	_width = Width;
	_height = Height;
}

void Leaf::generate(int maxLeafSize) {
	this->MAX_LEAF_SIZE = maxLeafSize;
	this->DoGeneration();
}

void Leaf::DoGeneration() {
	if (NULL == this->leftChild && NULL == this->rightChild) {
		if (this->_width > MAX_LEAF_SIZE || this->_height > MAX_LEAF_SIZE || randTrue(75)) {
			if (Split()) {
				this->leftChild->DoGeneration();
				this->rightChild->DoGeneration();
			}
			else {
				// no more splitting possible on this branch so return
				return;
			}
		}
	}
}

bool Leaf::Split() {
	if (leftChild != NULL && rightChild != NULL) {
		return false; // we are already split! abort
	}

	// determine the direction of the split
	// if the width is >25% larger than height, we split vertically
	// if the height is >25% larger than the width, we split horizontally
	// otherwise we split randomly

	bool splitH = (rand() % 10 + 1) > 5;
	if (_width > _height && _height / _width >= 0.05) {
		splitH = false;
	}
	else if (_height > _width && _width / _height >= 0.05) {
		splitH = true;
	}

	int max = (splitH ? _height : _width) - MIN_LEAF_SIZE; // determine the maximum height or width
	if (max <= MIN_LEAF_SIZE) {
		return false; // the area is too small to split any more...
	}

	int split = (rand() % max + MIN_LEAF_SIZE);

	// create our left and right children based on the direction of the split
	if (splitH) {
		this->leftChild = new Leaf(_x, _y, _width, split);
		this->rightChild = new Leaf(_x, _y + split, _width, _height - split);
	}
	else {
		this->leftChild = new Leaf(_x, _y, split, _height);
		this->rightChild = new Leaf(_x + split, _y, _width - split, _height);
	}
	return true;
}

void Leaf::createRooms(std::list<Leaf>* leaf_edge_nodes, std::list<RectangleMap>* halls) {
	if ((NULL != leftChild) || (NULL != rightChild)) {
		if (NULL != leftChild) {
			leftChild->createRooms(leaf_edge_nodes, halls);
		}
		if (NULL != rightChild) {
			rightChild->createRooms(leaf_edge_nodes, halls);
		}

		if ((NULL != leftChild) && (NULL != rightChild)) {
			createHall(halls, leftChild->getRoom(), rightChild->getRoom());
		}

	}
	else {
		// make a room
		Point* roomSize;
		Point* roomPosition;

		// the room can be between 3x3 tiles to the size of the leaf -2
		roomSize = new Point(randomInclusiveBetween(_width - 2, 3), randomInclusiveBetween(3, _height - 2));
		// place the room within the Leaf, but don't put it right agains the side of the leaf as it would merge then
		roomPosition = new Point(randomInclusiveBetween(1, _width - roomSize->first - 1), randomInclusiveBetween(1, _height - roomSize->second - 1));
		this->room = new RectangleMap(_x + roomPosition->first, _y + roomPosition->second, roomSize->first, roomSize->second);
		leaf_edge_nodes->push_back(*this);
	}
}

int Leaf::randomInclusiveBetween(int a, int b) {

	return ((rand() % (b == 0 ? 1 : b)) + a);
}

RectangleMap* Leaf::getRoom() {
	if (NULL != room) {
		return room;
	}
	else {
		RectangleMap* lRoom = NULL;
		RectangleMap* rRoom = NULL;
		if (NULL != leftChild) {
			lRoom = leftChild->getRoom();
		}
		if (NULL != rightChild) {
			rRoom = rightChild->getRoom();
		}
		if ((NULL == lRoom) && (NULL == rRoom)) {
			return NULL;
		}
		else if (NULL == rRoom) {
			return lRoom;
		}
		else if (NULL == lRoom) {
			return rRoom;
		}
		else if (randomInclusiveBetween(1, 10) > 5) {
			return lRoom;
		}
		else {
			return rRoom;
		}
	}
}

void Leaf::createHall(std::list<RectangleMap>* halls, RectangleMap* l, RectangleMap* r) {
	// now we connect these two rooms together with hallways.
	// this looks pretty complicated, but it's just trying to figure out which point is where and then either draw a straight line, or a pair of lines to make a right-angle to connect them.
	// you could do some extra logic to make your halls more bendy, or do some more advanced things if you wanted.
	//list<Rectangle> halls; 

	Point* p1 = new Point(randIBetween(l->left() + 1, l->right() - 2), randIBetween(l->top() + 1, l->bottom() - 2));
	Point* p2 = new Point(randIBetween(r->left() + 1, r->right() - 2), randIBetween(r->top() + 1, r->bottom() - 2));

	int w = p2->first - p1->first;
	int h = p2->second - p1->second;

	if (w < 0) {
		if (h < 0) {
			if (randTrue(50)) {
				halls->push_back(*new RectangleMap(p2->first, p1->second, abs(w), 1));
				halls->push_back(*new RectangleMap(p2->first, p2->second, 1, abs(h)));
			}
			else {
				halls->push_back(*new RectangleMap(p2->first, p2->second, abs(w), 1));
				halls->push_back(*new RectangleMap(p1->first, p2->second, 1, abs(h)));
			}
		}
		else if (h > 0) {
			if (randTrue(50)) {
				halls->push_back(*new RectangleMap(p2->first, p1->second, abs(w), 1));
				halls->push_back(*new RectangleMap(p2->first, p1->second, 1, abs(h)));
			}
			else {
				halls->push_back(*new RectangleMap(p2->first, p2->second, abs(w), 1));
				halls->push_back(*new RectangleMap(p1->first, p1->second, 1, abs(h)));
			}
		}
		else {
			halls->push_back(*new RectangleMap(p2->first, p2->second, abs(w), 1));
		}
	}
	else if (w > 0) {
		if (h < 0) {
			if (randTrue(50)) {
				halls->push_back(*new RectangleMap(p1->first, p2->second, abs(w), 1));
				halls->push_back(*new RectangleMap(p1->first, p2->second, 1, abs(h)));
			}
			else {
				halls->push_back(*new RectangleMap(p1->first, p1->second, abs(w), 1));
				halls->push_back(*new RectangleMap(p2->first, p2->second, 1, abs(h)));
			}
		}
		else if (h > 0) {
			if (randTrue(50)) {
				halls->push_back(*new RectangleMap(p1->first, p1->second, abs(w), 1));
				halls->push_back(*new RectangleMap(p2->first, p1->second, 1, abs(h)));
			}
			else {
				halls->push_back(*new RectangleMap(p1->first, p2->second, abs(w), 1));
				halls->push_back(*new RectangleMap(p1->first, p1->second, 1, abs(h)));
			}
		}
		else {
			halls->push_back(*new RectangleMap(p1->first, p1->second, abs(w), 1));
		}
	}
	else {
		if (h < 0) {
			halls->push_back(*new RectangleMap(p2->first, p2->second, 1, abs(h)));
		}
		else if (h > 0) {
			halls->push_back(*new RectangleMap(p1->first, p1->second, 1, abs(h)));
		}
	}

}

int Leaf::randIBetween(int a, int b) {
	return ((rand() % (b == 0 ? 1 : b)) + a);
}

bool Leaf::randTrue(int percentage) {
	return (percentage > randIBetween(1, 100));
}
