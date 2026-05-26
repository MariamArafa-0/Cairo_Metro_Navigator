#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

struct Node {
    unordered_map<int, Node*> children;
    Node* suffixLink;
    int start, *end;
    Node(int start, int* end);
};

class MetroUkkonen {
private:
    Node *root, *activeNode;
    int activeEdge = -1, activeLength = 0, remainingSuffixCount = 0;
    int *rootEnd, *lastLeafEnd;
    vector<int> text;
    unordered_map<int, string> idToName;

    int edgeLength(Node* n);
    void extend(int pos);
    bool findTarget(Node* n, int targetId, vector<int>& path, bool isFirstNode = false);

public:
    MetroUkkonen(const vector<int>& input, unordered_map<int, string>& mapping);
    int getPathLength(int refId, int targetId);
    string identifyDirection(int refId, int targetId);
    vector<int> findPathVector(int refId, int targetId);
    void getDirectionalPath(int refId, int targetId);
};