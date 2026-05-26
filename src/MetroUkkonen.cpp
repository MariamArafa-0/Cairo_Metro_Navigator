#include "MetroUkkonen.h"
#include <algorithm>

Node::Node(int start, int* end) : start(start), end(end), suffixLink(nullptr) {}

MetroUkkonen::MetroUkkonen(const vector<int>& input, unordered_map<int, string>& mapping)
    : text(input), idToName(mapping) {
    rootEnd = new int(-1); root = new Node(-1, rootEnd);
    activeNode = root; lastLeafEnd = new int(-1);
    for (int i = 0; i < (int)text.size(); i++) extend(i);
}

int MetroUkkonen::edgeLength(Node* n) { return *(n->end) - (n->start) + 1; }

void MetroUkkonen::extend(int pos) {
    (*lastLeafEnd)++; remainingSuffixCount++;
    Node* lastNewNode = nullptr;
    while (remainingSuffixCount > 0) {
        if (activeLength == 0) activeEdge = pos;
        auto it = activeNode->children.find(text[activeEdge]);
        if (it == activeNode->children.end()) {
            activeNode->children[text[activeEdge]] = new Node(pos, lastLeafEnd);
            if (lastNewNode) lastNewNode->suffixLink = activeNode;
            lastNewNode = nullptr;
        } else {
            Node* next = it->second;
            int length = edgeLength(next);
            if (activeLength >= length) {
                activeEdge += length; activeLength -= length; activeNode = next;
                continue;
            }
            if (text[next->start + activeLength] == text[pos]) {
                if (lastNewNode && activeNode != root) lastNewNode->suffixLink = activeNode;
                activeLength++; break;
            }
            int* splitEnd = new int(next->start + activeLength - 1);
            Node* split = new Node(next->start, splitEnd);
            activeNode->children[text[activeEdge]] = split;
            split->children[text[pos]] = new Node(pos, lastLeafEnd);
            next->start += activeLength; split->children[text[next->start]] = next;
            if (lastNewNode) lastNewNode->suffixLink = split; lastNewNode = split;
        }
        remainingSuffixCount--;
        if (activeNode == root && activeLength > 0) {
            activeLength--; activeEdge = pos - remainingSuffixCount + 1;
        } else if (activeNode != root) {
            activeNode = activeNode->suffixLink ? activeNode->suffixLink : root;
        }
    }
}

vector<int> MetroUkkonen::findPathVector(int refId, int targetId) {
    vector<int> path;
    if (refId == targetId) return {refId};
    if (root->children.find(refId) == root->children.end()) return {};
    path.push_back(refId);
    if (findTarget(root->children[refId], targetId, path, true)) return path;
    return {};
}

int MetroUkkonen::getPathLength(int refId, int targetId) {
    auto p = findPathVector(refId, targetId);
    return p.empty() ? 9999 : (int)p.size();
}

string MetroUkkonen::identifyDirection(int refId, int targetId) {
    int currentSegmentStart = 0, segmentIdx = 0;
    for (int i = 0; i < (int)text.size(); i++) {
        if (text[i] >= 990) {
            bool foundRef = false;
            for (int j = currentSegmentStart; j < i; j++) {
                if (text[j] == refId) foundRef = true;
                if (foundRef && text[j] == targetId) {
                    switch(segmentIdx) {
                        // These indices correspond to the order lines are added in MetroSystem::buildSystem
                        case 0: return "Line 1 (New El-Marg direction)";
                        case 1: return "Line 1 (Helwan direction)";
                        case 2: return "Line 2 (Shubra El-Kheima direction)";
                        case 3: return "Line 2 (El-Mounib direction)";
                        case 4: return "Line 3 (Rod al-Farag direction)";
                        case 5: return "Line 3 (Adly Mansour direction)";
                        case 6: return "Line 3 (Cairo University direction)";
                        case 7: return "Line 3 (Adly Mansour direction)";
                        default: return "Metro Line";
                    }
                }
            }
            currentSegmentStart = i + 1; segmentIdx++;
        }
    }
    return "Metro Line";
}

bool MetroUkkonen::findTarget(Node* n, int targetId, vector<int>& path, bool isFirstNode) {
    int startIdx = isFirstNode ? n->start + 1 : n->start;
    int addedCount = 0;
    for (int i = startIdx; i <= *(n->end); i++) {
        if (text[i] >= 990) { while(addedCount--) path.pop_back(); return false; }
        path.push_back(text[i]); addedCount++;
        if (text[i] == targetId) return true;
    }
    for (auto const& [val, child] : n->children) if (findTarget(child, targetId, path)) return true;
    while(addedCount--) path.pop_back();
    return false;
}