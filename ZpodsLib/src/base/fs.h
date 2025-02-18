//
// Created by code4love on 23-9-24.
//

#ifndef ZPODS_FS_H
#define ZPODS_FS_H

#include "pch.h"

#include <filesystem>
#include <regex>
#include <utime.h>

#ifdef __linux__

#include <sys/inotify.h>
#include <sys/stat.h>

#endif

namespace zpods
{
namespace fs
{

namespace ios
{
constexpr auto binary = std::ios::binary;
constexpr auto text = std::ios::out;
} // namespace ios

using openmode = std::ios::openmode;

using copy_options = std::filesystem::copy_options;

using zpath = std::filesystem::path;

inline auto copy(const char* src, const char* target, copy_options options)
{
    std::filesystem::copy(src, target, options);
}

inline auto path(const char* path) -> zpath
{
    return {path};
}

inline auto path(const std::string& path) -> zpath
{
    return {path};
}

inline auto last_modified_timestamp(const fs::zpath& path) -> uint32_t
{
    struct stat statbuf;
    stat(path.c_str(), &statbuf);
    return statbuf.st_mtime;
    /* let t = std::filesystem::last_write_time(path); */
    /* // 将文件最后修改时间从时间点转换为时间戳 */
    /* let duration = t.time_since_epoch(); */
    /* let modTimeTimestamp = */
    /*     std::chrono::duration_cast<std::chrono::seconds>(duration).count();
     */
    /**/
    /* return static_cast<uint32_t>(modTimeTimestamp); */
}

inline auto set_last_modified_timestamp(const fs::zpath& path, uint32_t ts)
{
    time_t time = ts;
    struct utimbuf new_times;
    new_times.actime = time;
    new_times.modtime = time;
    utime(path.c_str(), &new_times);
}

inline auto exists(const fs::zpath& path)
{
    return std::filesystem::exists(path);
}

inline auto create_directory_if_not_exist(const fs::zpath& path)
{
    if (!fs::exists(path))
    {
        std::filesystem::create_directories(path);
    }
}

inline auto remove_file(const char* path)
{
    return std::filesystem::remove(path);
}

inline auto remove_all(const char* path)
{
    return std::filesystem::remove_all(path);
}

inline auto get_last_part(const zpath& path) -> std::string
{
    if (path.has_filename())
    {
        return path.filename().string();
    }
    // for 'aaa/bbb/' return 'bbb'
    return path.parent_path().filename().string();
}

inline auto get_base_name(const zpath& path) -> std::string
{
    return path.parent_path().c_str();
}

auto open_or_create_file_as_ofs(auto&& path, zpods::fs::openmode mode)
    -> std::ofstream
{
    let base = get_base_name(path);
    if (!exists(base.c_str()))
    {
        create_directory_if_not_exist(base.c_str());
    }
    std::ofstream ofs(path, mode);
    ZPODS_ASSERT(ofs.is_open());
    return ofs;
}

auto open_file_as_ifs(const char* path, openmode mode) -> std::ifstream;

inline bool is_directory(const std::string& path)
{
    return std::filesystem::is_directory(path);
}

inline auto directory_iterator(const std::string& path)
{
    return std::filesystem::directory_iterator(path);
}

// given a file with path: "xx/yy/zz", retrieve all files with path like:
// "xx/yy/zz*"
inline auto get_file_family(const char* file_path)
{
    std::vector<zpath> paths;
    let dir = path(file_path).parent_path();

    for (const auto& entry : directory_iterator(dir))
    {
        if (strstr(entry.path().c_str(), file_path))
        {
            paths.push_back(entry.path());
        }
    }

    return paths;
}

inline auto get_file_size(const zpath& path)
{
    return std::filesystem::file_size(path);
}

auto read(const char* path, std::string_view buf);

auto write(const char* path, std::string_view buf);

auto read_from_file(const char* path) -> std::string;

auto relative(const char* path, const char* base) -> const char*;

bool is_same_content(const char* path1, const char* path2);

using FileType = std::filesystem::file_type;

inline auto make_year_month_day(int year, int month, int day)
{
    return std::chrono::year_month_day(std::chrono::year(year),
                                       std::chrono::month(month),
                                       std::chrono::day(day));
}

static inline auto file_time_to_date(std::filesystem::file_time_type time)
{
    let sys_time = decltype(time)::clock::to_sys(time);
    let tt = std::chrono::system_clock::to_time_t(sys_time);
    let tm = std::localtime(&tt);

    let year = std::chrono::year(tm->tm_year + 1900);
    let month = std::chrono::month(tm->tm_mon + 1);
    let day = std::chrono::day(tm->tm_mday);

    let date = std::chrono::year_month_day(year, month, day);
    return date;
}

// filter files with given conditions
struct FilesFilter
{
    std::unordered_set<FileType> types{FileType::regular}; ///< types to backup
    std::vector<std::string>
        re_list; ///< regular expressions for names filtering
    std::chrono::year_month_day min_date = make_year_month_day(0, 1, 1);
    ///< only backup files that are modified after min_time
    std::chrono::year_month_day max_date = make_year_month_day(9999, 12, 31);
    ///< only backup files that are modified before max_time
    uintmax_t min_size = 0; ///< only backup files that are larger than min_size
    uintmax_t max_size =
        (uintmax_t)-1; ///< only backup files that are smaller than max_size

