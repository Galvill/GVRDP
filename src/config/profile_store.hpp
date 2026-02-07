#pragma once

#include "config/connection_profile.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace gvrdp {

class ProfileStore {
public:
    explicit ProfileStore(const std::filesystem::path& config_dir);

    std::vector<ConnectionProfile> load_all() const;
    bool save(const ConnectionProfile& profile) const;
    bool remove(const std::string& profile_name) const;

private:
    std::filesystem::path profiles_dir_;

    std::filesystem::path profile_path(const std::string& name) const;
    static std::string sanitize_filename(const std::string& name);
};

}  // namespace gvrdp
