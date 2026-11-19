#pragma once

#include "graph.h"
#include "parcel.h"

struct Trip {
    string destination;
    vector<int> parcelIds;
    double load = 0;
    int priority = 3;
    Graph::Route route{};
};

vector<Trip> mergeParcels(const vector<Parcel>& parcels, double capacity);

struct DeliveryPlan {
    vector<Trip> trips;
    size_t baselineTrips = 0;
    long long baselineDistance = 0;
    long long plannedDistance = 0;

    size_t tripsSaved() const { return baselineTrips - trips.size(); }
    long long distanceSaved() const { return baselineDistance - plannedDistance; }
};

DeliveryPlan planDeliveries(const Graph& graph, const string& depot,
                            const vector<Parcel>& parcels, double capacity);
