#pragma once
#include <string>
#include "MatchingEngine.h"
#include "FreightManager.h"
#include "CargoManager.h"
#include "Utils.h"
#include "Freight.h"
#include "Cargo.h"
#include "InputHandler.h"
#include "InputParser.h"
#include "FileHandler.h"

/**
 * ConsoleUI - Thin coordinator for console user interface
 * Delegates to specialized classes: InputHandler, InputParser, FileHandler
 * Responsibility: Coordinate CRUD operations and menu navigation
 */
class ConsoleUI
{
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

