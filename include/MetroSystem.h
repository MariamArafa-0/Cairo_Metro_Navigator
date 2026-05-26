#pragma once
#include "MetroUkkonen.h"
#include <unordered_set>
#include <vector>
#include <string>

using namespace std;

struct RouteInfo {
    vector<string> pathNames;
    int stationCount = 0;
    int price = 0;
    int timeEstimate = 0;
    string directionDescription;
    string initialDirection;       // Starting line/direction
    string afterTransferDirection;  // Line/direction after the switch
};

class MetroSystem {
private:
    MetroUkkonen* tree = nullptr;
    unordered_map<string, int> nameToId;
    unordered_map<int, string> idToName;
    unordered_map<int, unordered_set<int>> stationToLines;
    int nextId = 1;

public:
    void buildSystem(string filename);
    vector<string> getAllStationNames() const;
    RouteInfo getRouteDetails(string start, string end);
    ~MetroSystem() { delete tree; }
};