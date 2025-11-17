#include "Freight.h"
#include "Utils.h"
#include <sstream>
using namespace std;

int Freight::capacityFor(FreightType ft) {
    switch (ft) {
    case FreightType::MiniMover:
        return 2;

    case FreightType::CargoCruiser:
        return 6;

    case FreightType::MegaCarrier:
        return 12;

    default:
        return 2;
    }
}

Freight::Freight(const string& id, const string& dest, int time)
    : Freight(id, dest, time, FreightType::MiniMover) {}

Freight::Freight(const string& id, const string& dest, int time, FreightType type)
    : Logistics(id, dest),
    time_(time),
    type_(type),
    maxCapacity_(capacityFor(type)) {}

string Freight::toString() const {
    ostringstream os;
    os << id_ << " | " << dest_ << " | " << minutesToHHMM12(time_);
    return os.str();
}

ostream& operator<<(ostream& os, const Freight& f) {
    return os << f.toString();
}
