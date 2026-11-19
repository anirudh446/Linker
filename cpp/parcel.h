#ifndef PARCEL_H
#define PARCEL_H

#include <string>
using namespace std;

struct Parcel {
    int id;
    string destination;
    double weight;
    int priority;

    Parcel(int id, string destination, double weight, int priority)
        : id(id), destination(destination),
          weight(weight), priority(priority) {}
};

struct ParcelCompare {
    bool operator()(const Parcel& a, const Parcel& b) const {
        if (a.priority == b.priority) {
            return a.id > b.id;
        }
        return a.priority > b.priority;
    }
};

#endif