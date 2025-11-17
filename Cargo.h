#pragma once
#ifndef CARGO_H
#define CARGO_H

#include <string>
#include <ostream>
#include "Logistics.h"

using namespace std;

class Cargo : public Logistics {
private:
    int deadline_;
    int groupSize_;

    static int clampGroup(int g) {
        if (g < 1) return 1;
		if (g > 10) return 10;
        return g;
    }

public:
    Cargo(const string& id = "", const string& dest = "", int deadline_ = -1, int groupSize = 1);

    int  getDeadline() const { return deadline_; }
    void setDeadline(int d) { deadline_ = d; }

    int  getTimestamp() const override { return deadline_; }
    void setTimestamp(int v) override { deadline_ = v; }
    const char* timestampLabel() const override { return "deadline"; }

	int getGroupSize() const { return groupSize_; }
	void setGroupSize(int g) { groupSize_ = clampGroup(g); }

    string toString() const;
    friend ostream& operator<<(ostream& os, const Cargo& c);
};

#endif


