#pragma once
#ifndef CARGOMANAGER_H
#define CARGOMANAGER_H

#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <algorithm>
#include "Cargo.h"
#include "Utils.h"

using namespace std;

class CargoManager {
private:
    vector<Cargo> cargos_;

	int findIndexById(const string& id) const;

public:
	CargoManager() = default;

	// Read operations
    size_t size() const noexcept { return cargos_.size(); }

    bool empty() const noexcept { return cargos_.empty(); }

    template <typename Fn>
    void forEach(Fn fn) const {
        for_each(cargos_.begin(), cargos_.end(), fn);
    }

    const Cargo* findById(const string& id) const;

    template <typename Predicate>
    vector<Cargo> filter(Predicate pred) const {
        vector<Cargo> result;
        copy_if(cargos_.begin(), cargos_.end(),
            back_inserter(result), pred);
        return result;
    }

	vector<Cargo> list() const { return cargos_; }

	// Write operations
    void clear() { cargos_.clear(); }
	bool loadFromCSV(const string& path);

    void add(const Cargo& c) { cargos_.push_back(c); }
    
	bool editById(const string& id, const string& newId, const string& newDest, int newDeadline);
	bool deleteById(const string& id);
    bool replaceById(const string& id, const Cargo& replacement);

    void printList() const;
};

#endif


