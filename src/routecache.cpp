#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include "routecache.h"
#include "sqlutils.h"
#include <wx/dir.h>
#include <wx/string.h>
#include <wx/filename.h>
#include <wx/utils.h>

bool DoesTableExist(sqlite3* db, const std::string& tableName) {
    Query query = Query(db, "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='" + tableName + "';");

    if (!query.step()) return false; // Shouldn't happen
    
    return query.get_int(0) > 0;
}

Routecache::Routecache(std::string migrations_dir, std::string db_name) {
    db = NULL;

    this->migrations_dir = migrations_dir;
    this->db_name = db_name;

    try {
        OpenDB();
        CreateEmpty();
        Migrate();
        return;
    } catch (const std::exception& e) {
        std::cerr << e.what() << ". Deleting route cache.\n";
        if (std::remove(db_name.c_str()) != 0) {
            std::cerr << "Unable to delete route cache: " << db_name << "\n";
        }
        CloseDB();
        throw e;
    }
}

Routecache::~Routecache() {
    if (db != NULL) sqlite3_close(db);
}

void Routecache::Insert(
    int target_id,
    double target_distance,
    double target_bearing,
    std::string target_bearing_unit,
    double target_speed,
    double target_course,
    std::string target_course_unit,
    std::string target_distance_unit,
    std::string target_name,
    std::string target_status) {
    Query(db, R"(
        INSERT INTO Targets (
            target_id,
            target_distance,
            target_bearing,
            target_bearing_unit,
            target_speed,
            target_course,
            target_course_unit,
            target_distance_unit,
            target_name,
            target_status) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
      )")
     .bind(1, target_id)
     .bind(2, target_distance)
     .bind(3, target_bearing)
     .bind(4, target_bearing_unit)
     .bind(5, target_speed)
     .bind(6, target_course)
     .bind(7, target_course_unit)
     .bind(8, target_distance_unit)
     .bind(9, target_name)
     .bind(10, target_status)
     .step();
}

void Routecache::OpenDB() {
    int res = sqlite3_open(db_name.c_str(), &db);
    if (res == SQLITE_OK) return;
    db = NULL;
    throw QueryException(db, "open " + db_name);
}

void Routecache::CloseDB() {
    if (db == NULL) return;
    sqlite3_close(db);
    db = NULL;
}

void Routecache::CreateEmpty() {
    if (DoesTableExist(db, "migrations")) return;
    Query(db, R"(
        CREATE TABLE IF NOT EXISTS migrations (
            id integer,
            name text,
            applied datetime default current_timestamp
        );
    )").step();
}

void Routecache::Migrate() {
    Query query = Query(db, "SELECT max(id) FROM migrations;");
    query.step();
    int max_id = query.get_int(0);

    wxString path(migrations_dir);
    wxDir dir(path);
    if (!dir.IsOpened()) {
        throw std::runtime_error("Unable to open migrations directory: " + migrations_dir);
    }

    std::set<wxString> sqlFiles;
    wxString filename;
    for (bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);
         cont;
         cont = dir.GetNext(&filename)) {
        sqlFiles.insert(filename);
    }

    wxString slash(wxFileName::GetPathSeparator());
    
    for (const auto& filename : sqlFiles) {
     int migration_id = std::stoi(std::string(filename.ToUTF8()));
        if (migration_id > max_id) {
            wxString pth = path + slash + filename;
            RunMigration(migration_id, pth);
        }
    }
}

void Routecache::RunMigration(int i, wxString name) {
    std::cerr << "Running migration " << i << ": " << name.ToStdString() << "\n";

    wxFile file(name);
    wxString sql;
    if (!file.IsOpened()) {
        throw std::runtime_error("Unable to open file: " + name.ToStdString());
    }    
    file.ReadAll(&sql, wxConvUTF8);

    Query query = Query(db, std::string(sql.ToUTF8()));
    do {
        query.step();
    } while (query.next());


    std::string strname(name.ToUTF8());
    
    Query(db, R"(
      insert into migrations (id, name) values (?, ?);
    )")
     .bind(1, i)
     .bind(2, strname)
     .step();
}
