#include "ZpodsLib/src/base/compress.h"
#include "ZpodsLib/src/base/crypto.h"
#include "ZpodsLib/src/base/fs.h"
#include "ZpodsLib/src/core/zpods_core.h"
#include "archive.h"
#include "backup.h"
#include "manager.h"
#include "transform/transform_pipeline_builder.h"

using namespace zpods;

using namespace zpods::fs;

static Status backup_internal(const char* src_dir, const char* target_dir,
                              const BackupConfig& config)
{
    Status status = PodsManager::Instance()->load_pods(target_dir, config);
    if (status != Status::OK)
    {
        return status;
    }

    do
    {
        config.current_pod_path =
            fs::path(target_dir) /
            fmt::format("{}{}", get_current_timestamp(), POD_FILE_SUFFIX);
    } while (fs::exists(config.current_pod_path.c_str()));

    // 1. archive files of src_path to a single file in target_dir
    TransformPipelineBuilder builder;

    builder.enable_archive(src_dir, target_dir, config);

    /* status = zpods::archive(src_dir, target_dir, config); */
    /* if (status == Status::NO_NEW_TO_ARCHIVE) */
    /* { */
    /*     return Status::OK; */
    /* } */
    /* else if (status != Status::OK) */
    /* { */
    /*     return status; */
    /* } */

    /* let_mut bytes = fs::read_from_file(config.current_pod_path.c_str()); */
    /* std::unique_ptr<byte[]> buf; */
    /* size_t buf_len; */
    // 2. compress if needed
    if (config.compress)
    {
        builder.enable_compress();
        /* std::tie(buf_len, buf) = compress({(p_byte)bytes.data(),
         * bytes.size()}); */
        /* bytes = {as_c_str(buf.get()), buf_len}; */
        /* spdlog::info("file compression succeeded : {}", */
        /*              config.current_pod_path.c_str()); */
    }

    // 3. encrypt if needed
    if (config.crypto_config)
    {
        let key = config.crypto_config->key_;
        let iv = config.crypto_config->iv_;
        builder.enable_encrypt(key, iv);
        /* let_ref conf = config.crypto_config; */
        /* let cipher = encrypt({as_c_str(bytes.data()), bytes.size()},
         * conf->key_, */
        /*                      conf->iv_); */
        /* if (!cipher.has_value()) */
        /* { */
        /*     spdlog::error("file encryption failed: {}", */
        /*                   config.current_pod_path.c_str()); */
        /*     return Status::ERROR; */
        /* } */
        /* bytes = {cipher->data(), cipher->size()}; */
        /* spdlog::info("file encryption succeeded : {}", */
        /*              config.current_pod_path.c_str()); */
    }

    // output to target file
    {
        let[status, bytes] = builder.build()->execute({});
        if (status != Status::OK)
        {
            return status;
        }

        let_mut ofs = fs::open_or_create_file_as_ofs(
            config.current_pod_path.c_str(), fs::ios::binary);

        let_mut header = PodHeader::from(config);
        calculate_checksum(header.checksum,
                           {(p_byte)bytes.data(), bytes.size()});
        if (config.crypto_config)
        {
            calculate_password_verify_token(header, config.crypto_config->key_);
        }
        ofs.write(as_c_str(header), sizeof(header));
        ofs << bytes;
    }

    spdlog::info("successfully backup {} to {}", src_dir, target_dir);
    return Status::OK;
}

Status zpods::backup(const char* dest_dir, const BackupConfig& config)
{
    fs::create_directory_if_not_exist(dest_dir);
    ZPODS_ASSERT(fs::is_directory(dest_dir));

    let dir_to_backup = config.tree_dir;

    let pods_dir_name =
        fmt::format("{}-PODS", fs::get_last_part(dir_to_backup));
    let pods_dir = fs::path(dest_dir) / pods_dir_name;

    if (!fs::exists(pods_dir.c_str()))
    {
        PodsManager::Instance()->create_pods(pods_dir);
    }

    let st = backup_internal(dir_to_backup.c_str(), pods_dir.c_str(), config);
    if (st != Status::OK)
    {
        return st;
    }

    PodsManager::Instance()->record_mapping(dir_to_backup, pods_dir, config);
    return Status::OK;
}

Status zpods::restore(const char* pods_dir, const char* target_dir,
                      BackupConfig config)
{
    fs::create_directory_if_not_exist(target_dir);
    ZPODS_ASSERT(fs::is_directory(target_dir));

    Status status = PodsManager::Instance()->load_pods(pods_dir, config);
    if (status != Status::OK)
    {
        return status;
    }

    // record the residing pod paths of the peas
    // this step is necessary because the peas may be archived in different pods
    // we only need to process the pods that contain the peas that we want to
    // restore
    std::unordered_map<fs::zpath, std::unordered_set<Pea>> pod_to_peas;
    for (let_ref pea : PodsManager::Instance()->tree_state_of_pods(pods_dir))
    {
        pod_to_peas[pea.resident_pod_path].insert(pea);
    }

    for (const auto& [pod_path, pea_set] : pod_to_peas)
    {
        status = zpods::process_origin_zpods_bytes(
            pod_path.c_str(), config, [&](auto& bytes) {
                return zpods::foreach_pea_in_pod_bytes(
                    (p_byte)bytes.c_str(), [&](const PeaHeader& header) {
                        let mts = header.get_last_modified_ts();
                        let pea = Pea{.last_modified_ts = mts,
                                      .rel_path = header.get_path()};
                        if (!pea_set.contains(pea))
                        {
                            return Status::OK;
                        }
                        let path = header.get_path();
                        let full_path = fs::path(target_dir) / path;
                        spdlog::info("unarchived file {}", full_path.c_str());
                        let base_name = fs::get_base_name(full_path.c_str());
                        fs::create_directory_if_not_exist(base_name.c_str());
                        if (fs::exists(full_path))
                        {
                            // if the file's last modified ts is not smaller
                            // than the pea, skip it
                            let ts = fs::last_modified_timestamp(full_path);
                            if (ts >= mts)
                            {
                                return Status::OK;
                            }
                        }

                        { // otherwise, this is a new file
                            std::ofstream ofs(full_path);
                            let bytes = header.get_data();
                            ofs.write((char*)bytes.data(), bytes.size());
                        }

                        // set last modified timestamp to pea's ts
                        fs::set_last_modified_timestamp(full_path, mts);
                        ZPODS_ASSERT(mts ==
                                     fs::last_modified_timestamp(full_path));
                        return Status::OK;
                    });
            });
        if (status != Status::OK)
        {
            return status;
        }
    }

    return Status::OK;
}

Status zpods::sync_backup(const char* target_dir, const BackupConfig& config)
{
    // make sure the backup file is up-to-date
    zpods::backup(target_dir, config);

    // backup callback if the src_path is modified
    let callback = [&](const char* path) { zpods::backup(target_dir, config); };

    // create a watcher in a thread for each path
    //    std::vector<std::thread> threads;
    //    for (let_ref path: config.filter.paths) {
    //        threads.emplace_back([=] {
    //            FsWatcher watcher(path.c_str(), {
    //                    .on_file_create = callback,
    //                    .on_file_delete = callback,
    //                    .on_file_modify = callback,
    //                    .on_dir_create = callback,
    //                    .on_dir_delete = callback,
    //            });
    //        });
    //    }
    //    for (let_mut_ref item: threads) { item.join(); }

    return Status::OK;
}
