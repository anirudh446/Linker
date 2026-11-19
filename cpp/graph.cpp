#include "graph.h"
#include <algorithm>
#include <functional>
#include <queue>
#include <stdexcept>

void Graph::addEdge(string source, string destination, int distance) {

    if (distance < 0) {
        throw invalid_argument("Edge distances must be nonnegative");
    }
    adjacencyList[source].push_back({destination, distance});

    adjacencyList[destination].push_back({source, distance});
}


void Graph::displayGraph() {

    cout << endl;
    cout << "DELIVERY NETWORK" << endl;
    cout << "================" << endl;

    for (const auto& location : adjacencyList) {

        cout << location.first << ":" << endl;

        for (const auto& connection : location.second) {

            cout << "  -> "
                 << connection.first
                 << " (" << connection.second << " km)"
                 << endl;
        }

        cout << endl;
    }
}
unordered_map<string, Graph::Route> Graph::shortestRoutes(const string& source) const {
    if (!adjacencyList.count(source)) {
        throw invalid_argument("Unknown source: " + source);
    }
    using Entry = pair<long long, string>;
    priority_queue<Entry, vector<Entry>, greater<Entry>> pending;
    unordered_map<string, long long> distances{{source, 0}};
    unordered_map<string, string> previous;
    pending.emplace(0, source);

    while (!pending.empty()) {
        auto [distance, location] = pending.top();
        pending.pop();
        if (distance != distances.at(location)) {
            continue;
        }
        for (const auto& [neighbor, length] : adjacencyList.at(location)) {
            const auto candidate = distance + length;
            const auto known = distances.find(neighbor);
            if (known == distances.end() || candidate < known->second) {
                distances[neighbor] = candidate;
                previous[neighbor] = location;
                pending.emplace(candidate, neighbor);
            }
        }
    }

    unordered_map<string, Route> routes;
    for (const auto& [location, distance] : distances) {
        vector<string> stops{location};
        while (stops.back() != source) {
            stops.push_back(previous.at(stops.back()));
        }
        reverse(stops.begin(), stops.end());
        routes.emplace(location, Route{distance, move(stops)});
    }
    return routes;
}