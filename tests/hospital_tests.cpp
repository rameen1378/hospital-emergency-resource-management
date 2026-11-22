#include "Hospital.hpp"

#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

template<class Error, class Action>
void expectError(Action action, const char* message) {
    try { action(); }
    catch (const Error&) { return; }
    throw std::runtime_error(message);
}

int main() {
    try {
        EmergencyDepartment department;
        require(!department.nextWaiting(), "New department should have no waiting patient.");
        department.addResource(std::make_unique<Bed>(101));
        department.addResource(std::make_unique<Equipment>(201));
        expectError<std::invalid_argument>([&] { department.addResource(std::make_unique<Equipment>(101)); }, "Duplicate resource accepted.");
        expectError<std::invalid_argument>([&] { department.registerPatient("  ", Priority::urgent); }, "Blank alias accepted.");
        expectError<std::invalid_argument>([&] { department.registerPatient("Demo", static_cast<Priority>(4)); }, "Invalid priority accepted.");
        const int routine = department.registerPatient("Demo-A", Priority::routine);
        const int first = department.registerPatient("Demo-B", Priority::critical);
        const int second = department.registerPatient("Demo-C", Priority::critical);
        require(routine == 1 && first == 2 && second == 3, "Unexpected patient IDs.");
        require(department.nextWaiting() == first, "Priority order or tie order is wrong.");
        expectError<std::out_of_range>([&] { department.assign(999, first); }, "Unknown resource accepted.");
        expectError<std::out_of_range>([&] { department.assign(101, 999); }, "Unknown patient accepted.");
        department.assign(101, first);
        department.assign(201, first);
        expectError<std::logic_error>([&] { department.assign(101, second); }, "Occupied resource accepted.");
        require(department.status().find("#101 Bed | patient #2") != std::string::npos, "Bed assignment or label not shown.");
        require(department.nextWaiting() == second, "Assigned patient still waiting.");
        department.release(101);
        require(department.nextWaiting() == second, "Patient still holds equipment.");
        department.release(201);
        require(department.nextWaiting() == first, "Patient did not rejoin waiting list after last release.");
        expectError<std::logic_error>([&] { department.release(201); }, "Already-free resource was released.");
        require(department.status().find("#201 Equipment | free") != std::string::npos, "Polymorphic equipment label not shown.");
        const auto path = std::filesystem::path("hospital-test-snapshot.txt");
        department.saveSnapshot(path);
        std::ifstream file(path);
        const std::string snapshot{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
        require(snapshot == department.status(), "Saved snapshot differs from status.");
        file.close();
        expectError<std::runtime_error>([&] { department.saveSnapshot(path / "child.txt"); }, "Invalid export path was accepted.");
        std::filesystem::remove(path);
        std::cout << "All hospital model checks passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
