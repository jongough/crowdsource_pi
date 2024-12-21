#ifndef _ROUTECACHE_H_
#define _ROUTECACHE_H_

#include <string>
#include <wx/string.h>
#include <sqlite3.h>
#include <mutex>

class Routecache {
public:
    Routecache(std::string migrations_dir, std::string db_name);
    ~Routecache();

    sqlite3* db = nullptr;
    std::mutex lock;

    void Insert(
        int target_id,
        double target_distance,
        double target_bearing,
        std::string target_bearing_unit,
        double target_speed,
        double target_course,
        std::string target_course_unit,
        std::string target_distance_unit,
        std::string target_name,
        std::string target_status);

private:
    std::string migrations_dir;
    std::string db_name;

    void OpenDB();
    void CloseDB();
    void CreateEmpty();
    void Migrate();
    void RunMigration(int i, wxString name);
};

#endif
