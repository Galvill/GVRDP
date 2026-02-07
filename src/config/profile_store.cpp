#include "config/profile_store.hpp"

#include "util/logger.hpp"

#include <fstream>

namespace gvrdp {

ProfileStore::ProfileStore(const std::filesystem::path& config_dir)
    : profiles_dir_(config_dir / "profiles") {
    std::error_code ec;
    std::filesystem::create_directories(profiles_dir_, ec);
    if (ec) {
        LOG_WARN("Failed to create profiles directory: {}", ec.message());
    }
}

std::vector<ConnectionProfile> ProfileStore::load_all() const {
    std::vector<ConnectionProfile> profiles;
    std::error_code ec;

    for (const auto& entry : std::filesystem::directory_iterator(profiles_dir_, ec)) {
        if (entry.path().extension() != ".json") continue;
        try {
            std::ifstream file(entry.path());
            if (!file.is_open()) continue;
            nlohmann::json j;
            file >> j;
            profiles.push_back(j.get<ConnectionProfile>());
        } catch (const std::exception& e) {
            LOG_WARN("Failed to load profile {}: {}", entry.path().string(), e.what());
        }
    }
    return profiles;
}

bool ProfileStore::save(const ConnectionProfile& profile) const {
    try {
        auto path = profile_path(profile.name);
        std::ofstream file(path);
        if (!file.is_open()) {
            LOG_ERROR("Cannot open profile file for writing: {}", path.string());
            return false;
        }
        nlohmann::json j = profile;
        // Remove password from persisted JSON
        j.erase("password");
        file << j.dump(4);
        LOG_INFO("Saved profile: {}", profile.name);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to save profile {}: {}", profile.name, e.what());
        return false;
    }
}

bool ProfileStore::remove(const std::string& profile_name) const {
    std::error_code ec;
    auto path = profile_path(profile_name);
    if (std::filesystem::remove(path, ec)) {
        LOG_INFO("Removed profile: {}", profile_name);
        return true;
    }
    LOG_WARN("Failed to remove profile {}: {}", profile_name, ec.message());
    return false;
}

std::filesystem::path ProfileStore::profile_path(const std::string& name) const {
    return profiles_dir_ / (sanitize_filename(name) + ".json");
}

std::string ProfileStore::sanitize_filename(const std::string& name) {
    std::string result;
    result.reserve(name.size());
    for (char c : name) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_') {
            result += c;
        } else {
            result += '_';
        }
    }
    return result.empty() ? "untitled" : result;
}

}  // namespace gvrdp
