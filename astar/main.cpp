#include <iostream>
#include <math.h>
#include <fstream>
#include <queue>
#include <sstream>
#include "Leaf.h"
//random
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <time.h>       /* time */
#include <random>
//color
#include <windows.h>


//A-star variables
const int n = 100; // horizontal size of the map
const int m = 100; // vertical size size of the map
char levelMap[n][m];

static int map[n][m];
static int pictureMap[n][m];
static int closed_nodes_map[n][m]; // map of closed (tried-out) nodes
static int open_nodes_map[n][m]; // map of open (not-yet-tried) nodes
static int dir_map[n][m]; // map of directions

const int dir = 8; // number of possible directions to go at any position
				   // if dir==4
				   //static int dx[dir]={1, 0, -1, 0};
				   //static int dy[dir]={0, 1, 0, -1};
				   // if dir==8
static int dx[dir] = { 1, 1, 0, -1, -1, -1, 0, 1 };
static int dy[dir] = { 0, 1, 1, 1, 0, -1, -1, -1 };

//Default coordinates
int startX = 1, startY = 1, endX = n - 2, endY = m - 2;

static std::random_device rd; // random device engine, usually based on /dev/random on UNIX-like systems
							  // initialize Mersennes' twister using rd to generate the seed
static std::mt19937 rng(rd());

int dice(int num)
{
	static std::uniform_int_distribution<int> uid(1, num - 2); // random dice
	return uid(rng); // use rng as a generator
}

class node
{
	// current position
	int xPos;
	int yPos;
	// total distance already travelled to reach the node
	int level;
	// priority=level+remaining distance estimate
	int priority;  // smaller: higher priority

public:
	node(int xp, int yp, int d, int p)
	{
		xPos = xp; yPos = yp; level = d; priority = p;
	}

	int getxPos() const { return xPos; }
	int getyPos() const { return yPos; }
	int getLevel() const { return level; }
	int getPriority() const { return priority; }

	void updatePriority(const int & xDest, const int & yDest)
	{
		priority = level + estimate(xDest, yDest) * 10; //A*
	}

	// give better priority to going strait instead of diagonally
	void nextLevel(const int & i) // i: direction
	{
		level += (dir == 8 ? (i % 2 == 0 ? 10 : 14) : 10);
	}

	// Estimation function for the remaining distance to the goal.
	const int & estimate(const int & xDest, const int & yDest) const
	{
		static int xd, yd, d;
		xd = xDest - xPos;
		yd = yDest - yPos;

		// Euclidian Distance
		d = static_cast<int>(sqrt(xd*xd + yd*yd));

		// Manhattan distance
		//d=abs(xd)+abs(yd);

		// Chebyshev distance
		//d=max(abs(xd), abs(yd));

		return(d);
	}
};

bool operator<(const node & a, const node & b)
{
	return a.getPriority() > b.getPriority();
}

