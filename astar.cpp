#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include "bitmap_image.hpp"

int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize);

struct node 
{
	int index;
	int fCost;
	int hCost;
	int parent;
};

struct mapinfo
{
    mapinfo(int r, int g, int b, int x, int y) { R = r; G = g; B = b; X = x; Y = y; };
    int R, G, B, X, Y;
};

struct position
{
    position(int ix, int iy) { x = ix; y = iy; };
    int x, y;
};

bool operator <(const node& x, const node& y) 
{
	return x.fCost > y.fCost;
}

void readBMP(char* filename);
int getX(int index);
int getY(int index);
int getIndex(int x, int y);
int getDistance(int x, int y);
void investigate(node n);
void findShortestPath(int dist, int* pOutBuffer, const int nOutBufferSize);
void buildMap(unsigned char* map);
void createMatrix();
int FindDistance(int x1, int y1, int x2, int y2);
void insertInMatrix(int i, int j, int dist);

int mapWidth, mapHeight;
unsigned char pMap[200 * 200];
int pOutBuffer[200 * 200];

//map
std::vector<mapinfo> colorVec;
std::vector<position> entrances;
std::vector<position> campings;
std::vector<position> rngrStops;
std::vector<position> gates;
std::vector<position> generalGates;
position rngrbase(-1, -1);

int distancematrix[40][40];

//Clear
const unsigned char* map;
std::vector<int> hCosts;
std::vector<int> parents;
std::priority_queue<node> prioQueue;

int goalIndex, startIndex;
int goalX, goalY;
bool targetFound = false;

int main()
{
    mapWidth = 200;
    mapHeight = 200;
    int size = mapWidth * mapHeight;

    std::cout << "Loading bmp" << std::endl;
    readBMP("../map24.bmp");

    std::cout << "Building map" << std::endl;
    buildMap(pMap);

    std::cout << "Building matrix" << std::endl;
    createMatrix();


    int startX = entrances[0].x;
    int startY = entrances[0].y;

    int goalX = generalGates[1].x;
    int goalY = generalGates[1].y;
   
    std::cout << "Running A*" << std::endl;

    int shortestDist = FindDistance(17, 67, 110, 9);


    std::cout << "Found shortest distance: " << shortestDist << std::endl;
    std::cout << "now printing shortest path... " << std::endl;

    for (int i = 0; i < shortestDist; i++) {
        std::cout << pOutBuffer[i] << ", ";
    }

    std::cout << std::endl;
    std::cin.get();

    return 0;
}

void createMatrix()
{
    //Entrance5 -> Campings9 -> RngrStops8 -> Gates9 -> GeneralGates8 -> RangerBase1
    int totalNodes = entrances.size() + campings.size() + rngrStops.size() + gates.size() + generalGates.size() + 1;

    int iter = 0;
    for (int i = 0; i < entrances.size(); ++i)
    {
        insertInMatrix(i, iter, 0);
        iter++;

        for (int j = 0; j < campings.size(); ++j)
        {
            insertInMatrix(i, iter, FindDistance(entrances[i].x, entrances[i].y, campings[j].x, campings[j].y));
            iter++;
        }
        for (int j = 0; j < rngrStops.size(); ++j)
        {
            insertInMatrix(i, iter, FindDistance(entrances[i].x, entrances[i].y, rngrStops[j].x, rngrStops[j].y));
            iter++;
        }
        for (int j = 0; j < gates.size(); ++j)
        {
            insertInMatrix(i, iter, FindDistance(entrances[i].x, entrances[i].y, gates[j].x, gates[j].y));
            iter++;
        }
        for (int j = 0; j < generalGates.size(); ++j)
        {
            insertInMatrix(i, iter, FindDistance(entrances[i].x, entrances[i].y, generalGates[j].x, generalGates[j].y));
            iter++;
        }
        insertInMatrix(i, iter, FindDistance(entrances[i].x, entrances[i].y, rngrbase.x, rngrbase.y));
        iter++;
    }
}

void insertInMatrix(int i, int j, int dist)
{
    distancematrix[i][j] = dist;
    distancematrix[j][i] = dist;
}

int FindDistance(int x1, int y1, int x2, int y2)
{
    int dist = FindPath(x1, y1, x2, y2, pMap, mapWidth, mapHeight, pOutBuffer, 200 * 200);
    if (dist == -1)
    {
        dist = FindPath(x2, y2, x1, y1, pMap, mapWidth, mapHeight, pOutBuffer, 200 * 200);
        if (dist == -1)
        {
            std::cout << "No valid distance: " << x1 << ", " << y1 << " to " << x2 << ", " << y2 << "\n";
        }
    }
    return dist;

}

int FindPath(const int nStartX, const int nStartY,	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight, int* pOutBuffer, const int nOutBufferSize)
{
	map = pMap;

    hCosts.clear();
    parents.clear();
	hCosts.assign(nMapWidth*nMapHeight, -1);
	parents.assign(nMapWidth*nMapHeight, -1);

	std::priority_queue<node> p;
	prioQueue = p;

	goalIndex = getIndex(nTargetX, nTargetY);
	goalX = nTargetX;
	goalY = nTargetY;

	node startingNode;
	startIndex = getIndex(nStartX, nStartY);
	startingNode.index = startIndex;
	startingNode.fCost = 0;
	startingNode.hCost = 0;
	startingNode.parent = -1;

	prioQueue.push(startingNode);
	targetFound = false;

	while (!prioQueue.empty() && !targetFound) 
	{
		node n = prioQueue.top();
		prioQueue.pop();
		investigate(n);
	}

	if (hCosts[goalIndex] != -1 && hCosts[goalIndex] <= nOutBufferSize) 
	{
		findShortestPath(hCosts[goalIndex], pOutBuffer, nOutBufferSize);
		return hCosts[goalIndex];
	}
	else if (hCosts[goalIndex] > nOutBufferSize) 
	{
		return hCosts[goalIndex];
	}
	else 
	{
		return -1;
	}
}


