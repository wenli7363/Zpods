#include "backup.h"
#include <filesystem>

using namespace zpods;

Status zpods::backup(const char *src_path, const char *target_dir) {
    // check src exist
    const auto src = std::filesystem::path(src_path);
    const auto target = std::filesystem::path(target_dir);

    if (!std::filesystem::exists(src)) {
        return Status::ERROR;
    }

    auto backup = target / src.filename();
    backup.replace_extension(".pods");

    auto options = std::filesystem::copy_options::recursive |
                   std::filesystem::copy_options::skip_symlinks |
                   std::filesystem::copy_options::update_existing;

    std::filesystem::copy(src, backup, options);
    return Status::OK;
}

Status zpods::restore(const char *src_path, const char *target_path) {
    return Status();
}