std::string pathFind(const std::string& FileName, const int & xStart, const int & yStart,
	const int & xFinish, const int & yFinish)
{
	std::string line;										//current line 
	char type;											//current character type on the map
	std::ifstream inputFile(FileName.c_str());				//read the given .txt file

	//write the values for the algorithm
	if (inputFile)										//if reading the inputted file
	{
		std::cout << "Input file success" << std::endl;
		for (unsigned int y = 0; y < n; ++y)	// Loops through the y-coordinate
		{
			std::getline(inputFile, line);
			for (unsigned int x = 0; x < m; ++x)	// Loops through the x-coordinate
			{
				type = line.at(x);
				if (type == 35)
				{
					map[x][y] = 1; //   # symbol = value 1
				}
				if (type == 46)
				{	
					map[x][y] = 0; //   . symbol = value 0
				}
							
			}
			line.clear();								//erases the contents of the line
		}
		inputFile.close();
	}

	//check if either point is surrounded
	if (map[startX+1][startY  ] == 1 && 
		map[startX+1][startY+1] == 1 && 
		map[startX  ][startY+1] == 1 && 
		map[startX-1][startY+1] == 1 && 
		map[startX-1][startY  ] == 1 && 
		map[startX-1][startY-1] == 1 && 
		map[startX  ][startY-1] == 1 && 
		map[startX+1][startY-1] == 1)
	{
		std::cout << "Start Point surrounded by walls\n";
		return "";
	}
	if (map[endX + 1][endY	  ] == 1 && 
		map[endX + 1][endY + 1] == 1 &&
		map[endX	][endY + 1] == 1 &&
		map[endX - 1][endY + 1] == 1 &&
		map[endX - 1][endY	  ] == 1 &&
		map[endX - 1][endY - 1] == 1 &&
		map[endX	][endY - 1] == 1 &&
		map[endX + 1][endY - 1] == 1)
	{
		std::cout << "Finish Point surrounded by walls\n";
		return "";
	}

	static std::priority_queue<node> pq[2]; // list of open (not-yet-tried) nodes
	static int pqi; // pq index
	static node* n0;
	static node* m0;
	static int i, j, x, y, xdx, ydy;
	static char c;
	pqi = 0;

	// reset the node maps
	for (y = 0; y<m; y++)
	{
		for (x = 0; x<n; x++)
		{
			closed_nodes_map[x][y] = 0;
			open_nodes_map[x][y] = 0;
		}
	}

	// create the start node and push into list of open nodes
	n0 = new node(xStart, yStart, 0, 0);
	n0->updatePriority(xFinish, yFinish);
	pq[pqi].push(*n0);

	//open_nodes_map[x][y] = n0->getPriority(); // mark it on the open nodes map
	open_nodes_map[xStart][yStart] = n0->getPriority();

	//a* search
	delete n0;
	while (!pq[pqi].empty())
	{
		// get the current node w/ the highest priority
		// from the list of open nodes
		n0 = new node(pq[pqi].top().getxPos(), pq[pqi].top().getyPos(),
			pq[pqi].top().getLevel(), pq[pqi].top().getPriority());

		x = n0->getxPos(); y = n0->getyPos();

		pq[pqi].pop(); // remove the node from the open list
		open_nodes_map[x][y] = 0;
		// mark it on the closed nodes map
		closed_nodes_map[x][y] = 1;

		// quit searching when the goal state is reached
		//if((*n0).estimate(xFinish, yFinish) == 0)
		if (x == xFinish && y == yFinish)
		{
			//std::cout << x << y << std::endl;
			// generate the path from finish to start
			// by following the directions
			std::string path = "";
			while (!(x == xStart && y == yStart))
			{
				j = dir_map[x][y];
				c = '0' + (j + dir / 2) % dir;
				path = c + path;
				x += dx[j];
				y += dy[j];
			}

			// garbage collection
			delete n0;
			// empty the leftover nodes
			while (!pq[pqi].empty()) pq[pqi].pop();
			return path;
		}

		// generate moves (child nodes) in all possible directions
		for (i = 0; i<dir; i++)
		{
			xdx = x + dx[i]; ydy = y + dy[i];

			if (!(xdx<0 || xdx>n - 1 || ydy<0 || ydy>m - 1 || map[xdx][ydy] == 1
				|| closed_nodes_map[xdx][ydy] == 1))
			{
				// generate a child node
				m0 = new node(xdx, ydy, n0->getLevel(),
					n0->getPriority());
				m0->nextLevel(i);
				m0->updatePriority(xFinish, yFinish);

				// if it is not in the open list then add into that
				if (open_nodes_map[xdx][ydy] == 0)
				{
					open_nodes_map[xdx][ydy] = m0->getPriority();
					pq[pqi].push(*m0);
					// mark its parent node direction
					delete m0;
					dir_map[xdx][ydy] = (i + dir / 2) % dir;

				}
				else if (open_nodes_map[xdx][ydy]>m0->getPriority())
				{
					// update the priority info
					open_nodes_map[xdx][ydy] = m0->getPriority();
					// update the parent direction info
					dir_map[xdx][ydy] = (i + dir / 2) % dir;
					// replace the node
					// by emptying one pq to the other one
					// except the node to be replaced will be ignored
					// and the new node will be pushed in instead
					while (!(pq[pqi].top().getxPos() == xdx &&
						pq[pqi].top().getyPos() == ydy))
					{
						pq[1 - pqi].push(pq[pqi].top());
						pq[pqi].pop();
					}
					pq[pqi].pop(); // remove the wanted node

								   // empty the larger size pq to the smaller one
					if (pq[pqi].size()>pq[1 - pqi].size()) pqi = 1 - pqi;
					while (!pq[pqi].empty())
					{
						pq[1 - pqi].push(pq[pqi].top());
						pq[pqi].pop();
					}
					pqi = 1 - pqi;
					pq[pqi].push(*m0); // add the better node instead
				}
				else delete m0; // garbage collection
			}
		}
		delete n0; // garbage collection
	}
	return ""; // no route found
}

void fixMap()
{
	//surround map with walls
	for (int i = 0; i < m; i++) //y
	{
		if (levelMap[i][m-1] == 46)
			levelMap[i][m-1] = 35; //creates ASCII wall symbol
		if (levelMap[i][m-1] == 46)
			levelMap[i][m-1] = 35; //creates ASCII wall symbol
		if (levelMap[0][i] == 46)
			levelMap[0][i] = 35; //creates ASCII wall symbol
		if (levelMap[n-1][i] == 46)
			levelMap[n-1][i] = 35; //creates ASCII wall symbol
	}

	//if start or finish is a wall
	if (levelMap[startX][startY] == 35)
	{
		std::cout << "Start fixed ";
		levelMap[startX][startY] = 46;
	}
	if (levelMap[endX][endY] == 35)
	{
		std::cout << "Finish fixed ";
		levelMap[endX][endY] = 46;
	}
	else
	{
		std::cout << "Tiles ok" << std::endl;
	}

}


