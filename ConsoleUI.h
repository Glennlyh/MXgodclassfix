#pragma once
#include <string>
#include "MatchingEngine.h"
#include "FreightManager.h"
#include "CargoManager.h"
#include "Utils.h"
#include "Freight.h"
#include "Cargo.h"

class ConsoleUI
{
private:
	// Menu Option 1 - Load Files -- NA
	
	// Menu Option 2 - CRUD
	static std::string ask(const char* prompt);
	static int askTime(const char* prompt);

	static FreightType parseFreightTypeUI(std::string s);
	static int parseGroupUI(std::string s);
	
	// Menu Option 3 - Show Lists -- NA

	// Menu Option 4 - Compute & Show -- NA

	// Menu Option 7 - Save & Export
	static std::string createCSV(std::string base);

	

public:
	// Menu Option 1 - Load Files
	void loadFilesAndValidate(FreightManager& fm, CargoManager& cm) const;

	// Menu Option 2 - CRUD
	void cargoAdd(CargoManager& cm);
	void cargoEdit(CargoManager& cm);
	void cargoDelete(CargoManager& cm);

	void freightAdd(FreightManager& fm);
	void freightEdit(FreightManager& fm);
	void freightDelete(FreightManager& fm);
	
	// Menu Option 3 - Show Lists
	void showLists(const FreightManager& fm, const CargoManager& cm) const;

	// Menu Option 5 - Show Assigned Plans
	void showPlanByCargoTime(const FreightManager& fm, const CargoManager& cm) const;
	void showPlanByMinimumFreight(const FreightManager& fm, const CargoManager& cm) const;
	
	// Menu Option 7 - Save & Export
	void saveScheduleAndCSV(const FreightManager& fm, const CargoManager& cm, const std::string& txtFilename);
};

