#include "Pager.cpp"
#include <map>
#include <filesystem>
#include <gtest/gtest.h>
namespace engine {
    namespace fs = std::filesystem;
    class Table {
    };
    class Database {
        string dbName_;
        map<Table, string> tableFiles_;
        public:
        void create(fs::directory_entry root, string dbName) {
            fs::directory_entry dbDir = fs::directory_entry(root.path() / dbName);
            if (dbDir.exists()) {
                cout << "Database \"" << dbName << "\" already exists" << endl;
            } else {
                if (!fs::create_directory(root.path() / dbName)) {
                    cout << "Couldn't create database" << endl;
                    exit(1);
                }
            }
        }
        string getDbName() { return dbName_; }
        Database(string dbName) {
            dbName_ = dbName;
        }
    };
    class Engine {
        string defaultSysqlFolderName = ".sysql.db";
        fs::directory_entry rootDir;
        Database *currentDb;
        public:
        fs::directory_entry getRootDir() { return rootDir; }
        Engine() {
            rootDir = fs::directory_entry(fs::current_path() / defaultSysqlFolderName); 
            if (!rootDir.exists())
                if (!fs::create_directory(rootDir.path())) {
                    cout << "Couldn't create .sysqldb directory" << endl;
                    exit(1);
            } 
            rootDir.refresh();
        }
        void createDb(string dbName) {
            Database *db = new Database( dbName);
            db->create(rootDir, dbName);
            delete db;
        }
        void openDb(string dbName) {
            dbName = "";
            return;
        }
    };
}

// int main() {
//     engine::Engine *engine = new engine::Engine();
//     engine->createDb("first_db");
//     engine->createDb("second_db");
//     engine->createDb("third_db");
//     delete engine;
//     return 0;
// }