void drawDungeon(unsigned int seed)
{
	srand(seed);

	int MAX_LEAF_SIZE = 30;
	Leaf* root = new Leaf(0, 0, n, m);
	std::list<Leaf> leaf_edge_nodes;
	std::list<RectangleMap> halls;
	root->generate(MAX_LEAF_SIZE);

	root->createRooms(&leaf_edge_nodes, &halls);

	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			levelMap[i][j] = 35; //creates ASCII wall symbol
		}
	}

	for (std::list<Leaf>::iterator l = leaf_edge_nodes.begin(); l != leaf_edge_nodes.end(); ++l) {

		RectangleMap* room = l->getRoom();
		int left = room->left();
		int right = room->right();
		int top = room->top();
		int bottom = room->bottom();

		if (left < 1) left = 1;
		if (right < 1) right = 1;
		if (top < 1) top = 1;
		if (bottom < 1) bottom = 1;

		if (right - left > 3 && bottom - top > 3) {

			for (int i = left; i <= right; i++) {
				for (int j = top; j <= bottom; j++)
				{
					levelMap[i][j] = 46;
				}
			}
		}
	}

	for (std::list<RectangleMap>::iterator hh = halls.begin(); hh != halls.end(); ++hh)
	{
		int left = hh->left();
		int right = hh->right();
		int top = hh->top();
		int bottom = hh->bottom();

		if (left < 1) left = 1;
		if (right < 1) right = 1;
		if (top < 1) top = 1;
		if (bottom < 1) bottom = 1;

		if (left > m - 1) left = m - 2;
		if (right > m - 1) right = m - 2;
		if (top > m - 1) top = m - 2;
		if (bottom > m - 1) bottom = m - 2;

		for (int i = left; i <= right; i++) {
			for (int j = top; j <= bottom; j++) {
				levelMap[i][j] = 46;
			}
		}
	}

	fixMap();

	std::cout << "\n=== DUNGEON GENERATOR v0.2 ===\n\n";
	std::ofstream myfile("Level1.txt");

	for (int j = 0; j < m; j++)
	{
		for (int i = 0; i < n; i++)
		{
			//print the level data to a .txt file
			if (myfile.is_open())
			{
				myfile << levelMap[i][j];
				if (i == n-1)
				{
					//divide the map into 99 character lengths
					myfile << "\n";
				}
			}
			else std::cout << "Unable to open file";
		}
	}
	myfile.close();
}


void PathfindStart()
{
	//randomize positions
	startX = dice(n), startY = dice(m), endX = dice(n), endY = dice(m);
	std::cout << "\n\n\n";
	std::cout << "Start Position: "<<startX << " " << startY << std::endl;
	std::cout << "End Position: " << endX << " " << endY << std::endl;

	drawDungeon(time(NULL));
	//drawDungeon(0);

	std::string route = pathFind("Level1.txt", startX, startY, endX, endY);
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	if (route == "")
	{
		SetConsoleTextAttribute(h, FOREGROUND_RED); //RED TEXT

		int sx = startX;
		int sy = startY;
		map[sx][sy] = 2;

		int ex = endX;
		int ey = endY;
		map[ex][ey] = 4;

		for (int y = 0; y<m; y++)
		{
			for (int x = 0; x < n; x++)
			{
				if (map[x][y] == 0)
				{
					std::cout << " ";
				}
				else if (map[x][y] == 1)
					std::cout << "#"; //obstacle
				else if (map[x][y] == 2)
				{
					SetConsoleTextAttribute(h, FOREGROUND_GREEN);
					std::cout << "S"; //start
					SetConsoleTextAttribute(h, FOREGROUND_RED); //RED TEXT
				}
				else if (map[x][y] == 4)
				{
					SetConsoleTextAttribute(h, FOREGROUND_GREEN);
					std::cout << "F"; //finish
					SetConsoleTextAttribute(h, FOREGROUND_RED); //RED TEXT
				}
			}
			std::cout << std::endl;
		}
		std::cout << "An empty route generated!" << std::endl;
		
	}
	std::cout << route << std::endl << std::endl;

	// follow the route on the map and display it 
	if (route.length()>0)
	{
		int j; char c;
		int x = startX;
		int y = startY;
		map[x][y] = 2;
		for (unsigned int i = 0; i<route.length(); i++)
		{
			c = route.at(i);
			j = atoi(&c);
			x = x + dx[j];
			y = y + dy[j];
			map[x][y] = 3;
		}
		//std::cout << x << y << std::endl;
		map[x][y] = 4;

		// display the map with the route
		for (int y = 0; y<m; y++)
		{
			SetConsoleTextAttribute(h, FOREGROUND_RED);
			for (int x = 0; x<n; x++)
				if (map[x][y] == 0)
				{
					std::cout << " ";
				}
				else if (map[x][y] == 1)
					std::cout << "#"; //obstacle
				else if (map[x][y] == 2)
					std::cout << "S"; //start
				else if (map[x][y] == 3)
				{
					SetConsoleTextAttribute(h, FOREGROUND_GREEN);
					std::cout << "O"; //route
					SetConsoleTextAttribute(h, FOREGROUND_RED);
				}
				else if (map[x][y] == 4)
					std::cout << "F"; //finish
			std::cout << std::endl;
		}
	}
	getchar(); // wait for a (Enter) keypress 
	PathfindStart();
}


int main() {

	PathfindStart();
	return(0);
}