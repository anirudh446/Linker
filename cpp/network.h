#pragma once

#include "graph.h"

inline Graph sampleDeliveryGraph() {
    Graph graph;
    graph.addEdge("Warehouse", "Sanjauli", 5);
    graph.addEdge("Warehouse", "Dhalli", 8);
    graph.addEdge("Sanjauli", "Dhalli", 3);
    graph.addEdge("Sanjauli", "Mall Road", 4);
    graph.addEdge("Dhalli", "Mall Road", 6);
    return graph;
}
