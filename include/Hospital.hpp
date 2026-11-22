#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

enum class Priority { routine = 1, urgent, critical };

class Patient {
public:
    Patient(int id, std::string alias, Priority priority);
    int id() const noexcept;
    const std::string& alias() const noexcept;
    Priority priority() const noexcept;

private:
    int id_;
    std::string alias_;
    Priority priority_;
};

class Resource {
public:
    explicit Resource(int id);
    virtual ~Resource() = default;
    int id() const noexcept;
    std::optional<int> occupant() const noexcept;
    virtual std::string_view kind() const noexcept = 0;

private:
    friend class EmergencyDepartment;
    int id_;
    std::optional<int> occupant_;
};

class Bed final : public Resource {
public:
    using Resource::Resource;
    std::string_view kind() const noexcept override;
};

class Equipment final : public Resource {
public:
    using Resource::Resource;
    std::string_view kind() const noexcept override;
};

class EmergencyDepartment {
public:
    int registerPatient(std::string alias, Priority priority);
    std::optional<int> nextWaiting() const noexcept;
    void addResource(std::unique_ptr<Resource> resource);
    void assign(int resourceId, int patientId);
    void release(int resourceId);
    std::string status() const;
    void saveSnapshot(const std::filesystem::path& path) const;

private:
    Resource& findResource(int resourceId);
    std::vector<Patient> patients_;
    std::vector<std::unique_ptr<Resource>> resources_;
    int nextPatientId_ = 1;
};
