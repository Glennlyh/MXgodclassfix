#pragma once
#ifndef FREIGHT_H
#define FREIGHT_H

#include <string>
#include <ostream>
#include "Logistics.h"

using namespace std;

enum class FreightType {
    MiniMover,
    CargoCruiser,
    MegaCarrier
};


class Freight : public Logistics {
private:
    int time_;
    FreightType type_;
    int maxCapacity_;

    static int capacityFor(FreightType ft);

public:
    Freight(const string& id = "", const string& dest = "", int time = -1);

    Freight(const string& id, const string& dest, int time, FreightType type);

    int  getTime() const { return time_; }
    void setTime(int t) { time_ = t; }

    int  getTimestamp() const override { return time_; }
    void setTimestamp(int v) override { time_ = v; }
    const char* timestampLabel() const override { return "depart"; }

    FreightType getType() const { return type_; }
    int getMaxCapacity() const { return maxCapacity_; }

    string toString() const;
    friend ostream& operator<<(ostream& os, const Freight& f);
};

#endif 