void readBMP(char* filename)
{
    bitmap_image image(filename);

    if (!image)
    {
        printf("Error - Failed to open: %p \n", filename);
    }

    for (int i = 0; i < 200; i++)
    {
        for (int j = 0; j < 200; j++)
        {
            rgb_t colour;
            image.get_pixel(j, i, colour);
            colorVec.push_back(mapinfo(colour.red, colour.green, colour.blue, j, i));
        }
    }
}

void buildMap(unsigned char* map)
{
    for (int i = 0; i < colorVec.size(); i++)
    {
        mapinfo m = colorVec[i];

        if ((m.R == 76) && (m.G == 255) && (m.B == 0)) //Entrance Green
        {
            entrances.push_back(position(m.X, m.Y));
            map[i] = 1;
        }
        else if ((m.R == 255) && (m.G == 106) && (m.B == 0)) //Campings Orange
        {
            campings.push_back(position(m.X, m.Y));
            map[i] = 1;
        }
        else if ((m.R == 255) && (m.G == 0) && (m.B == 220)) //RangerBase Purple
        {
            rngrbase = (position(m.X, m.Y));
            map[i] = 1;
        }
        else if ((m.R == 255) && (m.G == 216) && (m.B == 0)) //RangerStops Yellow
        {
            rngrStops.push_back(position(m.X, m.Y));
            map[i] = 1;
        }
        else if ((m.R == 255) && (m.G == 0) && (m.B == 0)) //Gates Red
        {
            gates.push_back(position(m.X, m.Y));
            map[i] = 1;
        }
        else if ((m.R == 0) && (m.G == 255) && (m.B == 255)) //GeneralGates Cyan
        {
            generalGates.push_back(position(m.X, m.Y));
            map[i] = 1;
        }
        else if ((m.R == 255) && (m.G == 255) && (m.B == 255)) //Road Whithe
        {
            map[i] = 1;
        }
        else //Forest black
        {
            map[i] = 0;
        }
    }
}


int getX(int index) 
{
	return index % mapWidth;
}

int getY(int index) 
{
	return floor(index / mapWidth);
}

int getIndex(int x, int y) 
{
	return y*mapWidth + x;
}

int getDistance(int x, int y) 
{
	return std::max(x - goalX, goalX - x) + std::max(y - goalY, goalY - y);
}

void investigate(node n) 
{
	// check if the node is the target
	if (n.index == goalIndex) 
	{
		targetFound = true;
		hCosts[n.index] = n.hCost;
		parents[n.index] = n.parent;
		return;
	}

	// check wether the node was already visited
	if (hCosts[n.index] != -1) 
	{
		return;
	}

	// if this is the shortest path from the source to this node:
	int x = getX(n.index);
	int y = getY(n.index);

	// set parent and hCost
	hCosts[n.index] = n.hCost;
	parents[n.index] = n.parent;

	// visit node on the left
	if (0 <= x - 1) 
	{
		int newId = getIndex(x - 1, y);
		if (hCosts[newId] == -1 && map[newId] == 1) 
		{
			node newNode;
			newNode.index = newId;
			newNode.parent = n.index;
			newNode.hCost = n.hCost + 1;
			newNode.fCost = (n.hCost + 1) + getDistance(x - 1, y);
			prioQueue.push(newNode);
		}
	}
	// visit node on the right
	if (x + 1 < mapWidth) 
	{
		int newId = getIndex(x + 1, y);
		if (hCosts[newId] == -1 && map[newId] == 1) 
		{
			node newNode;
			newNode.index = newId;
			newNode.parent = n.index;
			newNode.hCost = n.hCost + 1;
			newNode.fCost = (n.hCost + 1) + getDistance(x + 1, y);
			prioQueue.push(newNode);
		}
	}
	// visit node below
	if (0 <= y - 1) 
	{
		int newId = getIndex(x, y - 1);
		if (hCosts[newId] == -1 && map[newId] == 1) 
		{
			node newNode;
			newNode.index = newId;
			newNode.parent = n.index;
			newNode.hCost = n.hCost + 1;
			newNode.fCost = (n.hCost + 1) + getDistance(x, y - 1);
			prioQueue.push(newNode);
		}
	}
	if (y + 1 < mapHeight) 
	{
		int newId = getIndex(x, y + 1);
		if (hCosts[newId] == -1 && map[newId] == 1) 
		{
			node newNode;
			newNode.index = newId;
			newNode.parent = n.index;
			newNode.hCost = n.hCost + 1;
			newNode.fCost = (n.hCost + 1) + getDistance(x, y + 1);
			prioQueue.push(newNode);
		}
	}
}

void findShortestPath(int dist, int* pOutBuffer, const int nOutBufferSize) 
{
	int i = 1;
	int index = goalIndex;

	while (index != startIndex) 
	{
		*(pOutBuffer + dist - i) = index;
		index = parents[index];
		++i;
	}
}