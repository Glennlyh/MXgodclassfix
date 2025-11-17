#pragma once
#ifndef FREIGHTMANAGER_H
#define FREIGHTMANAGER_H

#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <algorithm>
#include "Freight.h"
#include "Utils.h"

using namespace std;

class FreightManager {
private:
    vector<Freight> freights_;

    int findIndexById(const string& id) const;

public:
	FreightManager() = default;

	// Read operations
    size_t size() const noexcept { return freights_.size(); }

    bool empty() const noexcept { return freights_.empty(); }

    template <class Fn>
    void forEach(Fn fn) const {
        for_each(freights_.begin(), freights_.end(), fn);
    }

    const Freight* findById(const string& id) const;

    template <typename Predicate>
    vector<Freight> filter(Predicate pred) const {
        vector<Freight> result;
        copy_if(freights_.begin(), freights_.end(), back_inserter(result), pred);
        return result;
	}

	vector<Freight> list() const { return freights_; }

	// Write operations
    void clear() { freights_.clear(); }
    bool loadFromCSV(const string& path);

    void add(const Freight& f) { freights_.push_back(f); }

    bool editById(const string& id, const string& newId, const string& newDest, int newTime);
    
    bool deleteById(const string& id);

    bool replaceById(const string& id, const Freight& replacement);

    void printList() const;
};

#endif 

