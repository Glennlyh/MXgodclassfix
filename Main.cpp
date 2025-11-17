#include <iostream>
#include <string>

#include "Utils.h"
#include "FreightManager.h"
#include "CargoManager.h"
#include "MatchingEngine.h"
#include "ConsoleUI.h"

int main() {
    FreightManager fm;
    CargoManager cm;
	ConsoleUI ui;

    std::cout << "Freight/Cargo Scheduler (Manager + Engine)\n";

    while (true) {
        std::cout << "\n===============Menu===============\n"
            << "1) Load files (freight.txt, cargo.txt)\n"
            << "2) Add/Edit/Delete\n"
            << "3) Show lists\n"
            << "4) View assigned schedule\n"
            << "5) View unassigned schedule\n"
            << "6) Save & Export schedule (TXT + CSV)\n"
            << "7) Quit\n"
            << "Choice: ";
        std::string choice; std::getline(std::cin, choice);

		// Menu - Option 1 - Load Freights
        if (choice == "1") {
            ui.loadFilesAndValidate(fm, cm);
        }

        // Menu - Option 2 - Add/Edit/Delete
        else if (choice == "2") {
            std::cout << "Edit which list?  a)cargo  b)freight  c)back : ";
            std::string s;
            std::getline(std::cin, s);

            if (s == "a") {
                std::cout << "i)add  ii)edit  iii)delete  (other=back): ";
                std::string op;
                std::getline(std::cin, op);
                if (op == "i")
                    ui.cargoAdd(cm);
                else if (op == "ii")
                    ui.cargoEdit(cm);
                else if (op == "iii")
                    ui.cargoDelete(cm);
            }
            else if (s == "b") {
                std::cout << "i)add  ii)edit  iii)delete  (other = back) : ";
                std::string op;
                std::getline(std::cin, op);
                if (op == "i")
                    ui.freightAdd(fm);
                else if (op == "ii")
                    ui.freightEdit(fm);
                else if (op == "iii")
                    ui.freightDelete(fm);
            }
        }

		// Menu - Option 3 - Show Lists
        else if (choice == "3") {
            ui.showLists(fm, cm);
        }
        

		// Menu - Option 4 - Show Assigned Schedule
        else if (choice == "4") {
            std::cout << "Show schedule:\n"
                << "  a) Sorted by cargo arrival time\n"
                << "  b) Sorted by minimum freight\n"
                << "  c) Back\n"
                << "Select: ";

            std::string viewAPlanOption;
            std::getline(std::cin, viewAPlanOption);

            if (viewAPlanOption == "a") {
                ui.showPlanByCargoTime(fm, cm);
            }
            else if (viewAPlanOption == "b") {
                ui.showPlanByMinimumFreight(fm, cm);
            }
        }

        // Menu - Option 5 - View Unassigned Schedule
        else if (choice == "5") {
            std::cout << "View schedule:\n"
                << "a) Freights not at full capacity : \n"
                << "b) Cargos not yet assigned : \n"
                << "c) Back\n"
                << "Select: ";

            std::string viewUPlanOption;
            std::getline(std::cin, viewUPlanOption);

            if (viewUPlanOption == "a") {
                MatchingEngine::printFreightsNotFull(fm.list(), cm.list());
            }
            else if (viewUPlanOption == "b") {
                MatchingEngine::printUnassignedCargos(fm.list(), cm.list());
            }
        }

        // Menu - Option 6 - Save & Export
        else if (choice == "6") {
            std::string filename;
            std::cout << "Output TXT filename: ";
            std::getline(std::cin, filename);

            ui.saveScheduleAndCSV(fm, cm, filename);
        }

        // Menu - Option 7 - Quit
        else if (choice == "7") {
            break;
        }

        else {
            std::cout << "Unknown choice.\n";
        }
    }

    std::cout << "ENDED.\n";
    return 0;
}
