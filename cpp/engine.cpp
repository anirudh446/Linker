#include "planner.h"

#include <iomanip>
#include <limits>
#include <stdexcept>

int main() {
    try {
        double capacity;
        int parcelCount;
        if (!(cin >> capacity >> parcelCount) || parcelCount < 0 || parcelCount > 1000) {
            throw invalid_argument("Expected capacity and a parcel count from 0 to 1000");
        }
        Graph graph;
        int roadCount;
        if (!(cin >> roadCount) || roadCount != 5) {
            throw invalid_argument("Expected five road distances");
        }
        for (int index = 0; index < roadCount; ++index) {
            string source, destination;
            int meters;
            if (!(cin >> quoted(source) >> quoted(destination) >> meters) || meters <= 0) {
                throw invalid_argument("Invalid road distance at row " + to_string(index + 1));
            }
            graph.addEdge(source, destination, meters);
        }
        vector<Parcel> parcels;
        parcels.reserve(parcelCount);
        for (int index = 0; index < parcelCount; ++index) {
            int id, priority;
            double weight;
            string destination;
            if (!(cin >> id >> weight >> priority >> quoted(destination))) {
                throw invalid_argument("Invalid parcel record at row " + to_string(index + 1));
            }
            parcels.emplace_back(id, destination, weight, priority);
        }

        const auto plan = planDeliveries(graph, "Warehouse", parcels, capacity);
        cout << setprecision(numeric_limits<double>::max_digits10)
             << "{\"baselineTrips\":" << plan.baselineTrips
             << ",\"plannedTrips\":" << plan.trips.size()
             << ",\"tripsSaved\":" << plan.tripsSaved()
             << ",\"baselineDistance\":" << plan.baselineDistance / 1000.0
             << ",\"plannedDistance\":" << plan.plannedDistance / 1000.0
             << ",\"distanceSaved\":" << plan.distanceSaved() / 1000.0 << ",\"trips\":[";
        bool firstTrip = true;
        for (const auto& trip : plan.trips) {
            if (!firstTrip) cout << ',';
            firstTrip = false;
            cout << "{\"destination\":" << quoted(trip.destination)
                 << ",\"load\":" << trip.load << ",\"priority\":" << trip.priority
                 << ",\"distance\":" << 2 * trip.route.distance / 1000.0 << ",\"parcelIds\":[";
            for (size_t i = 0; i < trip.parcelIds.size(); ++i) {
                if (i) cout << ',';
                cout << trip.parcelIds[i];
            }
            cout << "],\"route\":[";
            for (size_t i = 0; i < trip.route.stops.size(); ++i) {
                if (i) cout << ',';
                cout << quoted(trip.route.stops[i]);
            }
            cout << "]}";
        }
        cout << "]}\n";
    } catch (const exception& error) {
        cerr << error.what() << '\n';
        return 1;
    }
}
