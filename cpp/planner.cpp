#include "planner.h"

#include <cmath>
#include <queue>
#include <stdexcept>
#include <unordered_set>

namespace {
long long grams(double kilograms) {
    if (!std::isfinite(kilograms) || kilograms <= 0 || kilograms > 1e9) {
        return 0;
    }
    const auto value = std::llround(kilograms * 1000);
    return value / 1000.0 == kilograms ? value : 0;
}
}

vector<Trip> mergeParcels(const vector<Parcel>& parcels, double capacity) {
    const auto capacityGrams = grams(capacity);
    if (!capacityGrams) {
        throw invalid_argument("Vehicle capacity must be positive with up to three decimal places (maximum 1 billion kg)");
    }

    unordered_set<int> ids;
    unordered_map<string, size_t> openTrips;
    vector<Trip> trips;
    vector<long long> loads;
    priority_queue<Parcel, vector<Parcel>, ParcelCompare> pending;
    for (const auto& parcel : parcels) {
        if (parcel.priority < 1 || parcel.priority > 3) {
            throw invalid_argument("Parcel priority must be 1, 2, or 3");
        }
        if (!ids.insert(parcel.id).second) {
            throw invalid_argument("Duplicate parcel ID: " + to_string(parcel.id));
        }
        const auto weightGrams = grams(parcel.weight);
        if (!weightGrams || weightGrams > capacityGrams) {
            throw invalid_argument("Parcel " + to_string(parcel.id) + " has an invalid weight or exceeds capacity");
        }

        pending.push(parcel);
    }

    while (!pending.empty()) {
        const Parcel parcel = pending.top();
        pending.pop();
        const auto weightGrams = grams(parcel.weight);
        auto open = openTrips.find(parcel.destination);
        if (open == openTrips.end() || loads[open->second] + weightGrams > capacityGrams) {
            openTrips[parcel.destination] = trips.size();
            Trip trip;
            trip.destination = parcel.destination;
            trip.priority = parcel.priority;
            trips.push_back(move(trip));
            loads.push_back(0);
        }
        const size_t index = openTrips.at(parcel.destination);
        Trip& trip = trips[index];
        loads[index] += weightGrams;
        trip.load = loads[index] / 1000.0;
        trip.parcelIds.push_back(parcel.id);
    }
    return trips;
}

DeliveryPlan planDeliveries(const Graph& graph, const string& depot,
                            const vector<Parcel>& parcels, double capacity) {
    DeliveryPlan plan;
    plan.trips = mergeParcels(parcels, capacity);
    const auto routes = graph.shortestRoutes(depot);
    for (auto& trip : plan.trips) {
        const auto route = routes.find(trip.destination);
        if (route == routes.end()) {
            throw invalid_argument("No route from " + depot + " to " + trip.destination);
        }
        trip.route = route->second;
        const auto roundTripDistance = 2 * trip.route.distance;
        plan.baselineTrips += trip.parcelIds.size();
        plan.baselineDistance += roundTripDistance * static_cast<long long>(trip.parcelIds.size());
        plan.plannedDistance += roundTripDistance;
    }
    return plan;
}