    template <typename OStream>
        requires requires(OStream& os) { os << std::declval<std::string>(); }
    void serialize(OStream& os) const
    {
        os << "types: ";
        for (let_ref type : types)
        {
            os << (int)type << " ";
        }
        os << '\n';
        os << "min_date: " << (int)min_date.year() << " "
           << (unsigned)min_date.month() << " " << (unsigned)min_date.day()
           << '\n';
        os << "max_date: " << (int)max_date.year() << " "
           << (unsigned)max_date.month() << " " << (unsigned)max_date.day()
           << '\n';
        os << "min_size: " << min_size << '\n';
        os << "max_size: " << max_size << '\n';
        os << "re_list: ";
        for (let_ref re : re_list)
        {
            os << re << " ";
        }
        os << '\n';
    }

    template <typename IStream>
        requires requires(IStream& is, std::string s) { is >> s; }
    void deserialize(IStream& is)
    {
        std::string line;
        std::getline(is, line);
        std::istringstream iss(line);
        std::string s;
        iss >> s;
        while (iss >> s)
        {
            types.insert((FileType)std::stoi(s));
        }

        int year, month, day;

        std::getline(is, line);
        iss = std::istringstream(line);
        iss >> s;
        iss >> year >> month >> day;
        min_date = make_year_month_day(year, month, day);

        std::getline(is, line);
        iss = std::istringstream(line);
        iss >> s;
        iss >> year >> month >> day;
        max_date = make_year_month_day(year, month, day);

        std::getline(is, line);
        iss = std::istringstream(line);
        iss >> s;
        iss >> min_size;

        std::getline(is, line);
        iss = std::istringstream(line);
        iss >> s;
        iss >> max_size;

        std::getline(is, line);
        iss = std::istringstream(line);
        iss >> s;
        while (iss >> s)
        {
            re_list.push_back(s);
        }
    }
};

class FileCollector
{
  public:
    using iterator = std::vector<zpath>::iterator;

    explicit FileCollector(const fs::zpath& path, FilesFilter filter)
        : filter_(std::move(filter))
    {
        scan_path(path);
    }

    bool add_path(const std::string& path)
    {
        let status = std::filesystem::symlink_status(path);
        FileType type = status.type();

        spdlog::debug("PATH: {}, TYPE: {}", path, (int)type);

        // filter with file type
        if (!filter_.types.contains(type))
        {
            return false;
        }

        // filter with size
        uintmax_t bytes_cnt = 0;

        if (status.type() != FileType::directory)
        {
            bytes_cnt = std::filesystem::file_size(path);
            if (bytes_cnt > filter_.max_size || bytes_cnt < filter_.min_size)
            {
                return false;
            }
        }

        // filter with date
        std::filesystem::file_time_type time =
            std::filesystem::last_write_time(path);
        let date = file_time_to_date(time);

        if (date > filter_.max_date || date < filter_.min_date)
        {
            return false;
        }

        // filter with names
        if (!filter_.re_list.empty())
        {
            bool found = false;
            for (const auto& item : filter_.re_list)
            {
                if (std::regex_search(fs::path(path.c_str()).filename().c_str(),
                                      std::regex(item)))
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                return false;
            }
        }

        spdlog::info("{}: {}-{}-{} size: {}", path, (int)date.year(),
                     (unsigned)date.month(), (unsigned)date.day(), bytes_cnt);
        paths_.emplace_back(path);

        return true;
    }

    void scan_path(const std::string& path)
    {
        add_path(path);

        // for non dir, no need to dfs
        // for dir, continue to dfs
        if (is_directory(path))
        {
            for (const auto& entry : directory_iterator(path))
            {
                scan_path(entry.path());
            }
        }
    }

    std::vector<zpath> paths() { return std::move(paths_); }

