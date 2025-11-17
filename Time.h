#pragma once
#include <string>

using namespace std;

class Time {
    int minutes_; 
public:
    explicit Time(int mins = -1) : minutes_(mins) {}

    static Time parse(const string& s);

    int minutes() const { return minutes_; }
    bool isKnown() const { return minutes_ >= 0; }

    string hhmm12() const;


    static Time fromHHMM(int hhmm);
    Time addMinutes(int delta) const;
    static int compare(Time a, Time b);

    static bool inArrivalWindow(Time freightDepart, Time cargoDeadline, int earlyAllowanceMins);
};
