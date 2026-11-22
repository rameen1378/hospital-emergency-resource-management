#include "Hospital.hpp"

#include <algorithm>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>

Patient::Patient(int id, std::string alias, Priority priority)
    : id_(id), alias_(std::move(alias)), priority_(priority) {
    if (id_ <= 0 || alias_.find_first_not_of(" \t") == std::string::npos ||
        alias_.size() > 40 || alias_.find_first_of("\r\n") != std::string::npos ||
        priority_ < Priority::routine || priority_ > Priority::critical) {
        throw std::invalid_argument("Use a nonblank demo alias (max 40 bytes) and priority 1-3.");
    }
}

int Patient::id() const noexcept { return id_; }
const std::string& Patient::alias() const noexcept { return alias_; }
Priority Patient::priority() const noexcept { return priority_; }

Resource::Resource(int id) : id_(id) {
    if (id <= 0) throw std::invalid_argument("Resource ID must be positive.");
}

int Resource::id() const noexcept { return id_; }
std::optional<int> Resource::occupant() const noexcept { return occupant_; }
std::string_view Bed::kind() const noexcept { return "Bed"; }
std::string_view Equipment::kind() const noexcept { return "Equipment"; }

int EmergencyDepartment::registerPatient(std::string alias, Priority priority) {
    if (nextPatientId_ == std::numeric_limits<int>::max()) {
        throw std::overflow_error("No patient IDs remain.");
    }
    patients_.emplace_back(nextPatientId_, std::move(alias), priority);
    return nextPatientId_++;
}

std::optional<int> EmergencyDepartment::nextWaiting() const noexcept {
    const Patient* next = nullptr;
    for (const auto& patient : patients_) {
        const auto assigned = std::any_of(resources_.begin(), resources_.end(),
            [&patient](const auto& resource) { return resource->occupant() == patient.id(); });
        if (!assigned && (!next || patient.priority() > next->priority())) next = &patient;
    }
    return next ? std::optional<int>{next->id()} : std::nullopt;
}

void EmergencyDepartment::addResource(std::unique_ptr<Resource> resource) {
    if (!resource) throw std::invalid_argument("Resource is required.");
    const auto duplicate = std::any_of(resources_.begin(), resources_.end(),
        [&resource](const auto& existing) { return existing->id() == resource->id(); });
    if (duplicate) throw std::invalid_argument("Resource ID already exists.");
    resources_.push_back(std::move(resource));
}

Resource& EmergencyDepartment::findResource(int resourceId) {
    for (const auto& resource : resources_) {
        if (resource->id() == resourceId) return *resource;
    }
    throw std::out_of_range("Resource ID not found.");
}

void EmergencyDepartment::assign(int resourceId, int patientId) {
    auto& resource = findResource(resourceId);
    const auto exists = std::any_of(patients_.begin(), patients_.end(),
        [patientId](const auto& patient) { return patient.id() == patientId; });
    if (!exists) throw std::out_of_range("Patient ID not found.");
    if (resource.occupant_) throw std::logic_error("Resource is already occupied.");
    resource.occupant_ = patientId;
}

void EmergencyDepartment::release(int resourceId) {
    auto& resource = findResource(resourceId);
    if (!resource.occupant_) throw std::logic_error("Resource is already free.");
    resource.occupant_.reset();
}

std::string EmergencyDepartment::status() const {
    std::ostringstream report;
    report << "Patients (priority: 1 routine, 2 urgent, 3 critical)\n";
    for (const auto& patient : patients_) {
        report << "  #" << patient.id() << ' ' << patient.alias()
               << " | priority " << static_cast<int>(patient.priority()) << '\n';
    }
    report << "Resources\n";
    for (const auto& resource : resources_) {
        report << "  #" << resource->id() << ' ' << resource->kind() << " | ";
        if (const auto occupant = resource->occupant()) report << "patient #" << *occupant;
        else report << "free";
        report << '\n';
    }
    if (const auto next = nextWaiting()) report << "Next waiting: patient #" << *next << '\n';
    else report << "Next waiting: none\n";
    return report.str();
}

void EmergencyDepartment::saveSnapshot(const std::filesystem::path& path) const {
    std::ofstream file(path);
    if (!file) throw std::runtime_error("Could not open the snapshot file.");
    file << status();
    file.close();
    if (!file) throw std::runtime_error("Could not finish writing the snapshot file.");
}