  private:
    std::vector<zpath> paths_;
    FilesFilter filter_;
};
} // namespace fs

class FsWatcher
{
  public:
    struct Callback
    {
        std::function<void(const char* path)> on_file_create;
        std::function<void(const char* path)> on_file_delete;
        std::function<void(const char* path)> on_file_modify;
        std::function<void(const char* path)> on_dir_create;
        std::function<void(const char* path)> on_dir_delete;
    } callback_;

    [[noreturn]] FsWatcher(const char* path, Callback callback)
        : path_(path), callback_(std::move(callback))
    {
#ifdef __linux__
        fd_ = inotify_init();
        ZPODS_ASSERT(fd_ >= 0);
        fs::FilesFilter filter;
        filter.types.insert(fs::FileType::directory);
        for (let_ref p : fs::FileCollector(path_, std::move(filter)).paths())
        {
            // add watch for all directories
            add_watch_for(p.c_str());
        }

        while (true)
        {
            size_t i = 0;
            let len = read(fd_, buf_, sizeof(buf_));

            while (i < len)
            {
                let event = (struct inotify_event*)&buf_[i];
                let dir_path = fs::path(path_) / (event->name);
                let file_path = fs::path(path_) / (event->name);
                if (event->len)
                {
                    if (event->mask & IN_CREATE)
                    {
                        if (event->mask & IN_ISDIR)
                        {
                            // new directory created, add watch for it!
                            spdlog::info("The directory {} was created.",
                                         dir_path.c_str());
                            add_watch_for(dir_path.c_str());
                            callback_.on_dir_create(dir_path.c_str());
                        }
                        else
                        {
                            spdlog::info("The file {} was created.",
                                         event->name);
                            callback_.on_file_create(file_path.c_str());
                        }
                    }
                    else if (event->mask & IN_DELETE)
                    {
                        if (event->mask & IN_ISDIR)
                        {
                            // remove watch for the deleted directory!
                            let dir_path = fs::path(path_) / (event->name);
                            spdlog::info("The directory {} was deleted.",
                                         dir_path.c_str());
                            remove_watch_for(dir_path.c_str());
                            callback_.on_dir_delete(dir_path.c_str());
                        }
                        else
                        {
                            spdlog::info("The file {} was deleted.",
                                         event->name);
                            callback_.on_file_delete(file_path.c_str());
                        }
                    }
                    else if (event->mask & IN_MODIFY)
                    {
                        if (event->mask & IN_ISDIR)
                        {
                            spdlog::info("The directory {} was modified.",
                                         event->name);
                        }
                        else
                        {
                            spdlog::info("The file {} was modified.",
                                         event->name);
                            callback_.on_file_modify(file_path.c_str());
                        }
                    }
                }
                i += sizeof(struct inotify_event) + event->len;
            }
        }
#else
#endif
    }

    void add_watch_for(const char* dir)
    {
        let path = absolute(fs::path(dir));
        ZPODS_ASSERT(path_wd_map_.find(path) == path_wd_map_.end());
#ifdef __linux__
        let wd = inotify_add_watch(fd_, path.c_str(), IN_ALL_EVENTS);
        ZPODS_ASSERT(wd >= 0);
        path_wd_map_.emplace(path, wd);
        spdlog::info("add watch for {}", path.c_str());
#endif
    }

    void remove_watch_for(const char* dir)
    {
        let path = absolute(fs::path(dir));
#ifdef __linux__
        let wd = path_wd_map_[path];
        inotify_rm_watch(fd_, wd);
        path_wd_map_.erase(path);
        spdlog::info("remove watch for {}", path.c_str());
#endif
    }

    ~FsWatcher()
    {
        for (let_ref[_, wd] : path_wd_map_)
        {
            inotify_rm_watch(fd_, wd);
        }
        close(fd_);
    }

  private:
    const char* path_;
    int fd_;
    std::unordered_map<fs::zpath, int> path_wd_map_;
    char buf_[1024];
};

} // namespace zpods

// 定义字面量函数
constexpr unsigned long long operator"" _KB(unsigned long long x)
{
    return x * 1024;
}

constexpr unsigned long long operator"" _KB(long double x)
{
    return static_cast<unsigned long long>(x * 1024.0);
}

constexpr unsigned long long operator"" _MB(unsigned long long x)
{
    return x * 1024 * 1024;
}

constexpr unsigned long long operator"" _MB(long double x)
{
    return static_cast<unsigned long long>(x * 1024.0 * 1024.0);
}

constexpr unsigned long long operator"" _GB(unsigned long long x)
{
    return x * 1024 * 1024 * 1024;
}

constexpr unsigned long long operator"" _GB(long double x)
{
    return static_cast<unsigned long long>(x * 1024.0 * 1024.0 * 1024.0);
}

#endif // ZPODS_FS_H
