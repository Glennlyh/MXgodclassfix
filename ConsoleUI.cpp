#include "ConsoleUI.h"
#include <iostream>
#include <cctype>

// Generic Helpers
std::string ConsoleUI::ask(const char* prompt) {
	std::cout << prompt;
	std::string s;
	std::getline(std::cin, s);

	return s;
}

int ConsoleUI::askTime(const char* prompt) {
	std::string t = ask(prompt);
	return parseTimeToMinutes(t);
}

int ConsoleUI::parseGroupUI(std::string s) {
	s = trim(s);
	if (s.empty())
		return 1;

	int g = 1;
	try { g = std::stoi(s); }
	catch (...) { g = 1; }
	if (g < 1)
		g = 1;
	if (g > 10)
		g = 10;

	return g;
}

FreightType ConsoleUI::parseFreightTypeUI(std::string s) {
	s = trim(s);
	for (char& ch : s)
		ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
	if (s == "12" || s == "megacarrier" || s == "mega")
		return FreightType::MegaCarrier;
	if (s == "6" || s == "cargocruiser" || s == "cruiser")
		return FreightType::CargoCruiser;
	if (s == "2" || s == "minimover" || s == "mini")
		return FreightType::MiniMover;

	// default 
	return FreightType::MiniMover;
}

// Filename Helpers
std::string ConsoleUI::createCSV(std::string base) {
	base = trim(base);
	if (base.size() >= 4 && base.substr(base.size() - 4) == ".txt") {
		base = base.substr(0, base.size() - 4) + ".csv";
	}
	else if (base.size() >= 4 && base.substr(base.size() - 4) == ".csv") {}
	else {
		base += ".csv";
	}
	return base;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu Option 1 - Load Files
void ConsoleUI::loadFilesAndValidate(FreightManager& fm, CargoManager& cm) const {
	std::string fpath = "freight.txt";
	std::string cpath = "cargo.txt";

	std::cout << "Paths [default in current folder]:\n";

	std::cout << "  Freights file [" << fpath << "]: ";
	std::string tmp; std::getline(std::cin, tmp);
	if (!tmp.empty()) fpath = tmp;

	std::cout << "  Cargos file   [" << cpath << "]: ";
	std::getline(std::cin, tmp);
	if (!tmp.empty()) cpath = tmp;


	if (fm.loadFromCSV(trim(fpath)))
		std::cout << "Loaded freights (" << fm.list().size() << ").\n";
	else
		std::cout << "Failed to load freights.\n";

	if (cm.loadFromCSV(trim(cpath)))
		std::cout << "Loaded cargos (" << cm.list().size() << ").\n";
	else
		std::cout << "Failed to load cargos.\n";
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu Option 2 -  Add/Edit/Delete 
// Cargo CRUD
void ConsoleUI::cargoAdd(CargoManager& cm) {
	string id = ask("id: ");
	string dest = ask("dest: ");
	int ddl = askTime("deadline (HHMM or H:MM): ");
	int group = parseGroupUI(ask("group size [1..10] (default 1): "));
	cm.add(Cargo(trim(id), trim(dest), ddl, group));
	std::cout << "Added cargo (group=" << group << ").\n";
}
void ConsoleUI::cargoEdit(CargoManager& cm) {
	string id = ask("id to edit: ");
	string nid = ask("new id (blank to keep): ");
	string nd = ask("new dest (blank to keep): ");
	string nt = ask("new deadline (blank  tokeep): ");
	string ng = ask("new group size [1..10] (blank to keep): ");
	bool changeGroup = !trim(ng).empty();

	if (!changeGroup) {
		int t = nt.empty() ? -1 : parseTimeToMinutes(nt);
		std::cout << (cm.editById(trim(id), trim(nid), trim(nd), t) ? "Updated.\n" : "Not found.\n");
	}
	else {
		if (!cm.deleteById(trim(id))) { 
			std::cout << "Not found.\n";
			return;
		}
		string finalId = trim(nid).empty() ? trim(id) : trim(nid);
		string finalDest = trim(nd);
		int    finalTime = nt.empty() ? -1 : parseTimeToMinutes(nt);
		if (finalDest.empty())
			finalDest = ask("dest (required): ");
		if (finalTime < 0)
			finalTime = askTime("deadline (HHMM or H:MM) (required): ");
		int finalGroup = parseGroupUI(ng);
		cm.add(Cargo(finalId, trim(finalDest), finalTime, finalGroup));
		std::cout << "Group size updated to " << finalGroup << ".\n";
	}
}

void ConsoleUI::cargoDelete(CargoManager& cm) {
	string id = ask("id to delete: ");
	std::cout << (cm.deleteById(trim(id)) ? "Deleted.\n" : "Not found.\n");
}

// Freight CRUD
void ConsoleUI::freightAdd(FreightManager& fm) {
	string id = ask("id: ");
	string dest = ask("dest: ");
	int dep = askTime("depart (HHMM or H:MM): ");
	FreightType ft = parseFreightTypeUI(ask("type [mini/cruiser/mega or 2/6/12] (default mini): "));
	fm.add(Freight(trim(id), trim(dest), dep, ft));
	std::cout << "Added freight.\n";
}
void ConsoleUI::freightEdit(FreightManager& fm) {
	string id = ask("id to edit: ");
	string nid = ask("new id (blank to keep): ");
	string nd = ask("new dest (blank to keep): ");
	string nt = ask("new depart (blank to keep): ");
	string nty = ask("new type [mini/cruiser/mega or 2/6/12] (blank to keep): ");
	bool changeType = !trim(nty).empty();

	if (!changeType) {
		int t = nt.empty() ? -1 : parseTimeToMinutes(nt);
		std::cout << (fm.editById(trim(id), trim(nid), trim(nd), t) ? "Updated.\n" : "Not found.\n");
	}
	else {
		if (!fm.deleteById(trim(id))) {
			std::cout << "Not found.\n";
			return;
		}
		string finalId = trim(nid).empty() ? trim(id) : trim(nid);
		string finalDest = trim(nd);
		int    finalTime = nt.empty() ? -1 : parseTimeToMinutes(nt);
		if (finalDest.empty())
			finalDest = ask("dest (required): ");
		if (finalTime < 0)
			finalTime = askTime("depart (HHMM or H:MM) (requiredd): ");
		FreightType ft = parseFreightTypeUI(nty);
		fm.add(Freight(finalId, trim(finalDest), finalTime, ft));
		std::cout << "Type/capacity updated.\n";
	}
}
void ConsoleUI::freightDelete(FreightManager& fm) {
	string id = ask("id to delete: ");
	std::cout << (fm.deleteById(trim(id)) ? "Deleted.\n" : "Not found.\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu Option 3: Show lists
void ConsoleUI::showLists(const FreightManager& fm, const CargoManager& cm) const {
	// Freights
	const auto fcount = fm.list().size();
	std::cout << "Freights (" << fcount << "):\n";
	fm.forEach([&](const Freight& f) {
		std::cout << "  - " << f.getId()
			<< " | " << f.getDest()
			<< " | " << minutesToHHMM12(f.getTime())
			<< "\n";
		});

	// Cargos
	const auto ccount = cm.list().size();
	std::cout << "Cargos (" << ccount << "):\n";
	cm.forEach([&](const Cargo& c) {
		std::cout << "  - " << c.getId()
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
	MatchingEngine::printPlanSortedByCargoTime(freights, cargos);
}

void ConsoleUI::showPlanByMinimumFreight(const FreightManager& fm, const CargoManager& cm) const {
	auto freights = fm.list();
	auto cargos = cm.list();
	MatchingEngine::printPlanSortedByMinimumFreight(freights, cargos);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu Option 6 - Save & Export
void ConsoleUI::saveScheduleAndCSV(const FreightManager& fm, const CargoManager& cm, const std::string& txtFilename)
{
	auto freights = fm.list();
	auto cargos = cm.list();

	const std::string txt = trim(txtFilename);
	const std::string csv = createCSV(txt);

	// Ask user which CSV format they want
	std::cout << "CSV export format:\n"
		<< "  a) Sorted by cargo arrival time\n"
		<< "  b) Sorted by minimum freight\n"
		<< "Select: ";
	std::string csvChoice;
	std::getline(std::cin, csvChoice);

	const bool okTxt = MatchingEngine::saveScheduleToFile(freights, cargos, txt);
	bool okCsv = false;

	if (csvChoice == "a") {
		okCsv = MatchingEngine::savePlanByCargoTimeCSV(freights, cargos, csv);
	}
	else if (csvChoice == "b") {
		okCsv = MatchingEngine::savePlanByMinimumFreightCSV(freights, cargos, csv);
	}
	else {
		std::cout << "Invalid choice. Defaulting to sorted by cargo arrival time.\n";
		okCsv = MatchingEngine::savePlanByCargoTimeCSV(freights, cargos, csv);
	}

	if (okTxt)
		std::cout << "Saved updated text schedule to " << txt << "\n";
	else
		std::cout << "Failed to save text schedule.\n";

	if (okCsv)
		std::cout << "Saved updated CSV schedule to " << csv << "\n";
	else
		std::cout << "Failed to save updated CSV.\n";
}