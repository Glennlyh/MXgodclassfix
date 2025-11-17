#include "CargoManager.h"
#include <fstream>
#include <sstream>
using namespace std;

static string trimLocal(const string& s) { return trim(s); }

static bool parseCargoRow(const string& line, Cargo& out) {
    stringstream ss(line);
    string id, dest, deadlineStr, groupStr;

    if (!getline(ss, id, ',')) return false;
    if (!getline(ss, dest, ',')) return false;
    if (!getline(ss, deadlineStr, ',')) return false;
    
    id = trimLocal(id); dest = trimLocal(dest); deadlineStr = trimLocal(deadlineStr);
    if (id.empty() || dest.empty() || deadlineStr.empty()) return false;
    
    int d = parseTimeToMinutes(deadlineStr);
    if (d < 0 || d >= 24 * 60) return false;
    
    // Try to read the optional group size from 4th column
    int groupSize = 1; // default
    if (getline(ss, groupStr, ',')) {
        groupStr = trimLocal(groupStr);
        if (!groupStr.empty()) {
            try {
                groupSize = stoi(groupStr);
                if (groupSize < 1) groupSize = 1;
                if (groupSize > 10) groupSize = 10;
            } catch (...) {
                groupSize = 1;
            }
        }
    }
    
    out = Cargo(id, dest, d, groupSize);
    
    return true;
}

bool CargoManager::loadFromCSV(const string& path) {
    ifstream in(path);
    if (!in.is_open()) return false;
    cargos_.clear();

    string line;
    while (getline(in, line)) {
        string s = trimLocal(line);
        if (s.empty())
            continue;
        if (s.find("id") == 0 || s.find("cargo") == 0 || s[0] == '#')
            continue; 
        
        Cargo c;
        if (parseCargoRow(s, c))
            cargos_.push_back(c);
    }
    return true;
}

int CargoManager::findIndexById(const string& id) const {
    auto it = find_if(cargos_.begin(), cargos_.end(),
        [&](const Cargo& c) { return c.getId() == id; });
    return (it == cargos_.end()) ? -1 : static_cast<int>(distance(cargos_.begin(), it));
}

const Cargo* CargoManager::findById(const string& id) const {
    int idx = findIndexById(id);
    return (idx < 0) ? nullptr : &cargos_[static_cast<size_t>(idx)];
}

bool CargoManager::editById(const string& id, const string& newId,
    const string& newDest, int newDeadline) {
    int i = findIndexById(id);
    if (i < 0)
        return false;
    if (!newId.empty())
        cargos_[i].setId(newId);
    if (!newDest.empty())
        cargos_[i].setDest(newDest);
    if (newDeadline >= 0 && newDeadline < 24 * 60)  // Add validation
        cargos_[i].setDeadline(newDeadline);
    return true;
}

bool CargoManager::deleteById(const string& id) {
    auto it = remove_if(cargos_.begin(), cargos_.end(),
        [&id](const Cargo& c) { return c.getId() == id; });
    
    if (it == cargos_.end()) return false;
    cargos_.erase(it, cargos_.end());
    return true;
}

bool CargoManager::replaceById(const string& id, const Cargo& replacement) {
    int i = findIndexById(id);
    if (i < 0)
        return false;
    cargos_[i] = replacement;  // No cast needed
    return true;
}

void CargoManager::printList() const {
    cout << "Cargos (" << cargos_.size() << "):\n";
    for (const auto& c : cargos_) cout << "  - " << c << "\n";
}

