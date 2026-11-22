#include "ConsoleApp.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {
std::optional<std::string> readLine(const char* prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) return std::nullopt;
    return line;
}

std::optional<int> readNumber(const char* prompt) {
    const auto line = readLine(prompt);
    if (!line) return std::nullopt;
    std::istringstream input(*line);
    int number;
    char extra;
    if (!(input >> number) || (input >> extra)) throw std::invalid_argument("Enter one whole number.");
    return number;
}
}

ConsoleApp::ConsoleApp() {
    department_.addResource(std::make_unique<Bed>(101));
    department_.addResource(std::make_unique<Equipment>(201));
}

void ConsoleApp::run() {
    std::cout << "Hospital Emergency and Resource Management\n"
                 "Classroom simulation only. Use fictional aliases.\n";
    while (true) {
        try {
            const auto choice = readNumber("\n1 Register | 2 Status | 3 Assign | 4 Release | 5 Export | 0 Exit\n> ");
            if (!choice || *choice == 0) return;
            switch (*choice) {
            case 1: {
                const auto alias = readLine("Demo alias: ");
                if (!alias) return;
                const auto priority = readNumber("Demo priority (1-3): ");
                if (!priority) return;
                if (*priority < 1 || *priority > 3) throw std::invalid_argument("Priority must be 1-3.");
                const int id = department_.registerPatient(*alias, static_cast<Priority>(*priority));
                std::cout << "Registered patient #" << id << '\n';
                break;
            }
            case 2:
                std::cout << department_.status();
                break;
            case 3: {
                const auto resource = readNumber("Resource ID: ");
                if (!resource) return;
                const auto patient = readNumber("Patient ID: ");
                if (!patient) return;
                department_.assign(*resource, *patient);
                std::cout << "Assigned.\n";
                break;
            }
            case 4: {
                const auto resource = readNumber("Resource ID: ");
                if (!resource) return;
                department_.release(*resource);
                std::cout << "Released.\n";
                break;
            }
            case 5:
                department_.saveSnapshot("snapshot.txt");
                std::cout << "Exported snapshot.txt\n";
                break;
            default:
                std::cout << "Choose an option from 0 to 5.\n";
            }
        } catch (const std::exception& error) {
            std::cout << "Error: " << error.what() << '\n';
        }
    }
}
