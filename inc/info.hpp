#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

struct Coords {
    int x, y;
};

struct ChipInfo {
    int lowerLeftX, lowerLeftY, width, height;
    vector<pair<int, Coords>> bumps; // <bumpIdx, <X, Y>>
};

struct GCell {
    int x, y;
    int leftEdgeCapacity, bottomEdgeCapacity;
    double layer1Cost;
    double layer2Cost;
};

struct netInfo {
    int netID;
    Coords source, sink;
    vector<pair<Coords, Coords>> path; // <GCellX, GCellY>
};

struct GridMap {
    double OverflowPenalty;
    int ArealowerLeftX, ArealowerLeftY, Areawidth, Areaheight;
    int Gcellwidth, Gcellheight;
    int GcellNumX, GcellNumY;
    ChipInfo chip1, chip2;
    vector<netInfo> nets;
    map<pair<int, int>, GCell> gCells;
};

struct CostSettings {
    double alpha, beta, gamma, delta;
    double viaCost;
};