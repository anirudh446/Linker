#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <utility>

using namespace std;

class Graph {
private:
    unordered_map<string, vector<pair<string, int>>> adjacencyList;

public:
    struct Route {
        long long distance;
        vector<string> stops;
    };

    void addEdge(string source, string destination, int distance);
    void displayGraph();
    unordered_map<string, Route> shortestRoutes(const string& source) const;
};

#endif
