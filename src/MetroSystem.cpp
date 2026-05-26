#include "MetroSystem.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

void MetroSystem::buildSystem(string filename) {
    ifstream file(filename);
    if (!file.is_open()) return;
    string line;
    vector<int> fullSequence;
    int lineCount = 1, separator = 999;

    while (getline(file, line)) {
        stringstream ss(line);
        string name;
        vector<int> forwardLine;
        while (getline(ss, name, ',')) {
            name.erase(0, name.find_first_not_of(" \t\r\n"));
            name.erase(name.find_last_not_of(" \t\r\n") + 1);
            if (!nameToId.count(name)) {
                nameToId[name] = nextId; idToName[nextId] = name; nextId++;
            }
            int id = nameToId[name];
            forwardLine.push_back(id);
            stationToLines[id].insert(lineCount);
        }
        for(int id : forwardLine) fullSequence.push_back(id);
        fullSequence.push_back(separator--);
        reverse(forwardLine.begin(), forwardLine.end());
        for(int id : forwardLine) fullSequence.push_back(id);
        fullSequence.push_back(separator--);
        lineCount++;
    }
    fullSequence.push_back(0);
    tree = new MetroUkkonen(fullSequence, idToName);
}

vector<string> MetroSystem::getAllStationNames() const {
    vector<string> names;
    for (auto const& [name, id] : nameToId) names.push_back(name);
    sort(names.begin(), names.end());
    return names;
}

RouteInfo MetroSystem::getRouteDetails(string start, string end) {
    RouteInfo info;
    if (!nameToId.count(start) || !nameToId.count(end)) return info;

    int sId = nameToId[start], eId = nameToId[end];
    int commonLine = -1;
    for (int l : stationToLines[sId]) if (stationToLines[eId].count(l)) { commonLine = l; break; }

    vector<int> pathIds;
    if (commonLine != -1) {
        pathIds = tree->findPathVector(sId, eId);
        info.initialDirection = tree->identifyDirection(sId, eId);
        info.directionDescription = "Take " + info.initialDirection;
    } else {
        vector<string> transfers = {"Sadat", "Al-Shohadaa", "Attaba", "Nasser", "Cairo University", "Kit Kat"};
        string bestT = ""; int minL = 9999;
        for (const string& t : transfers) {
            if (!nameToId.count(t)) continue;
            int tid = nameToId[t];
            int l1 = tree->getPathLength(sId, tid), l2 = tree->getPathLength(tid, eId);
            if (l1 + l2 < minL) { minL = l1 + l2; bestT = t; }
        }
        if (!bestT.empty()) {
            int tId = nameToId[bestT];
            vector<int> p1 = tree->findPathVector(sId, tId);
            vector<int> p2 = tree->findPathVector(tId, eId);

            info.initialDirection = tree->identifyDirection(sId, tId);
            info.afterTransferDirection = tree->identifyDirection(tId, eId);
            info.directionDescription = "Transfer at " + bestT + " towards " + idToName[eId];

            pathIds = p1;
            for(size_t i = 1; i < p2.size(); ++i) pathIds.push_back(p2[i]);
        }
    }

    // Convert IDs to Names (Fixes the compilation error)
    for (int id : pathIds) info.pathNames.push_back(idToName[id]);

    info.stationCount = info.pathNames.size();
    info.timeEstimate = info.stationCount * 2;
    if (info.stationCount <= 9) info.price = 8;
    else if (info.stationCount <= 16) info.price = 10;
    else if (info.stationCount <= 23) info.price = 15;
    else info.price = 20;

    return info;
}