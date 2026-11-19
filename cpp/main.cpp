#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include "parcel.h"
#include "network.h"

using namespace std;
int main() {

    // STEP 1: Create sample parcels

    Parcel p1(101, "Sanjauli", 2.5, 1);
    Parcel p2(102, "Sanjauli", 1.5, 3);
    Parcel p3(103, "Dhalli", 3.0, 2);
    Parcel p4(104, "Sanjauli", 2.0, 1);
    Parcel p5(105, "Dhalli", 1.0, 3);


    // STEP 2: Store parcels in a Vector

    vector<Parcel> parcels = {
        p1, p2, p3, p4, p5
    };


    // STEP 3: Display all parcels

    cout << "========== PARCELMERGE ==========" << endl;

    cout << "\nALL PARCELS" << endl;
    cout << "-------------------------------" << endl;

    for (const Parcel& parcel : parcels) {

        cout << "Parcel ID: " << parcel.id << endl;
        cout << "Destination: " << parcel.destination << endl;
        cout << "Weight: " << parcel.weight << " kg" << endl;
        cout << "Priority: " << parcel.priority << endl;

        cout << "-------------------------------" << endl;
    }


    // STEP 4: Group parcels using Hash Map

    unordered_map<string, vector<int>> parcelGroups;

    for (const Parcel& parcel : parcels) {

        parcelGroups[parcel.destination].push_back(parcel.id);
    }


    // STEP 5: Display delivery groups

    cout << "\nDELIVERY GROUPS" << endl;
    cout << "===============================" << endl;

    for (const auto& group : parcelGroups) {

        cout << group.first << " -> ";

        for (int parcelId : group.second) {

            cout << "P" << parcelId << " ";
        }

        cout << endl;
    }


    // STEP 6: Create Delivery Graph

    Graph deliveryGraph = sampleDeliveryGraph();

    // STEP 7: Display Delivery Network

    deliveryGraph.displayGraph();


    // STEP 8: Create Priority Queue

    priority_queue<Parcel, vector<Parcel>, ParcelCompare> deliveryQueue;


    // STEP 9: Insert parcels into Priority Queue

    for (const Parcel& parcel : parcels) {

        deliveryQueue.push(parcel);
    }


    // STEP 10: Process parcels according to priority

    cout << "\nDELIVERY PRIORITY QUEUE" << endl;
    cout << "===============================" << endl;

    while (!deliveryQueue.empty()) {

        Parcel current = deliveryQueue.top();

        cout << "Parcel ID: " << current.id << endl;
        cout << "Destination: " << current.destination << endl;

        if (current.priority == 1) {
            cout << "Priority: Urgent" << endl;
        }
        else if (current.priority == 2) {
            cout << "Priority: Express" << endl;
        }
        else {
            cout << "Priority: Normal" << endl;
        }

        cout << "-------------------------------" << endl;

        deliveryQueue.pop();
    }


    // STEP 11: End program

    cout << "\nParcelMerge execution completed!" << endl;

    return 0;
}