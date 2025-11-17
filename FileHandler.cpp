#include "FileHandler.h"
#include "InputHandler.h"
#include "InputParser.h"
#include "ScheduleExporter.h"
#include "Utils.h"
#include <iostream>

using namespace std;

// ============================================================================
// Load freight and cargo files
// ============================================================================

void FileHandler::loadFiles(FreightManager& fm, CargoManager& cm) {
    string fpath = "freight.txt";
    string cpath = "cargo.txt";

    cout << "Paths [default in current folder]:\n";

    cout << "  Freights file [" << fpath << "]: ";
    string tmp;
    getline(cin, tmp);
    if (!tmp.empty()) fpath = tmp;

    cout << "  Cargos file   [" << cpath << "]: ";
    getline(cin, tmp);
    if (!tmp.empty()) cpath = tmp;

    if (fm.loadFromCSV(trim(fpath)))
        cout << "Loaded freights (" << fm.list().size() << ").\n";
    else
        cout << "Failed to load freights.\n";

    if (cm.loadFromCSV(trim(cpath)))
        cout << "Loaded cargos (" << cm.list().size() << ").\n";
    else
        cout << "Failed to load cargos.\n";
}

// ============================================================================
// Save schedule and CSV files
// ============================================================================

void FileHandler::saveSchedule(const FreightManager& fm, 
                               const CargoManager& cm, 
                               const string& txtFilename) {
    auto freights = fm.list();
    auto cargos = cm.list();

    const string txt = trim(txtFilename);
    const string csv = InputParser::createCSVFilename(txt);

    // Ask user which CSV format they want
    cout << "CSV export format:\n"
        << "  a) Sorted by cargo arrival time\n"
        << "  b) Sorted by minimum freight\n"
        << "Select: ";
    string csvChoice;
    getline(cin, csvChoice);

    const bool okTxt = ScheduleExporter::saveScheduleToFile(freights, cargos, txt);
    bool okCsv = false;

    if (csvChoice == "a") {
        okCsv = ScheduleExporter::savePlanByCargoTimeCSV(freights, cargos, csv);
    }
    else if (csvChoice == "b") {
        okCsv = ScheduleExporter::savePlanByMinimumFreightCSV(freights, cargos, csv);
    }
    else {
        cout << "Invalid choice. Defaulting to sorted by cargo arrival time.\n";
        okCsv = ScheduleExporter::savePlanByCargoTimeCSV(freights, cargos, csv);
    }

    if (okTxt)
        cout << "Saved updated text schedule to " << txt << "\n";
    else
        cout << "Failed to save text schedule.\n";

    if (okCsv)
        cout << "Saved updated CSV schedule to " << csv << "\n";
    else
        cout << "Failed to save updated CSV.\n";
}
