#include "ConsoleUI.h"
#include "InputHandler.h"
#include "InputParser.h"
#include "FileHandler.h"
#include "SchedulePrinter.h"
#include <iostream>

using namespace std;

// ============================================================================
// ConsoleUI - Thin coordinator
// All methods delegate to specialized classes
// ============================================================================

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu Option 1 - Load Files
void ConsoleUI::loadFilesAndValidate(FreightManager& fm, CargoManager& cm) const {
	FileHandler::loadFiles(fm, cm);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu Option 2 -  Add/Edit/Delete 
// Cargo CRUD
void ConsoleUI::cargoAdd(CargoManager& cm) {
	string id = InputHandler::ask("id: ");
	string dest = InputHandler::ask("dest: ");
	int ddl = InputHandler::askTime("deadline (HHMM or H:MM): ");
	int group = InputParser::parseGroupSize(InputHandler::ask("group size [1..10] (default 1): "));
	cm.add(Cargo(trim(id), trim(dest), ddl, group));
	cout << "Added cargo (group=" << group << ").\n";
}

void ConsoleUI::cargoEdit(CargoManager& cm) {
	string id = InputHandler::ask("id to edit: ");
	string nid = InputHandler::ask("new id (blank to keep): ");
	string nd = InputHandler::ask("new dest (blank to keep): ");
	string nt = InputHandler::ask("new deadline (blank  tokeep): ");
	string ng = InputHandler::ask("new group size [1..10] (blank to keep): ");
	bool changeGroup = !trim(ng).empty();

	if (!changeGroup) {
		int t = nt.empty() ? -1 : parseTimeToMinutes(nt);
		cout << (cm.editById(trim(id), trim(nid), trim(nd), t) ? "Updated.\n" : "Not found.\n");
	}
	else {
		if (!cm.deleteById(trim(id))) { 
			cout << "Not found.\n";
			return;
		}
		string finalId = trim(nid).empty() ? trim(id) : trim(nid);
		string finalDest = trim(nd);
		int    finalTime = nt.empty() ? -1 : parseTimeToMinutes(nt);
		if (finalDest.empty())
			finalDest = InputHandler::ask("dest (required): ");
		if (finalTime < 0)
			finalTime = InputHandler::askTime("deadline (HHMM or H:MM) (required): ");
		int finalGroup = InputParser::parseGroupSize(ng);
		cm.add(Cargo(finalId, trim(finalDest), finalTime, finalGroup));
		cout << "Group size updated to " << finalGroup << ".\n";
	}
}

void ConsoleUI::cargoDelete(CargoManager& cm) {
	string id = InputHandler::ask("id to delete: ");
	cout << (cm.deleteById(trim(id)) ? "Deleted.\n" : "Not found.\n");
}

// Freight CRUD
void ConsoleUI::freightAdd(FreightManager& fm) {
	string id = InputHandler::ask("id: ");
	string dest = InputHandler::ask("dest: ");
	int dep = InputHandler::askTime("depart (HHMM or H:MM): ");
	FreightType ft = InputParser::parseFreightType(InputHandler::ask("type [mini/cruiser/mega or 2/6/12] (default mini): "));
	fm.add(Freight(trim(id), trim(dest), dep, ft));
	cout << "Added freight.\n";
}

void ConsoleUI::freightEdit(FreightManager& fm) {
	string id = InputHandler::ask("id to edit: ");
	string nid = InputHandler::ask("new id (blank to keep): ");
	string nd = InputHandler::ask("new dest (blank to keep): ");
	string nt = InputHandler::ask("new depart (blank to keep): ");
	string nty = InputHandler::ask("new type [mini/cruiser/mega or 2/6/12] (blank to keep): ");
	bool changeType = !trim(nty).empty();

	if (!changeType) {
		int t = nt.empty() ? -1 : parseTimeToMinutes(nt);
		cout << (fm.editById(trim(id), trim(nid), trim(nd), t) ? "Updated.\n" : "Not found.\n");
	}
	else {
		if (!fm.deleteById(trim(id))) {
			cout << "Not found.\n";
			return;
		}
		string finalId = trim(nid).empty() ? trim(id) : trim(nid);
		string finalDest = trim(nd);
		int    finalTime = nt.empty() ? -1 : parseTimeToMinutes(nt);
		if (finalDest.empty())
			finalDest = InputHandler::ask("dest (required): ");
		if (finalTime < 0)
			finalTime = InputHandler::askTime("depart (HHMM or H:MM) (requiredd): ");
		FreightType ft = InputParser::parseFreightType(nty);
		fm.add(Freight(finalId, trim(finalDest), finalTime, ft));
		cout << "Type/capacity updated.\n";
	}
}

void ConsoleUI::freightDelete(FreightManager& fm) {
	string id = InputHandler::ask("id to delete: ");
	cout << (fm.deleteById(trim(id)) ? "Deleted.\n" : "Not found.\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu Option 3: Show lists
void ConsoleUI::showLists(const FreightManager& fm, const CargoManager& cm) const {
	// Freights
	const auto fcount = fm.list().size();
	cout << "Freights (" << fcount << "):\n";
	fm.forEach([&](const Freight& f) {
		cout << "  - " << f.getId()
			<< " | " << f.getDest()
			<< " | " << minutesToHHMM12(f.getTime())
			<< "\n";
		});

	// Cargos
	const auto ccount = cm.list().size();
	cout << "Cargos (" << ccount << "):\n";
	cm.forEach([&](const Cargo& c) {
		cout << "  - " << c.getId()
			<< " | " << c.getDest()
			<< " | " << minutesToHHMM12(c.getDeadline())
			<< "\n";
		});
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu Option 4 - View Assigned Schedule
void ConsoleUI::showPlanByCargoTime(const FreightManager& fm, const CargoManager& cm) const {
	auto freights = fm.list();
	auto cargos = cm.list();
	SchedulePrinter::printPlanSortedByCargoTime(freights, cargos);
}

void ConsoleUI::showPlanByMinimumFreight(const FreightManager& fm, const CargoManager& cm) const {
	auto freights = fm.list();
	auto cargos = cm.list();
	SchedulePrinter::printPlanSortedByMinimumFreight(freights, cargos);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu Option 6 - Save & Export
void ConsoleUI::saveScheduleAndCSV(const FreightManager& fm, const CargoManager& cm, const string& txtFilename)
{
	FileHandler::saveSchedule(fm, cm, txtFilename);
}
