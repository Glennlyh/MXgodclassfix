#include "Cargo.h"
#include "Utils.h"
#include <sstream>
using namespace std;

Cargo::Cargo(const string& id, const string& dest, int deadline, int groupSize)
    : Logistics(id, dest),
    deadline_(deadline),
    groupSize_(clampGroup(groupSize)) {}

string Cargo::toString() const {
    ostringstream os;
    os << id_ << " | " << dest_ << " | " << minutesToHHMM12(deadline_);
    if (groupSize_ > 1) os << " | group=" << groupSize_;
    return os.str();
}

ostream& operator<<(ostream& os, const Cargo& c) {
    return os << c.toString();
}

